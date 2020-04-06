#include "VFD.h"
#include "HBC_MISC.h"
#include "DS3231.h"
#include "page.h"
//extern u8 now_page;
//extern u8 page_2;
//extern u8 page_n;
char *day_dp[]={"Monday  ","Tuesday ","Wednesda","Thursday","Friday  ","Saturday","Sunday  "};
u8 now_set=0;

u8 month_cal(u8 y,u8 m){//平年闰年判断
	int year=2000;
	year+=y;
	if(m==2){
		if((year%4==0 && year%100!=0)||year%400==0){
			return 29;
		}else return 28;
	}else if(m==1 || m==3 || m==5 || m==7 || m==8 || m==10 || m==12) return 31;
	else return 30;
}
u8 day_cal(u8 y1,u8 m,u8 d){//星期计算
	int y;
	u8 day;
	y=2000+y1;
	if(m==1||m==2) {
        m+=12;
        y--;
    }
    day=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
		//if(++day>6) day=0;
		day++;
		return day;
}
void TIME_PAGE(void){
	u8 STATUS;
	if(now_page==1){
		if(page_n){
			page_n=0;
			S1201_WriteStr(0,"        ");
			if(page_2==1){
				S1201_WriteStr(0,"TIME");
				KEY_EXTI_CTL(1);
			}
			else if(page_2==2){
				S1201_WriteStr(0,"DATE");
				//KEY_EXTI_CTL(0);
			}
			else if(page_2==3){
				S1201_WriteStr(0,"DOW");
				KEY_EXTI_CTL(0);
			}else if(page_2==4){
				S1201_WriteStr(0,"TIMESET");
				KEY_EXTI_CTL(0);
			}
			else{
				page_2=1;
				page_n=1;
				return;
			}
			Delay(5000000);
		}
		if(!Set_mode)
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==RESET){
			if(page_2<4)
			page_2++;
			else page_2=1;
			page_n=1;
		}
		
		STATUS=DS3231_Check();
		if(!STATUS) {
			char dispbuf[8];
			if(!Set_mode){
				DS3231_Get(&DS3231);
				if(page_2==1){
					sprintf(dispbuf,"%02d:%02d:%02d",DS3231.hour,DS3231.minute,DS3231.second);
					S1201_WriteStr(0,dispbuf);
				}else if(page_2==2){
					sprintf(dispbuf,"%02d-%02d-%02d",DS3231.year,DS3231.month,DS3231.date);
					S1201_WriteStr(0,dispbuf);
				}else if(page_2==3){
					
					S1201_WriteStr(0,day_dp[DS3231.day-1]);
					sprintf(dispbuf,"%d\r\n",DS3231.day);
					UART1_Send(dispbuf);
				}else if(page_2==4){
					S1201_WriteStr(0,"+-TO SET");
					if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
						Set_mode=1;
						
					}
				}
			
			}else{
				if(Set_done==1){
					Set_done=0;
					now_set++;
				}
				else if(now_set==0){
					
					sprintf(dispbuf,"YEAR:%02d ",DS3231.year);
					S1201_WriteStr(0,dispbuf);
					Set_val=&DS3231.year;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=99;
				}
				else if(now_set==1){
					sprintf(dispbuf,"MONTH:%02d",DS3231.month);
					S1201_WriteStr(0,dispbuf);
					Set_val=&DS3231.month;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=12;
				}
				else if(now_set==2){
					sprintf(dispbuf,"DATE:%02d ",DS3231.date);
					S1201_WriteStr(0,dispbuf);
					Set_val=&DS3231.date;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=month_cal(DS3231.year,DS3231.month);
				}
				else if(now_set==3){
					sprintf(dispbuf,"HOUR:%02d ",DS3231.hour);
					S1201_WriteStr(0,dispbuf);
					Set_val=&DS3231.hour;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=23;
				}
				else if(now_set==4){
					sprintf(dispbuf,"MIN:%02d  ",DS3231.minute);
					S1201_WriteStr(0,dispbuf);
					Set_val=&DS3231.minute;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=59;
				}
				else{
					Set_mode=0;
					now_set=0;
					Set_done=0;
					page_2=1;
					DS3231.day=day_cal(DS3231.year,DS3231.month,DS3231.date);
					DS3231.second=0;
					DS3231_Set(&DS3231);
				}
			}
			
			/*
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET) {
				if(DS3231.hour<23)
			DS3231.hour++;
				else DS3231.hour=0;
				DS3231_Set(&DS3231);
			}
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET) {
				if(DS3231.minute<59)
			DS3231.minute++;
				else DS3231.minute=0;
				DS3231.second=0;
				DS3231_Set(&DS3231);
			}*/
		}
		else if(STATUS==0xFF) S1201_WriteStr(0,"I2CERROR");
		else	S1201_WriteStr(0,"DS3231ER");
	}

}