#include <ioCC2530.h>
#include "MQ-2.h"
#include "OnBoard.h"
#include "out.h"

#define AIR P0_0      //����ΪP0.0�ڿ���
extern int BZ;	
void AirInit(); 

void AirInit()
{
  P0SEL &= ~0X01;     //����P0.0Ϊ��ͨIO��  
  P0DIR &= ~0X01;    // ��P0.0�ڣ�����Ϊ����ģʽ 
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
      return 1;   //������          
    }
  }
  if(BZ==0)
  {
    BEEClose();
    JDQClose();
  }
  return 0;           //������
}