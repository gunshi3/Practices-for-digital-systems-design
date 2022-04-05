#include "STC15F2K60S2.H"

sbit SEL0=P2^0;			//��������
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;
sbit beep=P3^4;		//����������
sbit Key1=P3^2;	   //Key1   �л���־λ 

#define uint unsigned int
#define ulint unsigned long
#define uchar unsigned char	
#define NMAX_KEY 100  //100�ζ�ȡ����ֵ 

uint time=0;				//��ʱ

uint l=0;						//ִ�й�Ĵ���
ulint suml=0;				//��ADֵ���ܺ�

uint light=0;				//��
uint light_down=20; //��������
uint light_up=100;	//��������
uchar flag=0;				//�л�ģʽ��0�������ޣ�1��������

//����������ʾ�����������
uint light_bai=0;		//��λ
uint light_shi=0;		//ʮλ
uint light_ge =0;		//��λ
//����ֵ
uint down_bai=0;		//��λ
uint down_shi=0;		//ʮλ
uint down_ge =0;		//��λ
//����ֵ
uint up_bai=0;		//��λ
uint up_shi=0;		//ʮλ
uint up_ge =0;		//��λ
/**********************

��������
***********************/
uchar G_count;
uchar display;
uint Key1_count;
uint Key2_count;
uint Key3_count;
uint Key_count;

bit Key1_C;				//key1��ǰ��״̬
bit Key1_P;				//key1ǰһ��״̬

//���������ʾ0-F
char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
								 0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

void Delay(int n);
void weixuan(char i);
void SEG_Display();
void check();
void InitADC_light();
void date_processlight();
void date_processlight_down();
void date_processlight_up();

void Delay(int n)			//��ʱ����
{
	int x;
	while(n--)
	{
		x=60;
		while(x--);
	}
}

void weixuan(char i)	//�����λ��ѡ��
{
	SEL2=i/4;
	SEL1=i%4/2;
	SEL0=i%2;
}
void SEG_Display()
{
	//�������ù��λ��
	P0=0;
	weixuan(5);
	P0=segtable[light_bai];
	Delay(10);
	
	P0=0;
	weixuan(6);
	P0=segtable[light_shi];
	Delay(10);
	
	P0=0;
	weixuan(7);
	P0=segtable[light_ge];
	Delay(10);
	
	if(flag==0)    //flagΪ0 ��ʾ����ֵ 020
	{
		P0=0;
		weixuan(0);
		P0=segtable[down_bai];
		Delay(10);
	
		P0=0;
		weixuan(1);
		P0=segtable[down_shi];
		Delay(10);
	
		P0=0;
		weixuan(2);
		P0=segtable[down_ge];
		Delay(10);
	} 
	if(flag==1)   //flagΪ1 ��ʾ����ֵ 100
	{
		P0=0;
		weixuan(0);
		P0=segtable[up_bai];
		Delay(10);
	
		P0=0;
		weixuan(1);
		P0=segtable[up_shi];
		Delay(10);
	
		P0=0;
		weixuan(2);
		P0=segtable[up_ge];
		Delay(10);
	}
}

void main()
{
	P1M1=0x00;            //��P1^7����Ϊ����ģʽ������Ϊ׼˫���ģʽ
	P1M0=0x80;         
	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
	P3M1=0x00;
	P3M0=0x10;					  //����P3^4Ϊ����ģʽ
	SEL3=0;								//Ϩ��LED��
	
	/*��ʼ�����а����ĵ�ǰ״̬��ǰһ��״̬*/
	Key1_C=1;							//key1��ǰ��״̬
	Key1_P=1;							//key1ǰһ��״̬
	Key1_count=0x80+NMAX_KEY/3*2;
	Key_count=NMAX_KEY;
	
	IE=0xa8;							//EA=1�����жϣ�EADC=1����A/Dת���жϣ�ET1=1����T1�ж�	
	TMOD=0x10;						//ʹ�ö�ʱ��1��16λ������װ��ģʽ��TH1��TL1ȫ��
	TH1=(65535-100)/256;	//��8λ����ֵ����ʱ400����
	TL1=(65535-100)%256;	//��8λ����ֵ
	TR1=1;								//������ʱ��1
	
	InitADC_light();
	while(1)
	{
		SEG_Display();
	}
}

void InitADC_light()		//��ʼ����ADC
{
	P1ASF=0xff;           //��P1����Ϊģ�⹦��A/Dʹ��
	ADC_RES=0;            //ת���������
	ADC_RESL=0;
	ADC_CONTR=0x8c;				//ADC_POWER=1��A/Dת������Դ;ADC_START=1����ģ��ת����ADC;CHS=100ѡ��P1^4��ΪA/D����ʹ��
	CLK_DIV=0x20;
}

//�ֱ�ȡ���¶Ⱥ͹��յİ�λ��ʮλ����λ
void date_processlight_down()
{
	down_bai=light_down%1000/100;
	down_shi=light_down%100/10;
	down_ge =light_down%10;
}
void date_processlight_up()
{
	up_bai=light_up%1000/100;
	up_shi=light_up%100/10;
	up_ge =light_up%10;
}
void date_processlight()
{
	light_bai=light%1000/100;
	light_shi=light%100/10;
	light_ge =light%10;
}

void check()
{
			if(Key1==0)
			Key1_count--;
	
			Key_count--;					//�ܵĴ�����1

			if(Key_count==0)			//100������
			{
	
				if(Key1_count<0x80)
				{
					Key1_C=0;
					if(Key1_P==1)			//�½��أ�������������
					{
						Key1_P=0;
						flag=!flag;
					}
				}
				if(Key1_count>=0x80)
				{
					Key1_C=1;
					if(Key1_P==0)
					   Key1_P=1;			//�����أ����費�������Ǿͼ�����
				}
	
				//��һ�ֵ��ж�
				Key1_count=0x80+NMAX_KEY/3*2;
				Key_count=NMAX_KEY;
			}
}

void Timer1_Routine() interrupt 3	//3Ϊ��ʱ��1�жϱ��
{
	IE=0x00;												//�ر����ж�
	TR1=0;													//��ʱ��1ֹͣ
	TH1=(65535-100)/256;						//���¸�ֵ
	TL1=(65535-100)%256;

	check();
	if(flag==0)
	{
		if(light<light_down)
				beep=~beep;				  		 //��������ʹ�÷���������
		else beep=0;								 //��ʹ�÷�����ʱ����������0�������ڵ͵�ƽ
	}
	if(flag==1)
	{
		if(light>light_up)
				beep=~beep;				 		  //��������ʹ�÷���������
		else beep=0;
	}

	IE=0xa8;											//�����ж�
	TR1=1;												//������ʱ��1
}

//AD�ж�
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;											//�ر��ж�
	
	if(time>2000)                 //ȡ���ֵ��ƽ��ֵ��С���
	{

			light=(suml+l/2)/l;				//��������
			suml=0;
			l=0;
			time=0;
	   	date_processlight();
			date_processlight_down();
			date_processlight_up();
	}

		//����ⲿ�ֵ�����
		l++;
		suml+=ADC_RES*256+ADC_RESL;//��l��ADֵ�ĺ�

	ADC_CONTR&=~0X10;  					 //ת����ɺ�ADC_FLAG����
	ADC_CONTR|=0X08;	 					 //ת����ɺ�ADC_START��1
	IE=0xa8;					 					 //���ж�
}