#include "VFD.h"
#include "HBC_MISC.h"
#include "BMP180.h"
#include "page.h"
//extern u8 now_page;
//extern u8 page_2;
//extern u8 page_n;
extern BMP180 bmp180;
void PRESSURE_PAGE(void){
	if(now_page==3){
		u8 STATUS;
			char dispbuf[8];
			if(!BMP180_Check()) UART1_Send("OK\r\n");
			
			
			BMP_ReadCalibrationData();
			//BMP_Read_UT();
			//BMP_Read_UP();
			BMP_UncompemstatedToTrue();
		if(page_n){
			page_n=0;
			S1201_WriteStr(0,"        ");
			if(page_2==1){
				S1201_WriteStr(0,"PRESSURE");
				KEY_EXTI_CTL(1);
			}
			else if(page_2==2){
				S1201_WriteStr(0,"TEMP");
				KEY_EXTI_CTL(1);
			}else{
				page_2=1;
				page_n=1;
				return;
			}
			
			
		}
		Delay(5000000);
	
		if(page_2==1){
			sprintf(dispbuf,"%06ldpa",bmp180.p);
			S1201_WriteStr(0,dispbuf);
		}
		if(page_2==2){
			sprintf(dispbuf,"%.2fC",bmp180.Temp/10.0);
			S1201_WriteStr(0,dispbuf);
		}
	}
}