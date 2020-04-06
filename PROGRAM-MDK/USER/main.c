

//头文件
#include "sys.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "USART.h"
#include "KEY.h"
#include "I2C.h"
#include "VFD.h"
#include "ADC.h"
#include "DS3231.h"
#include "page.h"
#include "stm32f10x_exti.h"
#include "HBC_MISC.h"
#include "stm32f10x_tim.h"
#include "FLASH.h"
#include "EEPROM_POS.h"
#include "BMP180.h"
#include "stm32f10x_iwdg.h"
#define USART_RX_LEN 30
/*
Bilibili : -HBC-
代码闹钟那部分有BUG，我会更新一下
比较乱，后期再整理吧
*/
//函数声明
unsigned char USART_RX_BUF[USART_RX_LEN];
unsigned int USART_RX_BUF_I=0;
char USART_RX_FLAG=0;
double Vin,U2V,U3V,U2I,U3I;

extern __IO u16 ADC_ConvertedValue[3][3];
u16 ADC_Value[3];
u8 now_page=1;
u8 page_2=1;
u8 page_n=0;
Datetime DS3231={0};
u8 Set_mode=0;
u8 Set_done=0;
u8 *Set_val;
u8 Set_val_min;
u8 Set_val_max;
u16 wdt;
u8 screen_auto;
u8 screen_br;
u8 screen_br_now;
u8 alarm_up;
u8 led_ctl=0;
char dispbuf[8];
int bri_tm=0;
int alarm_cu=0;
ALARM_Str ALARM1,ALARM2,ALARM3;
BMP180 bmp180;
//简易延时函数(不精确)

/*
void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
					unsigned char Res;
					Res =USART_ReceiveData(USART1);//(USART1->DR);
					USART_RX_BUF[USART_RX_BUF_I]=Res ;
					USART_RX_BUF_I++;
					if(USART_RX_BUF[USART_RX_BUF_I-1] == 0x0a){
						
					}	
					if(USART_RX_BUF_I>(USART_RX_LEN-1))USART_RX_BUF_I=0; 
					 
				
		}
}*/


void ADC_CAL(){
	int i,j;
	u16 sum=0;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			sum+=ADC_ConvertedValue[j][i];
		}
		ADC_Value[i]=(sum/3);
		sum=0;
	}
}

void hex2str(unsigned char hex,unsigned char* str){
	unsigned char Num2CharTable[] = "0123456789ABCDEF";
	str[0] = Num2CharTable[((hex >> 4) & 0x0f)];
	str[1] = Num2CharTable[(hex & 0x0f)];
}



//GPIO初始化

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//创建GPIO初始化结构
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);//使能PB口总线时钟 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	
}
void IWDG_Config(uint8_t prv ,uint16_t rlv)
{    
    // 使能 预分频寄存器PR和重装载寄存器RLR可写
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
    
    // 设置预分频器值
    IWDG_SetPrescaler( prv );
    
    // 设置重装载寄存器值
    IWDG_SetReload( rlv );
    
    // 把重装载寄存器的值放到计数器中
    IWDG_ReloadCounter();
    
    // 使能 IWDG
    IWDG_Enable();    
}
void IWDG_Feed(void)
{
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
}
void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

  TIM_DeInit(TIM1);
  TIM_InternalClockConfig(TIM1);
  //预分频系数为72，这样计数器时钟为72MHz/72 = 1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 96;
  //设置时钟分割
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  //设置计数器模式为向上计数模式
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  //设置计数溢出大小，每计1000个数就产生一个更新事件
  TIM_TimeBaseStructure.TIM_Period = 230-1;
  //将配置应用到TIM2中
  TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);

  //禁止ARR预装载缓冲器
  TIM_ARRPreloadConfig(TIM1, DISABLE);
 
  TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设
	


	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

  TIM_DeInit(TIM3);
  TIM_InternalClockConfig(TIM3);
  //预分频系数为72，这样计数器时钟为72MHz/72 = 1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 48;
  //设置时钟分割
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  //设置计数器模式为向上计数模式
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  //设置计数溢出大小，每计1000个数就产生一个更新事件
  TIM_TimeBaseStructure.TIM_Period = 50-1;
  //将配置应用到TIM2中
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

  //禁止ARR预装载缓冲器
  TIM_ARRPreloadConfig(TIM3, DISABLE);
 
  TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}


void pwm_cfg()

{
TIM_OCInitTypeDef TimOCInitStructure;
      //设置缺省值

       TIM_OCStructInit(&TimOCInitStructure);

       //PWM模式1输出

       TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

       //设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%

       TimOCInitStructure.TIM_Pulse = 115-1;

       //TIM输出比较极性高

       TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

       //使能输出状态

       TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

       //TIM2的CH2输出

       TIM_OC4Init(TIM1, &TimOCInitStructure);

       //设置TIM2的PWM输出为使能

       TIM_CtrlPWMOutputs(TIM1,DISABLE);
			 
			 
			 


       TIM_OCStructInit(&TimOCInitStructure);

       //PWM模式1输出

       TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

       //设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%

       TimOCInitStructure.TIM_Pulse = 15-1;

       //TIM输出比较极性高

       TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

       //使能输出状态

       TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

       //TIM2的CH2输出

       TIM_OC2Init(TIM3, &TimOCInitStructure);

       //设置TIM2的PWM输出为使能

       TIM_CtrlPWMOutputs(TIM3,DISABLE);

}
void BEEP(u8 on){
		GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	
	if(on){
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		TIM_CtrlPWMOutputs(TIM1,ENABLE);
	}
	else {
	TIM_CtrlPWMOutputs(TIM1,DISABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		//GPIO_DeInit(GPIO_InitStructure);
		GPIO_ResetBits(GPIOA,GPIO_Pin_11);
		
	}
}

void LEDCTL(u8 on){
		GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	
	
	if(on){
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		TIM_CtrlPWMOutputs(TIM3,ENABLE);
	}
	else {
	TIM_CtrlPWMOutputs(TIM3,DISABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		//GPIO_DeInit(GPIO_InitStructure);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		
	}
}
//短delay

void EXTI9_5_IRQHandler(void){
	KEY_EXTI_CTL(0);
	if(Set_mode){
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
			if(*Set_val>Set_val_min)
				(*Set_val)--;
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET){
			if(*Set_val<Set_val_max)
				(*Set_val)++;
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==RESET){
			Set_done=1;
			KEY_EXTI_CTL(0);
		}
	}
	else{
		
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET){
			if(now_page>1)
			now_page--;
			page_n=1;
		}
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET){
			if(now_page<5)
			now_page++;
			page_n=1;
		}
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==RESET){
			if(page_2<4)
			page_2++;
			else page_2=1;
			page_n=1;
		}
	}
	UART1_Send("Interrupt!\r\n");
	EXTI_ClearITPendingBit(EXTI_Line5);
	EXTI_ClearITPendingBit(EXTI_Line8);
	EXTI_ClearITPendingBit(EXTI_Line9);
	KEY_EXTI_CTL(1);
}

void ALARM_PROC(){
	u8 i;
	u8 alarm_dr;
	ALARM_Str *ALM_PC;
	for(i=0;i<3;i++){
		if(i==0){ALM_PC=&ALARM1;alarm_dr=1;}
		else if(i==1){ALM_PC=&ALARM2;alarm_dr=2;}
		else if(i==2){ALM_PC=&ALARM3;alarm_dr=3;}
		if((ALM_PC->ALARM_CTL&0x800)==0x800)
		if(DS3231.hour==ALM_PC->ALARM_HOUR && DS3231.minute==ALM_PC->ALARM_MINUTE && DS3231.second<3){
			UART1_Send("ALARM1!!!!\r\n\r\n");
			if((ALM_PC->ALARM_CTL&(1<<(6-DS3231.day-1)))==(1<<(6-DS3231.day-1))){
				UART1_Send("ALARM2!!!!\r\n\r\n");
				alarm_up=alarm_dr;
			}
		}
	}
	if(alarm_up){
		
		sprintf(dispbuf,"ALARM%d  ",alarm_up);
		S1201_WriteStr(0,dispbuf);
		for(i=0;i<5;i++){
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==RESET || GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)==RESET || GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET)
			alarm_up=0;
			BEEP(1);
			Delay(1000000);
			BEEP(0);
			Delay(1000000);
		}
		alarm_cu++;
		if(alarm_cu>1000){
			alarm_cu=0;
			alarm_up=0;
		}
	}
}

int main(void)
{
	char *day_dpx[]={"Sunday  ","Monday  ","Tuesday ","Wed     ","Thur    ","Friday  ","Saturday"};
	u8 STATUS;
	char USBUF[50];
	SystemInit();
	Stm32_Clock_Init(6);
	
	GPIO_Configuration();//初始化GPIO
	VFD_IO_INIT();
	KEY_IO_Init();
	KEY_EXTI_Init();
	UART1_Configuration();
	I2C_Configuration();
	ADC_Configuration();
	TIM_Configuration();
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
	GPIO_SetBits(GPIOB,S_RST);
	Delay(10000000);
	/*
	ALARM1.ALARM_CTL=0x01;
	ALARM1.ALARM_HOUR=0x02;
	ALARM1.ALARM_MINUTE=0x03;
	ALARM2.ALARM_CTL=0x04;
	ALARM2.ALARM_HOUR=0x05;
	ALARM2.ALARM_MINUTE=0x06;
	ALARM3.ALARM_CTL=0x07;
	ALARM3.ALARM_HOUR=0x08;
	ALARM3.ALARM_MINUTE=0x09;
	ALARM_Set();*/
	
	GPIO_ResetBits(GPIOB,S_RST);
	Delay(20);
	GPIO_SetBits(GPIOB,S_RST);
	Delay(1000000);
	VFD_init();
	Delay(10000);
	STATUS=DS3231_Check();
	S1201_WriteStr(0,"-HELLO!-");
	Delay(5000000);
	S1201_WriteStr(0,"FW V1.00");
	Delay(5000000);
	KEY_EXTI_CTL(1);
	pwm_cfg();
	BEEP(0);
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		S1201_WriteStr(0,"FREBOOT ");
		RCC_ClearFlag();
		Delay(5000000);
	}
	Storage_Get();
	if((wdt>>8)!=0xA0){
		S1201_WriteStr(0,"--INIT--");
		wdt=0xA0<<8;
		screen_auto=1;
		screen_br=200;
		ALARM1.ALARM_CTL=0x00;
		ALARM1.ALARM_HOUR=0x00;
		ALARM1.ALARM_MINUTE=0x00;
		ALARM2.ALARM_CTL=0x00;
		ALARM2.ALARM_HOUR=0x00;
		ALARM2.ALARM_MINUTE=0x00;
		ALARM3.ALARM_CTL=0x00;
		ALARM3.ALARM_HOUR=0x00;
		ALARM3.ALARM_MINUTE=0x00;
		Storage_Set();
	}
	Storage_Get();
	Delay(4000000);
	IWDG_Config(IWDG_Prescaler_256,1000);
    while (1)
	{
		IWDG_Feed();
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==RESET &&GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==RESET){
			if(led_ctl) led_ctl=0;
			else led_ctl=1;
			LEDCTL(led_ctl);
		}
		LEDCTL(led_ctl);
		ALARM_PROC();
		TIME_PAGE();
		TH_PAGE();
		PRESSURE_PAGE();
		SCREEN_PAGE();
		ALARM_PAGE();
		
		ADC_CAL();
		//sprintf(USBUF,"\r\nAuto-Brightness:%d",screen_auto);
					//UART1_Send(USBUF);
		if(bri_tm==0 || bri_tm>50){
			bri_tm=1;
			if(screen_auto){
			
			if(50+(ADC_Value[1]/100)*10>255)
				screen_br_now=255;
				
			else screen_br_now=50+(ADC_Value[1]/100)*10;
			}else screen_br_now=screen_br;
			
			S1201_Brightness(screen_br_now);
		}
		bri_tm++;
		//sprintf(USBUF,"A1:%d,A2:%d,A3:%d\r\nBRI:%d\r\nDow:%s\r\n",ADC_Value[0],ADC_Value[1],ADC_Value[2],50+(ADC_Value[1]/100)*26,day_dpx[DS3231.day]);
		
		//UART1_Send(USBUF);
		//sprintf(USBUF,"SCREEN_AUTO:%d\r\nWDT=%x\r\nA1:%x-%x-%x A2:%x-%x-%x A3:%x-%x-%x\r\n",screen_auto,wdt,ALARM1.ALARM_CTL,ALARM1.ALARM_HOUR,ALARM1.ALARM_MINUTE,ALARM2.ALARM_CTL,ALARM2.ALARM_HOUR,ALARM2.ALARM_MINUTE,ALARM3.ALARM_CTL,ALARM3.ALARM_HOUR,ALARM3.ALARM_MINUTE);
		//UART1_Send(USBUF);
		Delay(2000000);
	}
}

