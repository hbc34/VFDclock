#include "VFD.h"
#include "HBC_MISC.h"
#include "DS3231.h"
#include "page.h"
//extern u8 now_page;
//extern u8 page_2;
//extern u8 page_n;
extern u8 screen_br_now;
void SCREEN_PAGE(void){
	char dispbuf[8];
	if(now_page==4){
		if(page_n){
			page_n=0;
			S1201_WriteStr(0,"        ");
			if(page_2==1){
				S1201_WriteStr(0,"SCREEN");
				KEY_EXTI_CTL(1);
			}
			else if(page_2==2){
				S1201_WriteStr(0,"AUTOBR");
				KEY_EXTI_CTL(0);
			}
			else if(page_2==3){
				S1201_WriteStr(0,"SETBR");
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
				sprintf(dispbuf,"BRI:%03d ",screen_br_now);
				S1201_WriteStr(0,dispbuf);
			}
			if(page_2==2){
				S1201_WriteStr(0,"+-TO SET");
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
					Set_mode=1;
				}
			}
			if(page_2==3){
				S1201_WriteStr(0,"+-TO SET");
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET && GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
							Set_mode=1;
				}
			}
		}else{
				if(page_2==2){
					
					if(screen_auto)
					S1201_WriteStr(0,"M:Auto  ");
					else
					S1201_WriteStr(0,"M:Manual");
					Set_val=&screen_auto;
					KEY_EXTI_CTL(1);
					Set_val_min=0;
					Set_val_max=1;
				}
				if(page_2==3){
					sprintf(dispbuf,"BRIS:%03d",screen_br);
					S1201_WriteStr(0,dispbuf);
					Set_val=&screen_br;
					KEY_EXTI_CTL(1);
					Set_val_min=50;
					Set_val_max=255;
				}
			if(Set_done==1){
					Set_mode=0;
					Set_done=0;
					page_2=1;
					Storage_Set();
			}
		}
	}

}