#ifndef __I2C_H
#define __I2C_H
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

#define I2C_ADDR 0x3C

u8 I2C_Check(u8 addr);
void I2C_WriteByte(u8 addr,u8 regaddr, u8 data);
u8 I2C_ReadByte(u8 addr,u8 regaddr);
void I2C_Configuration(void);
u16 I2C_ReadTwoByte(u8 addr,u8 regaddr);
u16 I2C_Read2(unsigned char id, unsigned char read_address);
#endif
