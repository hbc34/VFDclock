#include "VFD.h"
#include "HBC_MISC.h"
#include "DS3231.h"
#include "page.h"
//extern u8 now_page;
//extern u8 page_2;
//extern u8 page_n;
static u8 now_set=0;
ALARM_Str *ALARM_SET;
u8 ALM_CTL,ALM_DOW;
char USBUF[50];
static char *day_dp[]={"MON","TUE","WED","THU","FRI","SAT","SUN"};
void ALARM_PAGE(void){
	char dispbuf[8];
	if(now_page==5){
		if(page_n){
			page_n=0;
			S1201_WriteStr(0,"        ");
			if(page_2==1){
				S1201_WriteStr(0,"ALARM");
				KEY_EXTI_CTL(1);
			}
			else if(page_2==2){
				S1201_WriteStr(0,"ALARM1");
				KEY_EXTI_CTL(0);
			}
			else if(page_2==3){
				S1201_WriteStr(0,"ALARM2");
				KEY_EXTI_CTL(0);
			}
			else if(page_2==4){
				S1201_WriteStr(0,"ALARM3");
				KEY_EXTI_CTL(0);
			}
			else{
				page_2=1;
				page_n=1;
				return;
			}
			Delay(5000000);
		}
		if(!Set_mode){
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==RESET){
				if(page_2<4)
				page_2++;
				else page_2=1;
				page_n=1;
			}
			if(page_2==1){
				u8 a,b,c;
				if((ALARM1.ALARM_CTL&0x800)==0x800) a=1;else a=0;
				if((ALARM2.ALARM_CTL&0x800)==0x800) b=1;else b=0;
				if((ALARM3.ALARM_CTL&0x800)==0x800) c=1;else c=0;
				sprintf(dispbuf,"1%c 2%c 3%c",a?'O':'X',b?'O':'X',c?'O':'X');
				S1201_WriteStr(0,dispbuf);
			}
			if(page_2==2){
				S1201_WriteStr(0,"+-TO SET");
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
					ALARM_SET=&ALARM1;
					Set_mode=1;
					if((ALARM_SET->ALARM_CTL&0x800)==0x800) ALM_CTL=1; else ALM_CTL=0;
				}
			}
			if(page_2==3){
				S1201_WriteStr(0,"+-TO SET");
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
					ALARM_SET=&ALARM2;
							Set_mode=1;
					if((ALARM_SET->ALARM_CTL&0x800)==0x800) ALM_CTL=1; else ALM_CTL=0;
				}
			}
			if(page_2==4){
				S1201_WriteStr(0,"+-TO SET");
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
					ALARM_SET=&ALARM3;
							Set_mode=1;
					if((ALARM_SET->ALARM_CTL&0x800)==0x800) ALM_CTL=1; else ALM_CTL=0;
				}
			}
		}else{
			if(Set_done==1){
					Set_done=0;
				if(now_set==0)  {if(ALM_CTL)ALARM_SET->ALARM_CTL|=0x800;else ALARM_SET->ALARM_CTL&=0xF7FF;}
				if(now_set>2) ALARM_SET->ALARM_CTL|=ALM_DOW<<(9-now_set);
					now_set++;
				if(now_set>2) {if((ALARM_SET->ALARM_CTL&(0x40>>(now_set-3)))==(0x40>>(now_set-3)))ALM_DOW=1; else ALM_DOW=0;}
				
				sprintf(USBUF,"\r\nCTL:0x%x\r\n",ALARM_SET->ALARM_CTL);
				UART1_Send(USBUF);
				}
				else if(now_set==0){
					
					sprintf(dispbuf,"CTL:%s",ALM_CTL?"ON  ":"OFF ");
					S1201_WriteStr(0,dispbuf);
					Set_val=&ALM_CTL;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=1;
				}
				else if(now_set==1){
					sprintf(dispbuf,"HOUR:%02d ",ALARM_SET->ALARM_HOUR);
					S1201_WriteStr(0,dispbuf);
					Set_val=&ALARM_SET->ALARM_HOUR;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=23;
				}
				else if(now_set==2){
					sprintf(dispbuf,"MIN:%02d  ",ALARM_SET->ALARM_MINUTE);
					S1201_WriteStr(0,dispbuf);
					Set_val=&ALARM_SET->ALARM_MINUTE;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=59;
				}else if(now_set<10){
					sprintf(dispbuf,"%s:%s",day_dp[now_set-3],ALM_DOW?"ON  ":"OFF ");
					S1201_WriteStr(0,dispbuf);
					Set_val=&ALM_DOW;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=1;
				}
				else{
					if(now_set==10) ALARM_SET->ALARM_CTL|=ALM_DOW<<(9-now_set);
					Set_mode=0;
					now_set=0;
					Set_done=0;
					page_2=1;
					Storage_Set();
				}
		}
		
	}

}