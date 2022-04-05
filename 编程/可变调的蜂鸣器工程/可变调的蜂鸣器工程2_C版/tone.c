#include "STC15F2K60S2.H"   
#define uchar unsigned char
#define uint  unsigned int
#define X 1.05946

sbit beep=P3^4;						
sbit P3_3=P3^3;
sbit P3_2=P3^2;
uint y=956;
uint i=1;
uchar duanxuan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};		//��ѡ 0-f


void delay(uint t)		                   //��ʱ����  ��ʱtms
{
	uint j;  
	for(;t>0;t--)   
    	for(j=800;j>0;j--) 
		;								
}

void int0() interrupt 0			          
{
    EX0=0;								  //�ر��ⲿ�ж�0
	delay(10);							  //����
	if(P3_2==0)
	{									  //������k1ʱ���ı��ѡ�붨ʱ��0�ĳ�ʼֵ�Ӷ��ı����뵽�������ķ���Ƶ��
	    if(i==7)	   					  //ѭ���������1-7
		{y=956;
		 P0=duanxuan[1];
		 i=1;
		 }
		else if(i==3)					
		{y=y/X;i++;P0=duanxuan[i];}
		else
		{y=y/X/X;i++;P0=duanxuan[i];}
	}
	while(!P3_2);						 //�жϼ��ɨ�����
	EX0=1;								 //�����ⲿ�ж�0
}

void int1() interrupt 2							
{
	EX1=0;							   //�ر��ⲿ�ж�1
    delay(10);						   //����
	if(P3_3==0)
	{
		if(TR0)
			{TR0=0;beep=0;}			  //����ʱ��0������ر�
		else
			{TR0=1;}				  //����ʱ��0�ر�����
	}
	while(!P3_3);					  //�жϼ��ɨ�����
	EX1=1;							  //�����ⲿ�ж�1
}

void timer0() interrupt 1
{
	beep=~beep;
	TH0=(65536-y)/256;				  //������ͬ����ʱ����ʼֵ��ͬ
	TL0=(65536-y)%256;
} 

main()
{
	P3M0=0x10;		                  //�������
	P3M1=0x00;
	P2M0=0x0f;
	P2M1=0x00;
	P1M0=0xff;
	P1M1=0x00;
	TMOD=0x01;						  //��ʱ��0λ1��ʽ����
	TH0=(65536-y)/256;
	TL0=(65536-y)%256;
	EA=1;                             //�ж��ܿ���
	ET0=1;							  //��ʱ��0����
	TR0=1;
	EX1=1;
	IT1=1;							  //�ⲿ�жϿ���
	EX0=1;
	IT0=1;
	IP=0x05;
	beep=0;
	P2=0x07;						  //λѡ��Ϊ���ұ�һλ
	P0=duanxuan[1];					  
	while(1)
	{
	}
}