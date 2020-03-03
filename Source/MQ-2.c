#include <ioCC2530.h>
#include "MQ-2.h"
#include "OnBoard.h"
#include "out.h"

#define AIR P0_0      //烟雾为P0.0口控制
extern int BZ;	
void AirInit(); 

void AirInit()
{
  P0SEL &= ~0X01;     //设置P0.0为普通IO口  
  P0DIR &= ~0X01;    // 在P0.0口，设置为输入模式 
}

uchar AirScan(void)
{    
  AirInit();
  if(AIR==0)
  {
    MicroWait(10000);
    if(AIR==0)
    {
      if(BZ==0)
      {
        BEEStart();
        JDQStart();
      }
      return 1;   //有烟雾          
    }
  }
  if(BZ==0)
  {
    BEEClose();
    JDQClose();
  }
  return 0;           //无烟雾
}