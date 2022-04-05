#include "STC15F2K60S2.H"
#include "sys.H" 
#include "displayer.H" 
#include "key.h"
#include "hall.h"
#include "Vib.h"
#include "beep.h"
#include "music.h"
#include "adc.h"
#include "uart.h"
#include "stepmotor.h"
#include "stdio.h"                            // Keil C51 �⣬ʾ����ΪӦ����sprintf����

code unsigned long SysClock=11059200L;        //���롣 ������ϵͳ����ʱ��Ƶ�ʣ��û������޸ģ��ұ�����ʵ�ʹ���Ƶ�ʣ�����ʱѡ��ģ�һ��

#ifdef _displayer_H_                          //��ʾģ��ѡ��ʱ���롣���������ʾ������Ñ����޸ġ����ӵȣ� 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01 };   
	              /* ���:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13*        /
                /* ��ʾ:   0   1    2    3    4     5    6    7   8    9  (��)   ��-  ��-  ��-       */  
#endif

void my1Sfunc()
{ static i=0;
	i++;
	LedPrint(i);    
}

void my100mSfunc()
{ 
	static unsigned char i;
	i++;
	Seg7Print(12,10,10,10,10,i/100,(i/10)%10,i%10);              //����   
}



void main()
{  				
	  DisplayerInit();                                 //����
// 	
    SetEventCallBack(enumEventSys1S , my1Sfunc);
  	SetEventCallBack(enumEventSys100mS , my100mSfunc);
// 	
		SetDisplayerArea(0,7);                           //����

                                 //����
	     MySTC_Init();	             //���б��룡����   
	while(1)                         //���б��룡����   
		{  MySTC_OS();     				     //���б��룡����   
		}
}
