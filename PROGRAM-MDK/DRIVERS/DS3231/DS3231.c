#include "DS3231.h"
u8 BCD2HEX(u8 BCD){
	u8 HSB,LSB;
	HSB=BCD>>4;
	LSB=BCD&0x0F;
	return HSB*10+LSB;
}
u8 HEX2BCD(u8 HEX){
	u8 HSB,LSB;
	HSB=HEX/10;
	LSB=HEX%10;
	return (HSB<<4)+LSB;
}
u8 DS3231_Check(void){
u8 STATUS;
 STATUS=I2C_Check(DS3231_ADDR);
	return STATUS;
}

void DS3231_Get(Datetime * get){
	u8 BUF;
	get->second=BCD2HEX(I2C_ReadByte(DS3231_ADDR,DS3231_SEC));
	get->minute=BCD2HEX(I2C_ReadByte(DS3231_ADDR,DS3231_MIN));
	BUF=I2C_ReadByte(DS3231_ADDR,DS3231_HOUR);
	get->p1224=(BUF&0x40)>>6;
	if(get->p1224){
		get->hour=BCD2HEX(BUF&0x1F);
		get->ampm=(BUF&0x60)>>5;
	}
	else get->hour=BCD2HEX(BUF&0x3F);
	get->day=I2C_ReadByte(DS3231_ADDR,DS3231_DAY)&0x07;
	get->date=BCD2HEX(I2C_ReadByte(DS3231_ADDR,DS3231_DATE));
	BUF=I2C_ReadByte(DS3231_ADDR,DS3231_MPC);
	get->century=BUF&0x80;
	get->month=BCD2HEX(BUF&0x1F);
	get->year=BCD2HEX(I2C_ReadByte(DS3231_ADDR,DS3231_YEAR));
}
void DS3231_Set(Datetime * set){
	
	I2C_WriteByte(DS3231_ADDR,DS3231_SEC,HEX2BCD(set->second));
	I2C_WriteByte(DS3231_ADDR,DS3231_MIN,HEX2BCD(set->minute));
	if(set->p1224) {
		I2C_WriteByte(DS3231_ADDR,DS3231_HOUR,(set->p1224<<6)+(set->ampm<<5)+HEX2BCD(set->hour));
	}
	else I2C_WriteByte(DS3231_ADDR,DS3231_HOUR,(set->p1224<<6)+HEX2BCD(set->hour));
	I2C_WriteByte(DS3231_ADDR,DS3231_DAY,set->day);
	I2C_WriteByte(DS3231_ADDR,DS3231_DATE,HEX2BCD(set->date));
	//I2C_WriteByte(DS3231_ADDR,DS3231_MPC,set->century<<7&HEX2BCD(set->month));
	I2C_WriteByte(DS3231_ADDR,DS3231_MPC,HEX2BCD(set->month));
	I2C_WriteByte(DS3231_ADDR,DS3231_YEAR,HEX2BCD(set->year));
}
/*
u16 getVoltage(u8 P){
	u8 MSB=0,LSB=0;
	u16 vbuf=0,voltage=0;
		I2C_WriteByte(SW3518_I2C_CTRL,0x02);
		I2C_WriteByte(SW3518_ADC_DATA_TYPE,0x01);
		MSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_H);
		LSB=I2C_ReadByte(SW3518_ADC_DATA_BUF_L);
		vbuf=(MSB<<4)+LSB;
		voltage=vbuf*10;
		return voltage;
}*/


