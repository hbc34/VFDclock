#ifndef __VFD_H
#define __VFD_H

#include "USART.h"

#define S_CS GPIO_Pin_15
#define S_DIN GPIO_Pin_14
#define S_CLK GPIO_Pin_13
#define S_RST GPIO_Pin_12

void VFD_IO_INIT(void);
void VFD_cmd(unsigned char command);
void S1201_show(void);
void VFD_init(void);
void S1201_Brightness(unsigned char br);
void S1201_WriteOneChar(unsigned char x, unsigned char chr);
void S1201_WriteStr(unsigned char x, char *str);

#endif

