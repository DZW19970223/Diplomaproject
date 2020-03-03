#include <ioCC2530.h>
#include "SG90.h"
#include "OnBoard.h"

#define SG90 P0_6

void Init_Port(void)
{
  P0SEL &= ~(1<<6);  
  P0DIR |= (1<<6);   
  SG90 = 0;       
}

void SG90start(void)
{
  int k=1;
  Init_Port();
  for(int i=0;i<160;i++)
  {
    if(k==1)
    {
      for(int j=0;j<3;j++)
      {
        SG90=1;
        MicroWait(500);
      }
      for(int a=0;a<37;a++)
      {
        SG90=0;
        MicroWait(500);
      }
    }
    if(k==0)
    {
      for(int j=0;j<1;j++)
      {
        SG90=1;
        MicroWait(500);
      }
      for(int a=0;a<39;a++)
      {
        SG90=0;
        MicroWait(500);
      }
    }
    if(i==149)
      k=0;
    if(i==159)
      k=1;
  }
}
