#include "dht11.h"
#include "OnBoard.h"
#include "hal_led.h"
#include "out.h"
/**
������DHT11 ����
���ߣ�VigiLin from www.GEC-EDU.org
ʱ�䣺2012-7-9
*/
#include "ioCC2530.h"
#include <stdio.h>

#define uchar unsigned char 
#define uint unsigned int

#define  DHT11_DATA  P0_4

extern int LZ;

/*****************ȫ�ֱ����Ķ���******************/
static uchar  Overtime_counter;  //�жϵȴ��Ƿ�ʱ�ļ�����������uchar�͵���ֵ��Χ�����Զ���ʱ���ƣ�ʱ���ɳ�ֵ�����������ж��Ƿ�ʱ
static uchar  bit_value;          //��DATA�����϶�����λֵ
static uchar  T_data_H, T_data_L, RH_data_H, RH_data_L, checkdata;//У������¶ȸ�8λ,�¶ȵ�8λ,ʪ�ȸ�8λ,ʪ�ȵ�8λ,У���8λ
static uchar  T_data_H_temp, T_data_L_temp, RH_data_H_temp, RH_data_L_temp, checkdata_temp;//δ��У�������
static uchar  comdata;            //��DHT11��ȡ��һ���ֽڵ�����
static void  Read_Byte(void);
static void Read_DHT11(void);


/*****************��DHT11��ȡһ���ֽں���******************/        
static void  Read_Byte(void)
{
  uchar i;
  for (i = 0; i < 8; i++)                     //ѭ��8�Σ���ȡ8bit������
  {
    Overtime_counter = 2;                          //��ȡ���ȴ�DHT11������12-14us�͵�ƽ��ʼ�ź�
    P0DIR &= ~0x10;
    while ((!DHT11_DATA) && Overtime_counter++);
    //Delay_10us(80);                   //26-28us�ĵ͵�ƽ�ж�����
    bit_value = 0;                          //�������޺��ж������Ǹ߻��ǵͣ���Ϊ1����Ϊ0
    MicroWait(27);
    if(DHT11_DATA)
      bit_value = 1;
    Overtime_counter=2;                          //�ȴ�1bit�ĵ�ƽ�źŽ�����������0��1��118us�󶼱�Ϊ�͵�ƽ���������ʱ
    while (DHT11_DATA && Overtime_counter++);  //��U8FLAG�ӵ�255�����Ϊ0������ѭ��������Ӽ�Ϊ1
    if (Overtime_counter == 1)
      break;                           //��ʱ������forѭ��        
    comdata <<= 1;                      //����1λ��LSB��0
    comdata |= bit_value;                  //LSB��ֵ
  }
}

/*****************DHT11��ȡ����ֽں���******************/
static void Read_DHT11(void)
{
  uchar checksum;
  P0DIR |= 0x10;
  DHT11_DATA = 0;                //��������18ms
  //Delay(8900);
  MicroWait(18000);
  DHT11_DATA = 1;                //������������������ ������ʱ20us-40us
  //Delay_10us(150);
  MicroWait(30);
  DHT11_DATA = 1;                //����תΪ�����������ߵ�ƽ��DATA���������������ߣ�׼���ж�DHT11����Ӧ�ź�
  P0DIR &= ~0x10;
  if (!DHT11_DATA)                //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������        
  {
    Overtime_counter = 2;   //�ж�DHT11������80us�ĵ͵�ƽ��Ӧ�ź��Ƿ����
    while ((!DHT11_DATA)&&Overtime_counter++);
    Overtime_counter=2;   //�ж�DHT11�Ƿ񷢳�80us�ĸߵ�ƽ���緢����������ݽ���״̬
    while ((DHT11_DATA)&&Overtime_counter++);
    Read_Byte();                //��ȡʪ��ֵ�������ֵĸ�8bit
    RH_data_H_temp = comdata;
    Read_Byte();                //��ȡʪ��ֵС�����ֵĵ�8bit
    RH_data_L_temp = comdata;
    Read_Byte();                //��ȡ�¶�ֵ�������ֵĸ�8bit
    T_data_H_temp = comdata;
    Read_Byte();                //��ȡ�¶�ֵС�����ֵĵ�8bit
    T_data_L_temp = comdata;
    Read_Byte();                //��ȡУ��͵�8bit
    checkdata_temp = comdata;
    P0DIR |= 0x10;
    DHT11_DATA = 1;                //�������ݽ���������
    checksum = (T_data_H_temp + T_data_L_temp + RH_data_H_temp + RH_data_L_temp);//��������У��
    if (checksum == checkdata_temp)
    {
      RH_data_H = RH_data_H_temp;
      RH_data_L = RH_data_L_temp;
      T_data_H  = T_data_H_temp;
      T_data_L  = T_data_L_temp;
      checkdata = checkdata_temp;
    }
  }
}

void GetDht11Value(unsigned char *pTemp,unsigned char *pHumid)
{
  Read_DHT11();
  *pTemp=T_data_H;
  *pHumid=RH_data_H;
  if(LZ==0)
  {
    if(T_data_H>=28)
    {
      L298NStart();
    }
    else if(T_data_H<=25)
    {
      L298NClose();
    }
    if(RH_data_H<=60)
    {
      JDQ1Start();
    }
    else if(RH_data_H>=85)
    {
      JDQ1Close();
    }
  }
}


