#ifndef __SW3518_H
#define __SW3518_H

#include "I2C.h"
#include "USART.h"

#define SHT20_ADDR 0x40
#define SHT20_TEMP_HM 0xE3
#define SHT20_HUM_HM 0xE5
u8 SHT20_Check(void);
float SHT20_GetTemp();
float SHT20_GetHum();
#endif

