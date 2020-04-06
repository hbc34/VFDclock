#include "ADC.h"
#define ADC1_DR_Address    ((u32)0x4001244C)
__IO u16 ADC_ConvertedValue[3][3];  

void ADC_Configuration(){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //??MDA1??
	/* DMA channel1 configuration */
  DMA_DeInit(DMA1_Channel1);  //??DMA??
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;////ADC1??---??ADC1?????????
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//??DMA????,ADC???????????
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //?????????????
  DMA_InitStructure.DMA_BufferSize = 3 * 3;	//?????---2??????2???
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //??????
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //??????---?????????ADC1?????---???????????????---
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //????????---??16?
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//????---2????????????ADC1?????ADC?---
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);  //??DMA??

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//??ADC1??
     
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //??????,????
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; //???????????
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //???????
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //???????
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel =3;  // ADC??????
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel11 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5); //??1????55.5?????
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_55Cycles5);
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);	 //??ADC?DMA
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE); //??ADC1

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //????

}
