#ifndef __PAGE_H
#define __PAGE_H
#include "usart.h"
#include "DS3231.h"
#include "key.h"
#include "HBC_MISC.h"
extern u8 now_page;
extern u8 page_2;
extern u8 page_n;
extern Datetime DS3231;
extern u8 Set_mode;
extern u8 Set_done;
extern u8 *Set_val;
extern u8 Set_val_min;
extern u8 Set_val_max;
extern ALARM_Str ALARM1,ALARM2,ALARM3;
extern u16 wdt;
extern u8 screen_auto;
extern u8 screen_br;
void TIME_PAGE(void);
void TH_PAGE(void);
void PRESSURE_PAGE(void);
void SCREEN_PAGE(void);
void ALARM_PAGE(void);

#endif