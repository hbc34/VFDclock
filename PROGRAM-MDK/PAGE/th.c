#include "VFD.h"
#include "SHT20.h"
#include "HBC_MISC.h"
#include "DS3231.h"
#include "page.h"
//extern u8 now_page;
//extern u8 page_2;
//extern u8 page_n;
u8 STATUS;

void TH_PAGE(void){
	char dispbuf[8];
	if(now_page==2){
		if(page_n){
			S1201_WriteStr(0,"        ");
			if(page_2==1){
				S1201_WriteStr(0,"TEMP&HUM");
			}else if(page_2==2){
				S1201_WriteStr(0,"Humidity");
			}else{
				page_2=1;
				page_n=1;
				return;
			}
			page_n=0;
			
			Delay(5000000);
		}
		STATUS=SHT20_Check();
		if(!STATUS){
			if(page_2==1){
				sprintf(dispbuf,"%02.2fC  ",SHT20_GetTemp());
				S1201_WriteStr(0,dispbuf);
			}else if(page_2==2){
				sprintf(dispbuf,"%02.2f%%  ",SHT20_GetHum());
				S1201_WriteStr(0,dispbuf);
			}
		}else if(STATUS==0xFF) S1201_WriteStr(0,"I2CERROR");
		else	S1201_WriteStr(0," DEVERR ");
		
	}

}