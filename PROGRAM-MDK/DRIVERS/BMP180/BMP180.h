#ifndef __BMP180_H
#define __BMP180_H

#include "I2C.h"
#include "USART.h"

#define BMP180_ADDR 0x77


u8 BMP180_Check(void);
void BMP_ReadCalibrationData(void);
long BMP_Read_UT(void);
long BMP_Read_UP(void);
void BMP_UncompemstatedToTrue(void);

typedef struct BMP180{
	short AC1;
    short AC2;
    short AC3;
    unsigned short AC4;
    unsigned short AC5;
    unsigned short AC6;
    short B1;
    short B2;
    short MB;
    short MC;
    short MD;
    long UT;
    long UP;
    long X1;
    long X2;
    long X3;
    long B3;
    unsigned long B4;
    long B5;
    long B6;
    long B7;
    long p;
    float Temp;
    float altitude;
}BMP180;
#endif

