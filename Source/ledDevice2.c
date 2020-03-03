#include "smartHome.h"
#include "sapi.h"
#include "hal_led.h"
#include "dht11.h"
#include "adc.h"
#include "MQ-2.h"
#include "out.h"
#include "OnBoard.h"
#include "stdio.h"
#include "string.h"

#define NUM_LED_1 3
#define NUM_LED_2 4

#define LEDNUM  2
uint8 ledIdList[LEDNUM]={NUM_LED_1,NUM_LED_2};

#define NUM_IN_CMD_LEDDEVICE 1
#define NUM_OUT_CMD_LEDDEVICE 2

extern int BZ,LZ;

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
    
    if(led_num==NUM_LED_1)
    {
      if(led_status==1)
        HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
      else if(led_status==0)
        HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
      else if(led_status==2)
      {
        BEEStart();
      }
      else if(led_status==3)
      {
        BEEClose();
      }
      else if(led_status==4)
      {
        JDQStart();
      }
      else if(led_status==5)
      {
        JDQClose();
      }
      else if(led_status==6)
      {
        BZ=1;
      }
      else if(led_status==7)
      {
        BZ=0;
      }
    }
    else if(led_num==NUM_LED_2)
    {
      if(led_status==1)
        HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);
      else if(led_status==0)
        HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
      else if(led_status==2)
      {
        L298NStart();
      }
      else if(led_status==3)
      {
        L298NClose();
      }
      else if(led_status==4)
      {
        JDQ1Start();
      }
      else if(led_status==5)
      {
        JDQ1Close();
      }
      else if(led_status==6)
      {
        LZ=1;
      }
      else if(led_status==7)
      {
        LZ=0;
      }
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
    osal_start_timerEx(sapi_TaskID,DHT11_PERIOD_EVT,5000);
    HalLedBlink(HAL_LED_ALL,5,50,2000);
    
    Initout();
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
  
  if(event&DHT11_PERIOD_EVT)
  {
    unsigned char value[5];//0字节：温度； 1字节：湿度； 2、3字节：光照强度； 4字节：火警标志
    unsigned int lightvalue;
    unsigned int AvgValue;
    osal_start_timerEx(sapi_TaskID,DHT11_PERIOD_EVT,5000);
    GetDht11Value(value,value+1);
    Get_ADCValue(&lightvalue);
    value[2]=lightvalue&0xff;
    value[3]=(lightvalue>>8)&0xff;
    value[4]=AirScan();
    AvgValue = value[2] | ((unsigned int)value[3] << 8);
    if(LZ==0)
    {
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
    zb_SendDataRequest(0X0,DHT11VALUE_CMD_ID,
                       5,value,0,FALSE,AF_DEFAULT_RADIUS); 
    
  }
}

void zb_FindDeviceConfirm( uint8 searchType, 
                          uint8 *searchKey, uint8 *result )
{
  
}