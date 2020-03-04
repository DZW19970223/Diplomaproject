#include <ioCC2530.h>
#include <stdio.h>
#include <string.h>
#include "codetab.h"
#define uchar unsigned char
#define uint  unsigned int
#define OLED_CMD  0    //写命令
#define OLED_DATA 1    //写数据
#define IIC_READ  0x1D      //定义读指令
#define IIC_WRITE 0x1D      //定义写指令
#define  u8 unsigned char 
#define SDA P1_4              //I2C 数据传送位
#define SCL P1_5              //I2C 时钟传送位
#define OLEDadd 0x78
#define Max_Column    128
#define Max_Row        64
#define  u32 unsigned int 
#define iic_delay() Delay_1u(8)


void Delay_1u(uint microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs*/
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}



uchar ack_sign = 0;
/*********************************************************************/
/**************               I2C通信部分              ***************/
/*********************************************************************/
void iic_start()               //函数功能：I2C通信开始
{
    SDA = 1;
    iic_delay();
    SCL = 1;
    iic_delay();
    SDA = 0;
    iic_delay();
}

void iic_stop()                //函数功能：I2C通信停止
{
    SDA = 0;
    iic_delay();
    SCL = 1;
    iic_delay();
    SDA = 1;
    iic_delay();
}

void iic_ack()                 //函数功能：I2C通信查应答位
{ 
    SDA = 1;
    SCL = 1;
    iic_delay();
    ack_sign = SDA;
    SCL = 0;    
}

void iic_write_byte(uchar wdata)//函数功能：向I2C从机写入一个字节
{
     uchar i,temp,temp1;

     temp1 = wdata;
     
     for (i = 0; i < 8; i++) 
     {
        SCL = 0;
        iic_delay();
        temp = temp1;
        temp = temp&0x80;
        SDA = (temp == 0x80? 1: 0);
        iic_delay();
        SCL = 1;
        iic_delay();
        SCL = 0;
        iic_delay();
        temp1 <<= 1;
     }

}

void iic_write(uchar byte_add,uchar wdata)//函数功能：按地址写入一字节数据
{
     uchar t;
  t = OLEDadd;
  iic_start();   //起始信号
  iic_write_byte(t); 
  iic_ack();
  iic_write_byte(byte_add);//内部寄存器地址
  iic_ack();
  iic_write_byte(wdata);//内部寄存器数据
  iic_ack();
  iic_stop();
}


void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
    if(cmd)
            {

  // Write_IIC_Data(dat);
   iic_write(0x40,dat);
   
   
        }
    else {
    iic_write(0x00,dat);
        
    }


}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
    unsigned char m,n;
    for(m=0;m<8;m++)
    {
        OLED_WR_Byte(0xb0+m,0);        //page0-page1
        OLED_WR_Byte(0x00,0);        //low column start address
        OLED_WR_Byte(0x10,0);        //high column start address
        for(n=0;n<128;n++)
            {
                OLED_WR_Byte(fill_Data,1);
            }
    }
}


/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
    unsigned int m;
    for(;Del_50ms>0;Del_50ms--)
        for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
    unsigned char j;
    while(Del_1ms--)
    {    
        for(j=0;j<123;j++);
    }
}

//坐标设置

    void OLED_Set_Pos(unsigned char x, unsigned char y) 
{     OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD); 
}         
//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}                        
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!      
void OLED_Clear(void)  
{  
    u8 i,n;            
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
        for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
    } //更新显示
}
void OLED_On(void)  
{  
    u8 i,n;            
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
        for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
    } //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示                 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{          
    unsigned char c=0,i=0;    
        c=chr-' ';//得到偏移后的值            
        if(x>Max_Column-1){x=0;y=y+2;}
        if(Char_Size ==16)
            {
            OLED_Set_Pos(x,y);    
            for(i=0;i<8;i++)
            OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
            OLED_Set_Pos(x,y+1);
            for(i=0;i<8;i++)
            OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
            }
            else {    
                OLED_Set_Pos(x,y);
                for(i=0;i<6;i++)
                OLED_WR_Byte(F6x8[c][i],OLED_DATA);
                
}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
    u32 result=1;     
    while(n--)result*=m;    
    return result;
}                  
//显示2个数字
//x,y :起点坐标     
//len :数字的位数
//size:字体大小
//mode:模式    0,填充模式;1,叠加模式
//num:数值(0~4294967295);               
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{             
    u8 t,temp;
    u8 enshow=0;                           
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
                continue;
            }else enshow=1; 
              
        }
         OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
    }
} 
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {        OLED_ShowChar(x,y,chr[j],Char_Size);
            x+=8;
        if(x>120){x=0;y+=2;}
            j++;
    }
}

void LCD_P16x16Ch(unsigned char x, unsigned char y, unsigned char N)

{
  
  unsigned char wm=0;
  
  unsigned int adder=32*N; // 
  
  OLED_Set_Pos(x , y);
  
  for(wm = 0;wm < 16;wm++) // 
    
  {
    
    //LCD_WrDat(F16x16[adder]); 
    
    OLED_WR_Byte(F16x16[adder],OLED_DATA);
    
    adder += 1;
    
  } 
  
  OLED_Set_Pos(x,y + 1); 
  
  for(wm = 0;wm < 16;wm++) // 
    
  {
    
    //LCD_WrDat(F16x16[adder]);
    OLED_WR_Byte(F16x16[adder],OLED_DATA);
    
    adder += 1;
    
  } 
  
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{     
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
        {      
            OLED_WR_Byte(BMP[j++],OLED_DATA);            
        }
    }
} 

//初始化SSD1306                        
void OLED_Init(void)
{     
  
   //初始化
    P1SEL &= ~(1<<4); // P1_4 is GPIO
    P1SEL &= ~(1<<5); // P1_5 is GPIO

    P1DIR |= 1<<4; //output
    P1DIR |= 1<<5; //output
  
 
    OLED_WR_Byte(0xAE,OLED_CMD);//--display off
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
    
    OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
    
    OLED_WR_Byte(0x81,OLED_CMD); // contract control
    OLED_WR_Byte(0xFF,OLED_CMD);//--128   
    OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
    
    
    OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
    
    OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
    OLED_WR_Byte(0x00,OLED_CMD);//
    
    OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
    OLED_WR_Byte(0x80,OLED_CMD);//
    
    OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
    OLED_WR_Byte(0x05,OLED_CMD);//
    
    OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
    OLED_WR_Byte(0xF1,OLED_CMD);//
    
    OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
    OLED_WR_Byte(0x12,OLED_CMD);//
    
    OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
    OLED_WR_Byte(0x30,OLED_CMD);//
    
    OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
    OLED_WR_Byte(0x14,OLED_CMD);//
    
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}