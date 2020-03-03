#include "ioCC2530.h"
#include "RFP.h"
#include "OnBoard.h"

#define RFP P0_7

void RFPInit()
{
  P0SEL &= ~0X80;  
  P0DIR &= ~0X80;    
}

uchar RfpScan()
{
  RFPInit();
  if(RFP==0)
  {
    MicroWait(10000);
    if(RFP==0)
    {
      return 1;
    }
  }
  return 0;
}
