#ifndef __DS3231_H
#define __DS3231_H

#include "I2C.h"
#include "USART.h"

#define DS3231_ADDR 0X68

#define DS3231_SEC 0x00
#define DS3231_MIN 0x01
#define DS3231_HOUR 0x02
#define DS3231_DAY 0x03
#define DS3231_DATE 0x04
#define DS3231_MPC 0x05
#define DS3231_YEAR 0x06

typedef struct typDatetime{
	u8 second;
	u8 minute;
	u8 p1224;
	u8 ampm;
	u8 hour;
	u8 day;
	u8 date;
	u8 month;
	u8 century;
	u8 year;
}Datetime;

u8 DS3231_Check(void);
void DS3231_Get(Datetime * get);
void DS3231_Set(Datetime * set);
#endif

