#include "reg52.h"
#include <intrins.H>
#include "ds1302.c"//芯片驱动程序
#define u8 unsigned char

sfr P0M1 = 0x93;
sfr P0M0 = 0x94;
sfr P1M1 = 0x91;
sfr P1M0 = 0x92;
sfr P2M1 = 0x95;
sfr P2M0 = 0x96;
sfr P3M1 = 0xb1;
sfr P3M0 = 0xb2;
sfr P4M1 = 0xb3;
sfr P4M0 = 0xb4;
sfr P5M1 = 0xC9;
sfr P5M0 = 0xCA;

sfr AUXR = 0x8e;
sfr P5 = 0xc8;

sbit  LED0 = P2^0;
sbit  COM1 = P3^6;
sbit  COM2 = P3^7;
sbit  COM3 = P0^1;
sbit  COM4 = P0^0;
sbit  COM5 = P5^4;

//按钮定义
sbit  add = P3^2;
sbit  minus = P3^5;	
sbit  setting = P3^4;
sbit  exit = P3^3;

//   BGCF HADE
//0  1011 0111  0xb7
//1  1010 0000  0xa0
//2  1100 0111  0xc7
//3  1110 0110  0xe6
//4  1111 0000  0xf0
//5  0111 0110  0x76
//6  0111 0111  0x77
//7  1010 0110  0xa4
//8  1111 0111  0xf7
//9  1111 0110  0xf6
//a  1111 0101  0xf5
//b  0111 0011  0x73
//c  0001 0111  0x17
//d  1110 0011  0xe3
//E  0101 0111  0x57
//F  0101 0101  0x55
//-  0x40

code unsigned char ledtab[]={0xb7,0xa0,0xc7,0xe6,0xf0,0x76,0x77,0xa4,
                        0xf7,0xf6,0xf5,0x73,0x17,0xe3,0x57,0x55,0x00,0x40};

unsigned char thou,hund,ten,one, LED8, hour, minute,sec;
int counter1S,keymode,i,flashtimeout,count;
bit flash,dot;
								

u8 order(u8 a)
{
	u8 b,i;
	for (i=0;i<8;i++)
	{
		b <<= 1;
		b |= a&0x01;
		a >>= 1;
	}
	return b;
}
//二进制转bcd码
u8 bin2bcd(u8 i)
{
	i = ((i / 10) << 4) + i % 10;
	return i;
}
void delayms(int ms)
//这个软件大约可以等待ms毫秒
{
    int i,k;
    for(k=0; k<ms;k++)  {   for(i=0;i<500;i++);  }
}
//刷新
void refresh(void)
{
	
		P2 = ledtab[one];
    COM4=0;delayms(3); COM4=1;
  
    P2 = ledtab[ten];
    COM3=0;delayms(3);COM3=1;
  
    P2 = ledtab[hund]; 		if(dot) P2|=0x08;
    COM2=0;delayms(3);COM2=1;

  
    P2 = ledtab[thou];//thousand     
    COM1=0;delayms(3); COM1=1;

    P2 = LED8;//ledtab[thou];//thousand     
    COM5=0;delayms(3); COM5=1;
}


//显示
void DispTime(void)	//BCD code from 1302
{
	thou = hour/10;
	hund = hour%10;
	ten  = minute/10;
	one  = minute%10;
	LED8 = order(bin2bcd(sec));
}
//时分加减
void hourplus()
{
	if(++hour>23)hour=0;
	WRhour(hour);
	WRsec(0);

}
void hourminus()
{
	if(hour==0) hour =23;
	else hour--;
	WRhour(hour);
	WRsec(0);

}
void minuteplus()
{
	if(++minute>59) minute =0;
	WRmin(minute);
	WRsec(0);
}
void minuteminus()
{
	if(minute==0) minute =59;
	else minute--;
	WRmin(minute);
	WRsec(0);
}
void pluscheck()
{
	if(add ==0)   	
		{
			if(keymode ==1 ) hourplus();
			if(keymode ==2 ) minuteplus();
			flashtimeout =0; 
			flash = 0;
		}
}
void minuscheck(void)
{
	if(minus ==0)  	
		{
			if(keymode ==1 ) hourminus();
			if(keymode ==2 ) minuteminus();
			flashtimeout =0; 
			flash = 0;
		}
}

void wait(int n)
{
	for(i=0;i<n/3;i++)
	{
		delayms(1);refresh();
	}
}
void keyscan()
{	
	if(keymode != 0)
	{
		if(++flashtimeout>1000) {flashtimeout = 0; keymode =0;}
	}
		//判断设置模式
	if((minus ==0) || (add ==0)||(setting ==0)||(exit ==0))   
	{
		count =0;
		delayms(3);
		
		if(setting ==0)
		{
			if (++keymode ==3) keymode =1;
			flashtimeout =0; 
		}
		
		if(exit ==0)	keymode =0;
		
		minuscheck();
		pluscheck();
		
		do{				//等待键释放
			DispTime();
			refresh();
			//delayms(2);	//因为MCU处理速度远比人快，所以必须等待

			if(count < 200)
				{
						delayms(1);count+=1;
						refresh();
					}
			else if((count>=200) & (count < 205))
			{
				pluscheck();
				minuscheck();
				count += 1;//控制加的个数，这里大概加10个数会变得超快
				wait(200);
			}
			else
			{
				pluscheck();
				minuscheck();
				wait(100);
			}
		}while((minus ==0) || (add ==0)||(setting ==0)||(exit ==0));		
	
		//refresh();
	}
}

/*-------------------------------------------
Port Output Configuration Settings FOR STC
PxM1.y PxM0.y Port     Pin Mode for STC
 0      0      Quasi-bidirectional
 0      1      Push-Pull   高低电平强力输出，也叫推挽输出
 1      0      Input Only (High Impedance)
 1      1      Open Drain
此软件用芯片STC15W408
--------------------------------------------*/
//初始化
void PORTINIT(void)
{
	P2M1 = 0x00;  //这两句把P1口的所有8个IO脚设置成推挽输出
	P2M0 = 0xff;
   	P0M1 = 0x00;
   	P0M0 = 0x00;
	P5M1 = 0x00;  
	P5M0 = 0x00;
   	P3M1 = 0x00;  
   	P3M0 = 0x00;
	P1M1 = 0x00;  
	P1M0 = 0x00;
}

//主函数
void main(void)
{
	PORTINIT();
	COM5 = 0;
	P2 = 0;		//将P2所有LED都关闭
	unwrprt13();
	WRhour(20);WRmin(58);WRsec(0);
	keymode=0;
	while(1)
 	{
 		minus = 1;add = 1;setting = 1;exit = 1;
		keyscan();
		
		if(++counter1S >=40)
		{
			counter1S = 0; dot = !dot;
			
			hour = RDhour();
			minute = RDmin();	  	
			if(keymode==0) 	sec = RDsec(); 	
			
			flash = !flash;
		}
		DispTime();
		if(flash)
		{
			if(keymode == 1){thou = 16;hund = 16;}
			if(keymode == 2){ten = 16;one = 16;}
		}
		refresh();
	}
}

