#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#define Channel_Num  5
#define Sample_Num  5

void ADC_Configuration(void);

#endif
