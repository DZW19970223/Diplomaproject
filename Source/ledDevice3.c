#include "smartHome.h"
#include "sapi.h"
#include "hal_led.h"
#include "adc.h"
#include "IC.h"
#include "out.h"
#include "RFP.h"
#include "OnBoard.h"
#include "stdio.h"
#include "string.h"

#define NUM_LED_1 5
#define NUM_LED_2 6

#define LEDNUM  2
uint8 ledIdList[LEDNUM]={NUM_LED_1,NUM_LED_2};

#define NUM_IN_CMD_LEDDEVICE 1
#define NUM_OUT_CMD_LEDDEVICE 2

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
    }
    else if(led_num==NUM_LED_2)
    {
      if(led_status==1)
        HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);
      else if(led_status==0)
        HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
      else if(led_status==2)
      {
        while(IC_Test()==1)
          JDQ2Start();   
        JDQ2Close(); 
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
    osal_start_timerEx(sapi_TaskID,RC522b_PERIOD_EVT,2000);
    HalLedBlink(HAL_LED_ALL,5,50,2000);
    
    IC_Init();   
    InitJDQ2();
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
  
  if(event&RC522b_PERIOD_EVT)
  {
    osal_start_timerEx(sapi_TaskID,RC522b_PERIOD_EVT,2000);
    unsigned char asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},i;
    unsigned char Card_Id[9]; //存放32位卡号和水压标志
    
    unsigned int lightvalue;
    unsigned int AvgValue;
    unsigned char value[2];
    
    if(IC_Test()==1)
    {   
      //16进制转ASC码 串口打印IC卡号码
      for(i=0;i<4;i++)
      {
        Card_Id[i*2]=asc_16[qq[i]/16];
        Card_Id[i*2+1]=asc_16[qq[i]%16];        
      } 
      Card_Id[8]=RfpScan();
      zb_SendDataRequest(0X0,RC522bVALUE_CMD_ID,9,Card_Id,0,FALSE,AF_DEFAULT_RADIUS);
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