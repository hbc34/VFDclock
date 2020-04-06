#include "stm32f10x_flash.h"

#define StartServerManageFlashAddress    ((u32)0x08036000)//读写起始地址（内部flash的主存储块地址从0x08036000开始）

void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
void FLASH_WriteData(uint32_t startAddress,uint16_t writeData);
uint16_t FLASH_ReadHalfWord(uint32_t address);
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);
void write_to_flash(void);
void read_from_flash(void);
void FLASH_Erase(uint32_t startAddress);