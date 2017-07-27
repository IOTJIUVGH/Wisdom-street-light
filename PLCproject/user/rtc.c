#include "stm32f10x.h"

#include "rtc.h"  
#include "stdio.h"  
  
tm timer;                   //定义时钟结构体，主函数可以直接调用 
u32 m_OpenTime = 0;
u8 g_Bkp_flag = 0;
//平年的月份、日期表
const u8 Days_Table[12]={31,28,31,30,31,30,31,31,30,31,30,31};  
const u8 Month_Table[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};  
  
const u8* Week_Table[7]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};  
  
//月修正数据表                                                                           
u8 const _Week[12]={0,3,3,6,1,4,6,2,5,0,3,5};   
u32 cnt1;  
void Rtc_Init(void)  
{  
	cnt1 = RTC->CNTH;  
    cnt1 <<= 16;  
    cnt1 += RTC->CNTL;  
	
	if(!cnt1)
	{
		RCC->APB1ENR |= 1<<28; //使能电源时钟  
		RCC->APB1ENR |= 1<<27;  //使能备份时钟


		PWR->CR |= 1<<8;       //取消备份区写保护

//    RCC->BDCR |= 1<<16;  //备份区软件复位
//    RCC->BDCR |= ~(1<<16);   //备份区软件复位结束  

		RCC->BDCR |= 1<<0;     //开启外部低速振荡器 
		while(!(RCC->BDCR & 0x02));  //等待外部时钟就绪  
	
		//printf("init");
		RCC->BDCR |= 1<<8;         //LSE作为RTC时钟 
		RCC->BDCR |= 1<<15;            //RTC时钟使能  

		while(!(RTC->CRL & (1<<5)));   //等待RTC寄存器操作完成
		while(!(RTC->CRL & (1<<3)));   //等待RTC操作同步  


		RTC->CRH |= 0x01;                //允许秒中断    

		while(!(RTC->CRL & (1<<5)));   //等待RTC寄存器操作完成 
	
	
		RTC->CRL |=  1<<4;             //允许配置 
	
		RTC->PRLH = 0x0000;                
		RTC->PRLL = 32767;     //预分频装载值          
	
		
		RTC->CRL &= ~(1<<4);           //退出配置模式开始，开始更新RTC寄存器  
		while(!(RTC->CRL & (1<<5)));   //等待RTC寄存器操作完成 
		PWR->CR &= ~(1<<8);
		
	}
	else{
		while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
		RTC->CRH|=0X01;  		  //允许秒中断
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
	}  
 }
void Rtc_Init2(void)
{ 
	cnt1 = RTC->CNTH;  
    cnt1 <<= 16;  
    cnt1 += RTC->CNTL;  
	if(cnt1)
	{
		while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
		RTC->CRH|=0X01;  		  //允许秒中断
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
	}
	RTC__NVIC_Init();
}

 void Bkp_Init(void)//初始化备份区
 {
	RCC->APB1RSTR |= 1<<27;        //复位BKP寄存器
    RCC->APB1RSTR &= ~(1<<27);  
  
    RCC->APB1ENR|=1<<28;     //使能电源时钟       
    RCC->APB1ENR|=1<<27;     //使能BKP时钟     
	PWR->CR|=1<<8;//取消备份区写保护	
}
//设置RTC开始计时时间 
void Rtc_TIME_Set(u16 year,u8 month,u8 date,u8 hour,u8 minute, u8 second)  
{  
        u32 sec;      
  
        sec = Date_TO_Sec(year,month,date,hour,minute,second);  
      
        //printf("\nRtc TIME Set  Sec = %x\n",sec);  
      
        RCC->APB1ENR |= 1<<28;                         //使能电源时钟 
        RCC->APB1ENR |= 1<<27;                         //使能后备区时钟 
        PWR->CR |= 1<<8;                               //取消写保护 
      
        RTC-> CRL |= 1<<4;                             //允许配置 
          
        RTC-> CNTL = sec&0xffff;                     //取低16位
        RTC-> CNTH = sec>>16;                          //取高16位 
      
        RTC-> CRL &= ~(1<<4);                          //开始RTC寄存器更新  
      
        while(!(RTC->CRL&(1<<5)));                     //等待RTC寄存器更新完成  
		PWR->CR &= ~(1<<8);
}  
  
  
  
  
//判断是否是闰年
//返回: 1,是的     0,不是  
u8 Is_LeapYear(u16 year)  
{               
    if(year%4==0)              
    {   
        if(year%100==0)   
        {   
            if(year%400==0)  
                return 1;          
            else   
                return 0;     
        }else{   
            return 1;     
        }  
    }else{  
         return 0;    
    }  
}  
  
  
//将时间转化到19701月一号的总秒数  
//Bugs:此函数秒数会多加20左右，所以函数返回值做了改动
 
u32 Date_TO_Sec(u16 year,u8 month,u8 date,u8 hour,u8 minute, u8 second)  
{  
    u16 t;  
    u32 sec=0;  
  
    if(year >= 1970 && year<= 2099)        
    {  
        for(t= 1970 ;t<year;t++)           
        {  
            if(Is_LeapYear(t))            
  
                sec += 31622400;   
            else      
                sec += 31536000;          
        }     
  
  
        for(t=0;t<month-1;t++)                         
        {  
            sec += (u32) Days_Table[t]*86400;             
            if(Is_LeapYear(year) && t== 1)              
                sec += 86400;                             
      
        }  
				for(t=0;t<date;t++)                         
        {  
          sec += 86400;                             
      
        }  
        sec += (u32)(hour)*3600;  
        sec += (u32)(minute)*60;  
        sec += second;  
    }  
  
    return sec;                                     
  
}  
  
  
  
//自动获取当前时间配置RTC
  
void Rtc_TIME_AutoSet()  
{  
    u16 year,i=0,j=0;  
    u8  mon,date,sec,min,hour;  
  
    u8 *_date = __DATE__;  
    u8 *_time = __TIME__;  
  
    for(i=0;i<12;i++)  
    {  
        for(j=0;j<3;j++)  
        {  
            if(Month_Table[i][j] == _date[j]) mon = i;  //得到月份
        }  
    }  
  
  
    if(_date[4]==' ')
	{          //得到日期 
        date=_date[5]-'0';      //eg '7'-'0' =7  
    }
	else
	{   
        date=10*(_date[4]-'0')+_date[5]-'0';  
    }  
            
    year=1000*(_date[7]-'0')+100*(_date[8]-'0')+10*(_date[9]-'0')+_date[10]-'0';  //得到年份       
    hour=10*(_time[0]-'0')+_time[1]-'0';                                          //得到小时
    min=10*(_time[3]-'0')+_time[4]-'0';                                             
    sec=10*(_time[6]-'0')+_time[7]-'0';   
    //printf("\n%d-%d-%d  %d:%d:%d\n",year,mon,date,hour,min,sec);  
	Rtc_TIME_Set(year,mon,date,hour,min,sec);  
}  
  
  
//获取RTC时间 
void Rtc_Get()  
{  
    u32 secs,days,temp,years = 1970,months = 0;       
  
    secs = RTC->CNTH;    //获取当前总的秒数  
    secs <<= 16;  
    secs += RTC->CNTL;  
  
   
    days = secs/86400;  
    while(days >= 365)     
    {  
        if(Is_LeapYear(years))             
            {  
                if(days >= 366)   
                    days -= 366;   
                else  
                    break;  
            }else{  
                days -= 365;  
            }             
            years++;  
    }  
    timer.year = years;   

 
    while(days >= 28)  
    {  
       if(Is_LeapYear(years) && months ==1)    
       {  
            if(days >= 29)   
                days -= 29;   
             else  
                break;  
            }else{  
                if(days >= Days_Table[months])         
                    days -= Days_Table[months];  
                else  
                    break;  
            }  
						
        months++;    
	}  
	timer.date=(u8)days;
    timer.month = months+1;            
				
	temp = secs % 86400;                 
    timer.hour = temp/3600;                
    timer.minute = (temp%3600)/60;            
    timer.second = (temp%3600)%60;  
    timer.week = Rtc_DAY_Get(timer.year,timer.month,timer.date);  
}  	

  
//判断当前星期几                   
u8 Rtc_DAY_Get(u16 year,u8 month,u8 day)  
{     
    u16 temp;  
    u8 yearH,yearL;  
      
    yearH = year/100;     
    yearL = year%100;   
  
      
    if( yearH > 19 ) yearL += 100;  
  
  
  
    temp = yearL+yearL/4;  
    temp = temp%7;   
    temp = temp + day + _Week[month-1];  
  
    if( yearL%4 == 0 && month < 3 ) temp--;  
  
    return(temp%7);  
}  
  
void RTC__NVIC_Init()
{
	 SCB->AIRCR |= 0x5 << 8; 
    NVIC->IP[3] |= 0;   	
    NVIC->ISER[0] |=1<<3; //NVIC->ISER[1] |=1<<9;闹钟中断
}


void RTC_IRQHandler(void)  
{  
  if(RTC->CRL&0x0001)       
    {  
        
      Rtc_Get();  
			printf("\r\n Time : %d - %d - %d,%d : %d : %d ,Today is %s \r\n",  
      
                timer.year,  
      
                timer.month,  
      
                timer.date,  
      
                timer.hour,  
      
                timer.minute,  
      
                timer.second,  
                Week_Table[timer.week]  
        );   
			
			if((GPIOB->ODR & 0x0100) == 0)//LED灯累计工作时间
			{
				m_OpenTime++;
				if((m_OpenTime % 3600) == 0)
				{
					m_OpenTime = 0;	
					g_Bkp_flag = 1;
				}
			}
			else
			{
				if(m_OpenTime >= 2000)  //大于2000算1小时
				{
					g_Bkp_flag = 1;
				}
				m_OpenTime = 0;	 
			}

		}  
		RTC->CRL &= 0x0FFA;
}
