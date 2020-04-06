#include "SW3518.h"
u16 getVoltage(u8 P){
	u8 MSB=0,LSB=0;
	u16 vbuf=0,voltage=0;
	if(P==VIN){
		I2C_WriteByte(SW3518_I2C_CTRL,0x02);
		I2C_WriteByte(SW3518_ADC_DATA_TYPE,0x01);
		MSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_H);
		LSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_L);
		vbuf=(MSB<<4)+LSB;
		voltage=vbuf*10;
		return voltage;
	}else if(P==VOUT){
		I2C_WriteByte(SW3518_ADC_DATA_TYPE,0x02);
		MSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_H);
		LSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_L);
		vbuf=(MSB<<4)+LSB;
		voltage=vbuf*6;
		return voltage;
	}else return 0;
}
u16 getCurrent(u8 USB){
	u8 MSB=0,LSB=0;
	u16 cbuf=0,current=0;
	if(USB==USBC){
		I2C_WriteByte(SW3518_ADC_DATA_TYPE,0x03);
		MSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_H);
		LSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_L);
		cbuf=(MSB<<4)+LSB;
		current=cbuf*1.5;
		return current;
	}
	else if(USB==USBA){
		I2C_WriteByte(SW3518_ADC_DATA_TYPE,0x04);
		MSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_H);
		LSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_L);
		cbuf=(MSB<<4)+LSB;
		current=cbuf*1.5;
		return current;
	}else return 0;
}


