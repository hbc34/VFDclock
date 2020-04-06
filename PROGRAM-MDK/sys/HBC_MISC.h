#ifndef HBC_MISC
#define HBC_MISC
#include "USART.h"

typedef struct ALARM_Str{

	u16 ALARM_CTL;
	u8 ALARM_HOUR;
	u8 ALARM_MINUTE;
	
}ALARM_Str;
	void Delay(uint32_t nCount);
	void Storage_Get(void);
	void Storage_Set(void);
#endif