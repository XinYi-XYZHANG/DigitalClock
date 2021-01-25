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
sbit  timesetting = P3^4;
sbit  lightsetting = P3^3;

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
int thoumode,hundmode,tenmode,onemode,lightnumber;
bit flash,dot;
								
//二进制转bcd码
u8 bin2bcd(u8 i)
{
	i = ((i / 10) << 4) + i % 10;
	return i;
}

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

void delayms(int ms)
//这个软件大约可以等待ms毫秒
{
    int i,k;
    for(k=0; k<ms;k++)  {   for(i=0;i<500;i++);  }
}
//第一个灯亮度选择
void thourefresh(void)
{
	if(thoumode==1)//最亮
		{
			P2 = ledtab[thou];
			COM1=0;delayms(12); COM1=1;
		}
		else if(thoumode==2)//第二亮
		{
			P2 = ledtab[thou];COM1=0;delayms(6); 
			P2=0; delayms(6);COM1=1;
		}
		else if(thoumode==3)//第二暗
		{
			P2 = ledtab[thou];COM1=0;delayms(3); 
			P2=0; delayms(9);COM1=1;
		}
		else if(thoumode==4)//最暗
		{
			P2 = ledtab[thou];COM1=0;delayms(1); 
			P2=0; delayms(11);COM1=1;
		}
}
//第二个灯亮度选择
void hundrefresh(void)
{
	if(hundmode==1)//最亮
		{
			P2 = ledtab[hund];
			COM2=0;delayms(12); COM2=1;
		}
		else if(hundmode==2)//第二亮
		{
			P2 = ledtab[hund];COM2=0;delayms(6); 
			P2=0; delayms(6);COM2=1;
		}
		else if(hundmode==3)//第二暗
		{
			P2 = ledtab[hund];COM2=0;delayms(3); 
			P2=0; delayms(9);COM2=1;
		}
		else if(hundmode==4)//最暗
		{
			P2 = ledtab[hund];COM2=0;delayms(1); 
			P2=0; delayms(11);COM2=1;
		}
}
//第三个灯亮度选择
void tenrefresh(void)
{
	if(tenmode==1)//最亮
		{
			P2 = ledtab[ten];
			COM3=0;delayms(12); COM3=1;
		}
		else if(tenmode==2)//第二亮
		{
			P2 = ledtab[ten];COM3=0;delayms(6); 
			P2=0; delayms(6);COM3=1;
		}
		else if(tenmode==3)//第二暗
		{
			P2 = ledtab[ten];COM3=0;delayms(3); 
			P2=0; delayms(9);COM3=1;
		}
		else if(tenmode==4)//最暗
		{
			P2 = ledtab[ten];COM3=0;delayms(1); 
			P2=0; delayms(11);COM3=1;
		}
}
//第四个灯亮度选择
void onerefresh(void)
{
	if(onemode==1)//最亮
		{
			P2 = ledtab[one];
			COM4=0;delayms(12); COM4=1;
		}
		else if(onemode==2)//第二亮
		{
			P2 = ledtab[one];COM4=0;delayms(6); 
			P2=0; delayms(6);COM4=1;
		}
		else if(onemode==3)//第二暗
		{
			P2 = ledtab[one];COM4=0;delayms(3); 
			P2=0; delayms(9);COM4=1;
		}
		else if(onemode==4)//最暗
		{
			P2 = ledtab[one];COM4=0;delayms(1); 
			P2=0; delayms(11);COM4=1;
		}
}
//界面刷新
void refresh(void)
{
	thourefresh();
	hundrefresh();
	tenrefresh();
	onerefresh();
	P2 = LED8;
  COM5=0;delayms(12); COM5=1;
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
//时分加减限制判断
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

//加判断
void pluscheck()
{
	if(add ==0)   	
		{
			if(keymode ==1 ) hourplus();
			else if(keymode ==2 ) minuteplus();
			
			if(lightnumber==1){if(--thoumode<1) thoumode=1;}
			else if(lightnumber==2){if(--hundmode<1) hundmode=1;}
			else if(lightnumber==3){if(--tenmode<1) tenmode=1;}
			else if(lightnumber==4){if(--onemode<1) onemode=1;}
			
			flashtimeout =0; 
			flash = 0;
		}
}
//减判断
void minuscheck(void)
{
	if(minus ==0)  	
		{
			if(keymode ==1 ) hourminus();
			else if(keymode ==2 ) minuteminus();
			
			if(lightnumber==1){if(++thoumode>4) thoumode=4;}
			else if(lightnumber==2){if(++hundmode>4) hundmode=4;}
			else if(lightnumber==3){if(++tenmode>4) tenmode=4;}
			else if(lightnumber==4){if(++onemode>4) onemode=4;}
			
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
	if(keymode != 0)//长时间不调节时间，退出调节时间模式
	{
		if(++flashtimeout>1000) {flashtimeout = 0; keymode =0;}
	}
	
	if(lightnumber != 0)//长时间不调节亮度，退出调节亮度模式
	{
		if(++flashtimeout>1000) {flashtimeout = 0; lightnumber =0;}
	}
	
		//判断设置模式
	if((minus ==0) || (add ==0)||(timesetting ==0)||(lightsetting ==0))   
	{
		count =0;
		delayms(3);

		if(lightsetting==0)
		{
			keymode=0;
			if(++lightnumber>4)lightnumber=0;
			do{ //等待键释放
				delayms(3);
			}while(lightsetting ==0);
		}
		if(timesetting ==0)
		{
			lightnumber=0;
			if (++keymode ==3) keymode =0;
			flashtimeout =0; 
		}
		
		//判断加减
		minuscheck();
		pluscheck();
		
		//长按加速增减
		do{				//等待键释放
			DispTime();
			refresh();

			if(count < 40)
				{
						delayms(1);count+=1;
						refresh();
					}
			else if((count>=40) & (count < 45))
			{
				pluscheck();
				minuscheck();
				count += 1;//控制加的个数，这里大概加5个数会变得超快
				wait(60);
			}
			else
			{
				pluscheck();
				minuscheck();
				wait(20);
			}
		}while((minus ==0) || (add ==0)||(timesetting ==0)||(lightsetting ==0));		
	
	}
}

/*-------------------------------------------
Port Output Configuration timesettings FOR STC
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
	thoumode=1;hundmode=1;tenmode=1;onemode=1;
	lightnumber=0;
	while(1)
 	{
 		minus = 1;add = 1;timesetting = 1;lightsetting = 1;
		keyscan();
		
		if(++counter1S >=40)
		{
			counter1S = 0; dot = !dot;
			
			hour = RDhour();	//	i = RDsec();
			minute = RDmin();	  	//	i=RDmin();
			if(keymode==0) 	sec = RDsec(); 	//	i = RDmin();
			
			flash = !flash;
		}
		DispTime();
		if(flash)
		{
			if(keymode == 1){thou = 16;hund = 16;}
			else if(keymode == 2){ten = 16;one = 16;}
			
			if(lightnumber == 1){thou = 16;}
			else if(lightnumber == 2){hund = 16;}
			else if(lightnumber == 3){ten = 16;}
			else if(lightnumber == 4){one = 16;}

		}
		refresh();
	}
}

