#include "adc.h"
#include "OnBoard.h"
#include "ioCC2530.h"
#include <stdio.h>
#define ADC_CHNN  0x05  //选用通道5
#define uchar unsigned char 
#define uint unsigned int
static uint16 readAdc(uint8 channal);
void Get_ADCValue(unsigned int *AvgValue);
/********************************************************
* @brief   读取ADC的值
********************************************************/
static uint16 readAdc(uint8 channal)
{
  uint16 value ; 
  APCFG |= 1 << channal ; // 模拟信号设置
  ADCIF = 0 ;
  
  ADCCON3 = channal;          
  while ( !ADCIF ) ;  //等待转换完成
  
  value = ADCL ;
  value |= ((uint16) ADCH) << 8 ;
  
  return value; 
}
void Get_ADCValue(unsigned int *AvgValue)
{
  char i;
  uint16 avgValue;
  for (i = 0 ; i < 64 ; i++)
  {
    avgValue += readAdc(ADC_CHNN);             
    avgValue >>= 1;
  }
  *AvgValue = avgValue;
}



