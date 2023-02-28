/* Main.c file generated by New Project wizard
 *
 * Created:   周四 6月 29 2017
 * Processor: 80C51
 * Compiler:  Keil for 8051
 */

#include <reg51.h>
#include <stdio.h>
#include<intrins.h> 
#define uchar unsigned char
#define uint unsigned int

uchar code SEGCC_CODE[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71}; //七段译码 
uchar code SEGLOCAL_CODE[]={0x40,0x20,0x10,0x08,0x04,0x02,0x01};//数码管选择
uchar xdata *px;//片选px
uchar xdata *wx;//位选wx 
uchar data watch[]={12,12,12};//时间显示计数 24 60 60
uchar data stopwatch[]={0,0,0};//秒表计数 60 60 99
uchar count1;//定时器1循环计数
uchar stopwatch_status;//秒表状态
bit watch_status;//显示状态切换

void myinitinterrupt();
void myinitvar();
void DelayMS(uint x);
void main(void)
 { 
   myinitinterrupt();
   myinitvar();
   while (1)
   {
      if(watch_status==0)
      {
	 //计时器 分显示
	 *px=SEGLOCAL_CODE[1];
	 *wx=SEGCC_CODE[stopwatch[0]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[2];
	 *wx=SEGCC_CODE[stopwatch[0]%10];
	 DelayMS(1);
	 //计时器 秒显示
	 *px=SEGLOCAL_CODE[3];
	 *wx=SEGCC_CODE[stopwatch[1]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[4];
	 *wx=SEGCC_CODE[stopwatch[1]%10];
	 DelayMS(1);
	 //计时器 毫秒显示
	 *px=SEGLOCAL_CODE[5];
	 *wx=SEGCC_CODE[stopwatch[2]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[6];
	 *wx=SEGCC_CODE[stopwatch[2]%10];
	 DelayMS(1);	 
      }
      else
      {
	 //时间 分显示
	 *px=SEGLOCAL_CODE[1];
	 *wx=SEGCC_CODE[watch[0]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[2];
	 *wx=SEGCC_CODE[watch[0]%10];
	 DelayMS(1);
	 //时间 秒显示
	 *px=SEGLOCAL_CODE[3];
	 *wx=SEGCC_CODE[watch[1]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[4];
	 *wx=SEGCC_CODE[watch[1]%10];
	 DelayMS(1);
	 //时间 毫秒显示
	 *px=SEGLOCAL_CODE[5];
	 *wx=SEGCC_CODE[watch[2]/10];
	 DelayMS(1);
	 *px=SEGLOCAL_CODE[6];
	 *wx=SEGCC_CODE[watch[2]%10];
	 DelayMS(1);	 
      }
      
   }//end while
 }
 //配置中断
void myinitinterrupt()
{
   EX0=1;
   IT0=1;
   //外部中断0负责秒表开始、暂停、结束
   EX1=1;
   IT1=1;
   //外部中断1负责时间显示开关，只允许非计时状态下使用 
   TMOD=0x11;
   ET0=1;
   TH0=(65536-10000)/256; 
   TL0=(65536-10000)%256; 
   //T0负责秒表计时，10MS一次
   ET1=1;
   TH1=(65536-50000)/256; 
   TL1=(65536-50000)%256; 
   count1=20;
   //T1负责正常时间计时，1S一次  

   TR1=1;
   EA=1;
}
//初始化变量
void myinitvar()
{
   stopwatch_status=0;//秒表状态 0:停止 1:运行 2:暂停 
   watch_status=0;//显示状态切换 0:计时状态 1:时间显示状态
   px=0xA002;//片选地址
   wx=0xA004;//位选地址
}
 //ms延时 
void DelayMS(uint x)
 {  
   uchar i;
   while(x--)
      {
	 for(i=0;i<120;i++);
      }
 }
 //秒表控制中断
void myint0() interrupt 0  
 {
    if(watch_status==0)
    {
       if(stopwatch_status==0)
       {
	  stopwatch_status=1;
	  TR0=1;
       }
       else if(stopwatch_status==1)
       {
	  stopwatch_status=2;
	  TR0=0;
       }
       else
       {
	  stopwatch_status=0;
	  stopwatch[0]=0;
	  stopwatch[1]=0;
	  stopwatch[2]=0;
	  TH0=(65536-10000)/256; 
	  TL0=(65536-10000)%256; 
       }
    }//end if
    else
    {
       ;//待添加时间修改功能
    }//end else
 }
//秒表时间控制中断
void mytime0() interrupt 1 
 {
    
    TH0=(65536-10000)/256; 
    TL0=(65536-10000)%256; 
    stopwatch[2]++;
    if(stopwatch[2]==100)
    {
       stopwatch[2]=0;
       stopwatch[1]++;
       if(stopwatch[1]==60)
       {
	  stopwatch[1]=0;
	  stopwatch[0]++;
	  if(stopwatch[0]==60)
	  {
	     stopwatch[0]=0;
	  }//end if
       }//end if
    }//end if
 }
 //显示状态控制中断
void myint1() interrupt 2
 {
    if(stopwatch_status==0)
    {
       watch_status=~watch_status;
    }
    else if(stopwatch_status==2)
    {     
       stopwatch_status=1;
       TR0=1;
    }
       
 }
 //时间显示控制中断
void mytime1() interrupt 3 
 {
    
    TH1=(65536-50000)/256; 
    TL1=(65536-50000)%256; 
    if(count1>0)
    {
       count1--;
       return;
    }
    count1=20;
    watch[2]++;
    if(watch[2]==60)
    {
       watch[2]=0;
       watch[1]++;
       if(watch[1]==60)
       {
	  watch[1]=0;
	  watch[0]++;
	  if(watch[0]==60)
	  {
	     watch[0]=0;
	  }//end if
       }//end if
    }//end if
 }
