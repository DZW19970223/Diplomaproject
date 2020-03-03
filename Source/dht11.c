#include "dht11.h"
#include "OnBoard.h"
#include "hal_led.h"
#include "out.h"
/**
描述：DHT11 测试
作者：VigiLin from www.GEC-EDU.org
时间：2012-7-9
*/
#include "ioCC2530.h"
#include <stdio.h>

#define uchar unsigned char 
#define uint unsigned int

#define  DHT11_DATA  P0_4

extern int LZ;

/*****************全局变量的定义******************/
static uchar  Overtime_counter;  //判断等待是否超时的计数器。利用uchar型的数值范围进行自动延时控制（时长由初值决定），并判断是否超时
static uchar  bit_value;          //从DATA总线上读到的位值
static uchar  T_data_H, T_data_L, RH_data_H, RH_data_L, checkdata;//校验过的温度高8位,温度低8位,湿度高8位,湿度低8位,校验和8位
static uchar  T_data_H_temp, T_data_L_temp, RH_data_H_temp, RH_data_L_temp, checkdata_temp;//未经校验的数据
static uchar  comdata;            //从DHT11读取的一个字节的数据
static void  Read_Byte(void);
static void Read_DHT11(void);


/*****************从DHT11读取一个字节函数******************/        
static void  Read_Byte(void)
{
  uchar i;
  for (i = 0; i < 8; i++)                     //循环8次，读取8bit的数据
  {
    Overtime_counter = 2;                          //读取并等待DHT11发出的12-14us低电平开始信号
    P0DIR &= ~0x10;
    while ((!DHT11_DATA) && Overtime_counter++);
    //Delay_10us(80);                   //26-28us的低电平判断门限
    bit_value = 0;                          //跳过门限后判断总线是高还是低，高为1，低为0
    MicroWait(27);
    if(DHT11_DATA)
      bit_value = 1;
    Overtime_counter=2;                          //等待1bit的电平信号结束，不管是0是1在118us后都变为低电平，否则错误超时
    while (DHT11_DATA && Overtime_counter++);  //当U8FLAG加到255后溢出为0，跳出循环，并后加加为1
    if (Overtime_counter == 1)
      break;                           //超时则跳出for循环        
    comdata <<= 1;                      //左移1位，LSB补0
    comdata |= bit_value;                  //LSB赋值
  }
}

/*****************DHT11读取五个字节函数******************/
static void Read_DHT11(void)
{
  uchar checksum;
  P0DIR |= 0x10;
  DHT11_DATA = 0;                //主机拉低18ms
  //Delay(8900);
  MicroWait(18000);
  DHT11_DATA = 1;                //总线由上拉电阻拉高 主机延时20us-40us
  //Delay_10us(150);
  MicroWait(30);
  DHT11_DATA = 1;                //主机转为输入或者输出高电平，DATA线由上拉电阻拉高，准备判断DHT11的响应信号
  P0DIR &= ~0x10;
  if (!DHT11_DATA)                //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行        
  {
    Overtime_counter = 2;   //判断DHT11发出的80us的低电平响应信号是否结束
    while ((!DHT11_DATA)&&Overtime_counter++);
    Overtime_counter=2;   //判断DHT11是否发出80us的高电平，如发出则进入数据接收状态
    while ((DHT11_DATA)&&Overtime_counter++);
    Read_Byte();                //读取湿度值整数部分的高8bit
    RH_data_H_temp = comdata;
    Read_Byte();                //读取湿度值小数部分的低8bit
    RH_data_L_temp = comdata;
    Read_Byte();                //读取温度值整数部分的高8bit
    T_data_H_temp = comdata;
    Read_Byte();                //读取温度值小数部分的低8bit
    T_data_L_temp = comdata;
    Read_Byte();                //读取校验和的8bit
    checkdata_temp = comdata;
    P0DIR |= 0x10;
    DHT11_DATA = 1;                //读完数据将总线拉高
    checksum = (T_data_H_temp + T_data_L_temp + RH_data_H_temp + RH_data_L_temp);//进行数据校验
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


