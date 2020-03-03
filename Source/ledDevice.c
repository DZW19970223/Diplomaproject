#include "smartHome.h"
#include "sapi.h"
#include "hal_led.h"
#include "adc.h"
#include "IC.h"
#include "LCD.h"
#include "SG90.h"
#include "out.h"
#include "OnBoard.h"
#include "stdio.h"
#include "string.h"

#define NUM_LED_1 1
#define NUM_LED_2 2

#define LEDNUM  2
uint8 ledIdList[LEDNUM]={NUM_LED_1,NUM_LED_2};

#define NUM_IN_CMD_LEDDEVICE 1
#define NUM_OUT_CMD_LEDDEVICE 2

static unsigned char buffer[8];
const cId_t ledDeviceInputCommandList[NUM_IN_CMD_LEDDEVICE]=
{TOGGLE_LED_CMD_ID};
const cId_t ledDeviceOutputCommandList[NUM_OUT_CMD_LEDDEVICE]=
{LEDJOINNET_CMD_ID,HEART_BEAT_CMD_ID};
const SimpleDescriptionFormat_t zb_SimpleDesc=
{
  ENDPOINT_ID_SMARTHOME,
  PROFILE_ID_SMARTHOME,
  DEVICE_ID_LEDDEVICE,
  DEVIDE_VERSION_ID,
  0,
  NUM_IN_CMD_LEDDEVICE,
  (cId_t*)ledDeviceInputCommandList,
  NUM_OUT_CMD_LEDDEVICE,
  (cId_t*)ledDeviceOutputCommandList  
};
/***********
执行时机：发送的数据包被接收方收到时被调用
handle:包的编号；
status:ZSUCCESS表示成功接收
************/
void zb_SendDataConfirm( uint8 handle, uint8 status )
{
  
}

/***********
执行时机：接收到的数据包被调用
************/
void zb_ReceiveDataIndication( uint16 source, uint16 command, 
                              uint16 len, uint8 *pData  )
{
  if(command==TOGGLE_LED_CMD_ID)
  {
    uint8 led_num=pData[0];
    uint8 led_status=pData[1];
    uint8 unit=pData[2];
    uint8 ten=pData[3];
    uint8 hundred=pData[4];
    uint8 unit1=pData[5];
    uint8 ten1=pData[6];
    
    if(led_num==NUM_LED_1)
    {
      if(led_status==1)
        HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
      else if(led_status==0)
        HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
      else if(led_status==2)
      {
        LCD_P8x16Str(8,0, "ID:");
        LCD_P8x16Str(33,0,buffer);    
        memset(buffer,0,8); 
        for(int i=0; i<2; i++) 
        {
          LCD_P16x16Ch(16*i+8, 3, i);   
        }
        LCD_P8x16Str(40,3, ":");
        unsigned char buffe[1];
        sprintf((char *)buffe,"%d",unit);
        if(unit!=10)
          LCD_P8x16Str(64,3,buffe);
        else
          LCD_P8x16Str(64,3,"0");
        sprintf((char *)buffe,"%d",ten);
        if(ten!=10)
          LCD_P8x16Str(56,3,buffe);
        else
          LCD_P8x16Str(56,3,"0");
        sprintf((char *)buffe,"%d",hundred);
        if(hundred!=10)
          LCD_P8x16Str(48,3,buffe);
        else
          LCD_P8x16Str(48,3,"0");
        LCD_P16x16Ch(72, 3, 4);
        for(int j=0,k=5;k<9;j++,k++)
        {
          LCD_P16x16Ch(16*j+8, 6, k);
        }
        LCD_P8x16Str(72,6, ":");
        sprintf((char *)buffe,"%d",unit1);
        if(unit1!=10)
          LCD_P8x16Str(88,6,buffe);
        else
          LCD_P8x16Str(88,6,"0");
        sprintf((char *)buffe,"%d",ten1);
        if(ten1!=10)
          LCD_P8x16Str(80,6,buffe);
        else
          LCD_P8x16Str(80,6,"0");
        LCD_P16x16Ch(96, 6, 9);
        SG90start(); 
        LCD_CLS(); 
      }
      else if(led_status==3)
      {
        LCD_P8x16Str(8,0, "ID:");
        LCD_P8x16Str(33,0,buffer);    
        memset(buffer,0,8); 
        for(int i=0; i<4; i++) 
        {
          LCD_P16x16Ch(16*i+8, 3, i);   
        }
        for(int a=0;a<100;a++)
        {
          MicroWait(10000);
        }
        LCD_CLS(); 
      }
      else if(led_status==4)
      {
        Draw_BMP(0,0,127,7,BMP2);
        for(int a=0;a<100;a++)
        {
          MicroWait(10000);
        }
        LCD_CLS(); 
      }
    }
    else if(led_num==NUM_LED_2)
    {
      if(led_status==1)
        HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);
      else if(led_status==0)
        HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
    }
  }
}


void zb_AllowBindConfirm( uint16 source )
{
}

void zb_HandleKeys( uint8 shift, uint8 keys )
{
  
}

void zb_BindConfirm( uint16 commandId, uint8 status )
{
}

void zb_StartConfirm( uint8 status )
{
  if(status==ZSUCCESS)
  {
    //可把节点所包含的led灯的ID号发送过去
    zb_SendDataRequest(0X0,LEDJOINNET_CMD_ID,
                       LEDNUM,ledIdList,0,FALSE,AF_DEFAULT_RADIUS);
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,60000);
    osal_start_timerEx(sapi_TaskID,RC522_PERIOD_EVT,2000);
    HalLedBlink(HAL_LED_ALL,5,50,2000);
    
    IC_Init();
    LCD_Init(); //oled 初始化 
    LCD_CLS(); //屏全亮
  }
}

void zb_HandleOsalEvent( uint16 event )
{
  if(event&TIMER_TIMEOUT_EVT)
  {
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,60000);
    zb_SendDataRequest(0X0,HEART_BEAT_CMD_ID,
                       0,NULL,0,FALSE,AF_DEFAULT_RADIUS); 
  }
  
  if(event&RC522_PERIOD_EVT)
  {
    osal_start_timerEx(sapi_TaskID,RC522_PERIOD_EVT,2000);
    unsigned char asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},i;
    unsigned char Card_Id[8]; //存放32位卡号
    
    unsigned int lightvalue;
    unsigned int AvgValue;
    unsigned char value[2];
    
    for(int j=0,k=10;k<13;j++,k++)
    {
      LCD_P16x16Ch(24*j+32, 3, k);
    }
    if(IC_Test()==1)
    {   
      LCD_CLS(); 
      //16进制转ASC码 串口打印IC卡号码
      for(i=0;i<4;i++)
      {
        Card_Id[i*2]=asc_16[qq[i]/16];
        Card_Id[i*2+1]=asc_16[qq[i]%16];        
      }
      sprintf((char *)buffer,"%c%c%c%c%c%c%c%c",Card_Id[0],Card_Id[1],Card_Id[2],Card_Id[3],Card_Id[4],Card_Id[5],Card_Id[6],Card_Id[7]); 
      zb_SendDataRequest(0X0,RC522VALUE_CMD_ID,8,Card_Id,0,FALSE,AF_DEFAULT_RADIUS);
    }
    
    Get_ADCValue(&lightvalue);
    value[0]=lightvalue&0xff;
    value[1]=(lightvalue>>8)&0xff;
    AvgValue = value[0] | ((unsigned int)value[1] << 8);
    
    if(AvgValue==32763)
    {
      HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
      HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);
    }
    else
    {
      HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
      HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
    }
  }
}

void zb_FindDeviceConfirm( uint8 searchType, 
                          uint8 *searchKey, uint8 *result )
{
  
}