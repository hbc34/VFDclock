#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"
void KEY_IO_Init(void);
void KEY_EXTI_Init(void);
void KEY_EXTI_CTL(u8 st);

#endif