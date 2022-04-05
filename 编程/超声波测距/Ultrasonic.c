#include "STC15F2K60S2.H" 

/**************************
���ű����������£�
**************************/
sbit SEL0=P2^0;
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;
sbit Echo=P1^0;						//����
sbit Trig=P1^1;						//����

/**************************
����������£�
**************************/
#define uint unsigned int
float TIME;								//һ�����ڵ�ʱ��
float receive;
uint receiveint; 					//����
char qianwei=0;
char baiwei=0;
char shiwei=0;
char gewei=0;
char global=0;

char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x71};

void SEG_Display();
void date_process();
void Delay(int n);

/**********************
�������ƣ�void ultr()
�������������ó�����ģ��HC-SR04�Ѳ�õľ�����ʾ���������
***********************/
void main()
{
	TIME=12/11.0592;						//�Ĵ���AUXR��T1x12=0,12��Ƶ

	P0M1=0x00;									//����
  P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;	
	SEL3=0;

	TMOD=0x11;									//ͬʱʹ�ö�ʱ��T0��T1

  EA=1;												//���ܵ��ж�
	ET1=1;											//������ʱ��1�ж�	
 	TH1=(65535-60000)/256;			//60ms
	TL1=(65535-60000)%256;		
	TR1=1;											//������ʱ��

	while(1)
	{
	 SEG_Display();
	}
}

//���嶨ʱ��1�жϣ�����ÿ������60ms��һ���ź�
void Timer1_Routine() interrupt 3
{
	global++;
	if(global>10)
	{
		ET1=0;										 //�ر�T1�ж�
		TR1=0;										 //�رն�ʱ��1
		TH1=(65535-60000)/256;
		TL1=(65535-60000)%256;		//���¸�ֵ
		
		Trig=1;				
		Delay(2);									//����20us�������ȵĴ����ź�
		Trig=0;

		while(!Echo);							//�ȴ������źű�ߵ�ƽ
		TR0=1;										//������ʱ��0
		
		while(Echo);							//�ȴ������źű�͵�ƽ
		TR0=0;										//�رն�ʱ��0
		
		//ͨ������Ƶ�ʣ�����ʱ������������ٶȿ��Բ�þ���
		receive=(340*TIME*((float)TH0*256+(float)TL0))/2000;
		receiveint=(int)receive;
		if(receiveint<30||receiveint>4000)
		{	
			receiveint=0;
		}
		date_process();
	
		TH0=0;										//��ʱ��0��ֵ0
		TL0=0;

		ET1=1;										//�򿪶�ʱ��T1
		TR1=1;
		global=0;
	}
}

void date_process()
{
	qianwei=receiveint/1000;
	baiwei=receiveint%1000/100;
	shiwei=receiveint%100/10;
	gewei=receiveint%10;	
}
void weixuan(char i)
{
	SEL2=i/4;
	SEL1=i%4/2;
	SEL0=i%2;
}
void SEG_Display()
{
	P0=0;
	weixuan(4);
	P0=segtable[qianwei];
	Delay(30);
	P0=0;
	weixuan(5);
	P0=segtable[baiwei];
	Delay(30);
	P0=0;
	weixuan(6);
	P0=segtable[shiwei];
	Delay(30);
	P0=0;
	weixuan(7);
	P0=segtable[gewei];
	Delay(30);
	
}

void Delay(int n)
{
	int x;
	while(n--)
	{
		x=50;
		while(x--);
	}
}