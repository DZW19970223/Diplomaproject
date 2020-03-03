#include "adc.h"
#include "OnBoard.h"
#include "ioCC2530.h"
#include <stdio.h>
#define ADC_CHNN  0x05  //ѡ��ͨ��5
#define uchar unsigned char 
#define uint unsigned int
static uint16 readAdc(uint8 channal);
void Get_ADCValue(unsigned int *AvgValue);
/********************************************************
* @brief   ��ȡADC��ֵ
********************************************************/
static uint16 readAdc(uint8 channal)
{
  uint16 value ; 
  APCFG |= 1 << channal ; // ģ���ź�����
  ADCIF = 0 ;
  
  ADCCON3 = channal;          
  while ( !ADCIF ) ;  //�ȴ�ת�����
  
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



