#include "BMP180.h"
#include "HBC_MISC.h"
#include "math.h"
#include "USART.h"
extern BMP180 bmp180;
u8 BMP180_Check(void){
u8 STATUS;
 STATUS=I2C_Check(BMP180_ADDR);
	return STATUS;
}

void BMP_ReadCalibrationData(void)
{
    bmp180.AC1 = I2C_ReadTwoByte(BMP180_ADDR,0xAA);
    bmp180.AC2 = I2C_ReadTwoByte(BMP180_ADDR,0xAC);
    bmp180.AC3 = I2C_ReadTwoByte(BMP180_ADDR,0xAE);
    bmp180.AC4 = I2C_ReadTwoByte(BMP180_ADDR,0xB0);
    bmp180.AC5 = I2C_ReadTwoByte(BMP180_ADDR,0xB2);
    bmp180.AC6 = I2C_ReadTwoByte(BMP180_ADDR,0xB4);
    bmp180.B1  = I2C_ReadTwoByte(BMP180_ADDR,0xB6);
    bmp180.B2  = I2C_ReadTwoByte(BMP180_ADDR,0xB8);
    bmp180.MB  = I2C_ReadTwoByte(BMP180_ADDR,0xBA);
    bmp180.MC  = I2C_ReadTwoByte(BMP180_ADDR,0xBC);
    bmp180.MD  = I2C_ReadTwoByte(BMP180_ADDR,0xBE);
}

//从BMP180读取未修正的温度
long BMP_Read_UT(void)
{
	char sendbuf[20];
		long temp = 0;
		Delay(500000);
    
    I2C_WriteByte(BMP180_ADDR,0xF4,0x2E);
		Delay(500000);
    Delay(500);
    temp = (long)I2C_ReadTwoByte(BMP180_ADDR,0xF6);
	sprintf(sendbuf,"UT:%ld\r\n",temp);
	UART1_Send(sendbuf);
    return temp;
	
}

//从BMP180读取未修正的大气压
long BMP_Read_UP(void)
{
	char sendbuf[20];
    long pressure = 0;
		Delay(500000);
    I2C_WriteByte(BMP180_ADDR,0xF4,0x34);
    Delay(500000);

    pressure = (long)I2C_ReadTwoByte(BMP180_ADDR,0xF6);
    //pressure = pressure + BMP_ReadOneByte(0xf8);
    pressure &= 0x0000FFFF;
		sprintf(sendbuf,"UP:%ld\r\n",pressure);
	UART1_Send(sendbuf);
    return pressure;
}

//用获取的参数对温度和大气压进行修正，并计算海拔
void BMP_UncompemstatedToTrue(void)
{
    //bmp180.UT = BMP_Read_UT();//第一次读取错误
    bmp180.UT = BMP_Read_UT();//进行第二次读取修正参数
    bmp180.UP = BMP_Read_UP();

    bmp180.X1 = (bmp180.UT - bmp180.AC6) * bmp180.AC5 / 32768;
    bmp180.X2 = (((long)bmp180.MC) *2048) / (bmp180.X1 + bmp180.MD);
    bmp180.B5 = bmp180.X1 + bmp180.X2;
    bmp180.Temp  = (bmp180.B5 + 8) /16.0;

    bmp180.B6 = bmp180.B5 - 4000;
    bmp180.X1 = ((long)bmp180.B2 * (bmp180.B6 * bmp180.B6 >> 12)) >> 11;
    bmp180.X2 = ((long)bmp180.AC2) * bmp180.B6 >> 11;
    bmp180.X3 = bmp180.X1 + bmp180.X2;

    bmp180.B3 = ((((long)bmp180.AC1) * 4 + bmp180.X3) + 2) /4;
    bmp180.X1 = ((long)bmp180.AC3) * bmp180.B6 >> 13;
    bmp180.X2 = (((long)bmp180.B1) *(bmp180.B6*bmp180.B6 >> 12)) >>16;
    bmp180.X3 = ((bmp180.X1 + bmp180.X2) + 2) >> 2;
    bmp180.B4 = ((long)bmp180.AC4) * (unsigned long)(bmp180.X3 + 32768) >> 15;
    bmp180.B7 = ((unsigned long)bmp180.UP - bmp180.B3) * 50000;

    if(bmp180.B7 < 0x80000000)
    {
        bmp180.p = (bmp180.B7 * 2) / bmp180.B4;     
    }
    else
    {
        bmp180.p = (bmp180.B7 / bmp180.B4) * 2;
    }

    bmp180.X1 = (bmp180.p >> 8) * (bmp180.p >>8);
    bmp180.X1 = (((long)bmp180.X1) * 3038) >> 16;
    bmp180.X2 = (-7357 * bmp180.p) >> 16;

    bmp180.p = bmp180.p + ((bmp180.X1 + bmp180.X2 + 3791) >> 4);

    bmp180.altitude = 44330 * (1-pow(((bmp180.p) / 101325.0),(1.0/5.255)));  
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


