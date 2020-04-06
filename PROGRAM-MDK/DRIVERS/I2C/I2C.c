#include "HBC_MISC.h"
#include "I2C.h"
#include "USART.h"
#include "STM32F10x_GPIO.h"
#include "STM32F10x_RCC.h"
u8 I2C_Check(u8 addr){
	u16 timeout;
	I2C_Cmd(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE); 
	I2C_GenerateSTART(I2C1,ENABLE); 
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){
		  if((timeout--)==0){
				UART1_Send("CEV5 Fail");
				return 0xFF;
			}
	}//EV5
	I2C_Send7bitAddress(I2C1,addr<<1,I2C_Direction_Transmitter); 
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		  if((timeout--)==0){
				UART1_Send("CEV6 Fail");
				return 0xFE;
			}
	} //EV6
	I2C_GenerateSTOP(I2C1,ENABLE); 
	I2C_Cmd(I2C1,DISABLE);
	return 0;
}
void I2C_WriteByte(u8 addr,u8 regaddr, u8 data)
{
	u16 timeout;
	I2C_Cmd(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE); 
	I2C_GenerateSTART(I2C1,ENABLE); 
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){
		  if((timeout--)==0){
				UART1_Send("WEV5 Fail");
				return;
			}
	}//EV5
	I2C_Send7bitAddress(I2C1,addr<<1,I2C_Direction_Transmitter); 
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		  if((timeout--)==0){
				UART1_Send("WEV6 Fail");
				return;
			}
	} //EV6
	I2C_SendData(I2C1,regaddr); 
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		  if((timeout--)==0){
				UART1_Send("WEV8 Fail");
				return;
			}
	} //EV8
	I2C_SendData(I2C1,data);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
		  if((timeout--)==0){
				UART1_Send("WEV8_2 Fail");
				return;
			}
	}
	I2C_GenerateSTOP(I2C1,ENABLE); 
		I2C_Cmd(I2C1,DISABLE);

}
u16 I2C_ReadTwoByte(u8 addr,u8 regaddr)
{
	u8 readtemp;
	u16 timeout=5000;
  short data;
  uint8_t msb,lsb;
	I2C_Cmd(I2C1,ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);
	timeout=5000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))//5
	{
		  if((timeout--)==0){
				UART1_Send("REV5 Fail");
				return 0xFE;
			}
	}	
	I2C_Send7bitAddress(I2C1,addr<<1,I2C_Direction_Transmitter);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//6
	{
		  if((timeout--)==0){
				UART1_Send("REV6 Fail");
				return 0xFE;
			}
	}	
	I2C_Cmd(I2C1,ENABLE);
	I2C_SendData(I2C1,regaddr);
	timeout=1000;
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS )
  {
		  if((timeout--)==0){
				UART1_Send("REV8 Fail");
				return 0xFE;
			}
	}	
	
	I2C_GenerateSTART(I2C1,ENABLE);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{
		  if((timeout--)==0){
				UART1_Send("REV5 Fail");
				return 0xFE;
			}
	}	
	
	I2C_Send7bitAddress(I2C1,(addr<<1)+1,I2C_Direction_Receiver);
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		  if((timeout--)==0){
				UART1_Send("REV6.2 Fail");
				return 0xFE;
			}
	}
	timeout=50000;
	while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))){
		if((timeout--)==0){
				UART1_Send("Timeout Fail");
				return 0xFE;
			}
	}
	/*
	timeout=5000;
	while(I2C_CheckEvent(I2C1,I2C_FLAG_BUSY))
  {
		  if((timeout--)==0){
				UART1_Send("BUSY Fail");
				return 0xFE;
			}
	}*/
  /* ?????? */
  msb = I2C_ReceiveData(I2C1);
	/*
	while(I2C_CheckEvent(I2C1,I2C_FLAG_BUSY))
  {
		  if((timeout--)==0){
				UART1_Send("BUSY Fail");
				return 0xFE;
			}
	}*/
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	timeout=5000;
	while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))){
		if((timeout--)==0){
				UART1_Send("Timeout Fail");
				return 0xFE;
			}
	}
	lsb	= I2C_ReceiveData(I2C1);
	/*
   while(!I2C_CheckEvent(I2C1,I2C_FLAG_RXNE))
  {
		  if((timeout--)==0){
				UART1_Send("REV7 Fail");
				return 0xFE;
			}
	}*/
  
  I2C_Cmd(I2C1,DISABLE);
	
	data = (msb<<8) + lsb;
	I2C_AcknowledgeConfig(I2C1,ENABLE);
  return data;
}

u8 I2C_ReadByte(u8 addr,u8 regaddr)
{
	u8 readtemp;
	u16 timeout;
	I2C_Cmd(I2C1,ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))//5
	{
		  if((timeout--)==0){
				UART1_Send("REV5 Fail");
				return 0xFE;
			}
	}	
	
	
	I2C_Send7bitAddress(I2C1,addr<<1,I2C_Direction_Transmitter);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//6
	{
		  if((timeout--)==0){
				UART1_Send("REV6 Fail");
				return 0xFE;
			}
	}	
	
	I2C_Cmd(I2C1,ENABLE);
	I2C_SendData(I2C1,regaddr);
	timeout=1000;
  while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS )
  {
		  if((timeout--)==0){
				UART1_Send("REV8 Fail");
				return 0xFE;
			}
	}	
	
	I2C_GenerateSTART(I2C1,ENABLE);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{
		  if((timeout--)==0){
				UART1_Send("REV5 Fail");
				return 0xFE;
			}
	}	
	
	I2C_Send7bitAddress(I2C1,(addr<<1)+1,I2C_Direction_Receiver);
	timeout=1000;
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		  if((timeout--)==0){
				UART1_Send("REV6.2 Fail");
				return 0xFE;
			}
	}	
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	I2C_GenerateSTOP(I2C1,ENABLE);
	timeout=1000;
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
		  if((timeout--)==0){
				UART1_Send("REV7 Fail");
				return 0xFE;
			}
	}	

  readtemp = I2C_ReceiveData(I2C1);
   I2C_AcknowledgeConfig(I2C1,ENABLE);
  /* ???? */
  
  	I2C_Cmd(I2C1,DISABLE);

  return readtemp;
}

void I2C_Configuration(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // ????
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
  I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 =0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(I2C1, &I2C_InitStructure);
	
}
