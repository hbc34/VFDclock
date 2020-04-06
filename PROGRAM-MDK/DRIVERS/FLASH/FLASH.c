#include <FLASH.h>
#include <USART.h>
//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{    
    uint32_t offsetAddress=startAddress - FLASH_BASE;               //计算去掉0X08000000后的实际偏移地址
  uint32_t sectorPosition=offsetAddress;            //计算扇区地址，对于STM32F103VET6为0~255
  uint32_t sectorStartAddress=sectorPosition+FLASH_BASE;    //对应扇区的首地址
    uint16_t dataIndex;
    
  FLASH_Unlock();         //解锁写保护

  FLASH_ErasePage(sectorStartAddress);//擦除这个扇区
  
  for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
  {
    FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
  }
  
  FLASH_Lock();//上锁写保护
	
}
void FLASH_Erase(uint32_t startAddress){
	FLASH_Unlock();         //解锁写保护
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR );
  FLASH_ErasePage(startAddress);//擦除这个扇区
	FLASH_Lock();
}
void FLASH_WriteData(uint32_t startAddress,uint16_t writeData)
{    
    volatile FLASH_Status FLASHStatus;
  FLASH_Unlock();         //解锁写保护
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR );
  //FLASH_ErasePage(startAddress);//擦除这个扇区
 
    FLASHStatus=FLASH_ProgramHalfWord(startAddress,writeData);
  
  FLASH_Lock();//上锁写保护
	if (FLASHStatus != FLASH_COMPLETE)  //FLASH_COMPLETE表示闪存完成
      {
        //printf("FLSH :Error %08X\n\r",FLASHStatus);
        UART1_Send("Flash Write fail\r\n");
      }else UART1_Send("Flash Write OK\r\n");
}
//读取指定地址的半字(16位数据)
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}

//从指定地址开始读取多个数据
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
  uint16_t dataIndex;
  for(dataIndex=0;dataIndex<countToRead;dataIndex++)
  {
    readData[dataIndex]=FLASH_ReadHalfWord(startAddress+dataIndex*2);
  }
}

void write_to_flash(void)
{
    u16 buff[1200];
    u16 count_len = 2272 / 2;
    FLASH_WriteMoreData(StartServerManageFlashAddress,buff,count_len);
}

void read_from_flash(void)
{
    u16 buff[1200];
    u16 count_len = 2272 / 2;
    FLASH_WriteMoreData(StartServerManageFlashAddress,buff,count_len);

 }