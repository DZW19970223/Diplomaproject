#include "ioCC2530.h"
#include "out.h"

#define  IN1  P1_2
#define  IN2  P1_3
#define  BEE  P0_1      
#define  JDQ  P0_6     
#define  JDQ1  P0_7
#define  JDQ2  P0_0
int BZ,LZ;

void Initout(void)
{
  P1DIR|=0x0c;
  P0DIR|=0xc2;
  IN1=0;
  IN2=0;
  BEE=0;        
  JDQ=0;
  JDQ1=0;   
  BZ=0;
  LZ=0;
}

void InitJDQ2()
{
  P0DIR|=0x01;
  JDQ2=0;
}

void L298NStart()
{
  IN1=1;
  IN2=0;
}

void L298NClose()
{
  IN1=0;
  IN2=0;
}

void JDQ1Start()
{
  JDQ1=1;
}

void JDQ1Close()
{
  JDQ1=0;
}

void BEEStart()
{
  BEE=1;
}

void BEEClose()
{
  BEE=0;
}

void JDQStart()
{
  JDQ=1;
}

void JDQClose()
{
  JDQ=0;
}

void JDQ2Start()
{
  JDQ2=1;
}

void JDQ2Close()
{
  JDQ2=0;
}
