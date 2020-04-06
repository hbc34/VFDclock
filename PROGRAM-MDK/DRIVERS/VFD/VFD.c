#include "VFD.h"

static void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
void VFD_IO_INIT(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;//创建GPIO初始化结构
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);//使能PB口总线时钟 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}
void write_6302(unsigned char w_data)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
		GPIO_ResetBits(GPIOB,S_CLK);
    if ( (w_data & 0x01) == 0x01)
    {
      GPIO_SetBits(GPIOB,S_DIN);
    }
    else
    {
      GPIO_ResetBits(GPIOB,S_DIN);
    }
		Delay(40);
    w_data >>= 1;
    GPIO_SetBits(GPIOB,S_CLK);
		Delay(40);
  }
}
void VFD_cmd(unsigned char command)
{
  GPIO_ResetBits(GPIOB,S_CS);
  write_6302(command);
  GPIO_SetBits(GPIOB,S_CS);
	//delay 5us
	Delay(40);
}
void S1201_show(void)
{
  GPIO_ResetBits(GPIOB,S_CS);//开始传输
  write_6302(0xe8);     //地址寄存器起始位置
  GPIO_SetBits(GPIOB,S_CS); //停止传输
}

void VFD_init(void)
{
  //SET HOW MANY digtal numbers
  GPIO_ResetBits(GPIOB,S_CS);
  write_6302(0xe0);
  //delayMicroseconds(5);
	Delay(40);
  write_6302(0x07);//8 digtal
  GPIO_SetBits(GPIOB,S_CS);
  //delayMicroseconds(5);
	Delay(40);
  //set bright
  GPIO_ResetBits(GPIOB,S_CS);
  write_6302(0xe4);
  //delayMicroseconds(5);
	Delay(40);
  write_6302(0xff);//leve 255 max
  GPIO_SetBits(GPIOB,S_CS);
  //delayMicroseconds(5);
	Delay(40);
}
void S1201_Brightness(unsigned char br){
GPIO_ResetBits(GPIOB,S_CS);
  write_6302(0xe4);
  //delayMicroseconds(5);
	Delay(40);
  write_6302(br);//leve 255 max
  GPIO_SetBits(GPIOB,S_CS);
  //delayMicroseconds(5);
	Delay(40);

}
void S1201_WriteOneChar(unsigned char x, unsigned char chr)
{
  GPIO_ResetBits(GPIOB,S_CS);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  write_6302(chr + 0x30);
  GPIO_SetBits(GPIOB,S_CS); //停止传输
  S1201_show();
}

void S1201_WriteStr(unsigned char x, char *str)
{
  GPIO_ResetBits(GPIOB,S_CS);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  while (*str)
  {
    write_6302(*str); //ascii与对应字符表转换
    str++;
  }
  GPIO_SetBits(GPIOB,S_CS); //停止传输
  S1201_show();
}
