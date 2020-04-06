#include "SHT20.h"
#include "HBC_MISC.h"
#include "math.h"
#include "USART.h"

u8 SHT20_Check(void){
u8 STATUS;
 STATUS=I2C_Check(SHT20_ADDR);
	return STATUS;
}
float SHT20_GetTemp(){
	u16 TBUF;
	TBUF=I2C_ReadTwoByte(SHT20_ADDR,SHT20_TEMP_HM);
	TBUF &= ~0x0003;
  return ((float)TBUF * 0.00268127) - 46.85;
	
}
float SHT20_GetHum(){
	u16 HBUF;
	HBUF=I2C_ReadTwoByte(SHT20_ADDR,SHT20_HUM_HM);
	HBUF &= ~0x0003;
  return ((float)HBUF * 0.00190735) - 6;
}
