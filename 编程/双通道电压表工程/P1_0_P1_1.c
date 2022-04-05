#include "STC15F2K60S2.H"

sbit SEL0=P2^0;				//��������
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;

#define uint unsigned int
#define ulint unsigned long

uint time=0;					//��ʱ
uint flag=1;					//��־λ������P1_0��P1_1

uint count_0=0;				//ִ��P1_0�Ĵ���
uint count_1=0;				//ִ��P1_1�Ĵ���
ulint sum_0=0;				//P1_0�ӿ�ADֵ���ܺ�
ulint sum_1=0;				//P1_1�ӿ�ADֵ���ܺ�

uint P1_0=0;					//P1_0
uint P1_1=0;					//P1_1 

//����������ʾP1_0��ѹ����������(��ѹ��Χ��0~5֮�䣬��ʾС�������λ)
uint U0_bai=0;				//��λ(��ѹ��λ����λ)
uint U0_shi=0;				//ʮλ(С�����ʮ��λ)
uint U0_ge =0;				//��λ(С�����ٷ�λ)

//����������ʾP1_1��ѹU1����������
uint U1_bai=0;				//��λ
uint U1_shi=0;				//ʮλ
uint U1_ge =0;				//��λ

//�����м�ת������
float f0=0.0,f1=0.0;
int i0=0,i1=0;

//���������ʾ0-F
char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
								 0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//��ʾС���������(0~5V)
char segtabletwo[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed};

void Delay(int n);
void weixuan(char i);
void SEG_Display();
void InitADC_P1_0();
void InitADC_P1_1();
void date_processP1_0();
void date_processP1_0();

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
  //������ʾP1_0��ѹֵ
	P0=0;
	weixuan(0);
	P0=segtabletwo[U0_bai];
	Delay(10);

	P0=0;
	weixuan(1);
	P0=segtable[U0_shi];
	Delay(10);
	
	P0=0;
	weixuan(2);
	P0=segtable[U0_ge];
	Delay(10);

  //������ʾP1_1��ѹֵ
	P0=0;
	weixuan(5);
	P0=segtabletwo[U1_bai];
	Delay(10);
	
	P0=0;
	weixuan(6);
	P0=segtable[U1_shi];
	Delay(10);
	
	P0=0;
	weixuan(7);
	P0=segtable[U1_ge];
	Delay(10);
}

/**********************
�������ƣ�void U_T()
�����������ö�ʱ��1ʹ��AD��ʱ�ɼ�P1.0��P1.1�ڵ�ADֵ������ʾ��������ϣ������ʾP1.0���Ҳ���ʾP1.1
��ڲ�������
���ڲ�������
��ע��
***********************/

void U0_U1()
{
	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
	SEL3=0;								//Ϩ��LED��
	
	IE=0xa8;							//EA=1�����жϣ�EADC=1����A/Dת���жϣ�ET1=1����T1�ж�	
	TMOD=0x10;						//ʹ�ö�ʱ��1��16λ������װ��ģʽ��TH1��TL1ȫ��
	TH1=(65535-40000)/256;//��8λ����ֵ����ʱ40000����
	TL1=(65535-40000)%256;//��8λ����ֵ
	TR1=1;								//������ʱ��1
	
	while(1)
	{
		SEG_Display();
	}
}

void InitADC_P1_0()			//��ʼ��P1_0
{
	P1ASF=0xff;						//��P1����Ϊģ�⹦��A/Dʹ��
	ADC_RES=0;						//�Ĵ���ADC_RES��ADC_RESL����A/Dת�����
	ADC_RESL=0;						//��ʼ��ֵ0
	ADC_CONTR=0x88;				//ADC_POWER=1��A/Dת������Դ;ADC_START=1����ģ��ת����ADC;CHS=000ѡ��P1^0��ΪA/D����ʹ��
	CLK_DIV=0x20;					//ADRJ=1��ADC_RES[1:0]��Ÿ�2λADC�����ADC_RESL[7:0]��ŵ�8λADC���
}
void InitADC_P1_1()			//��ʼ��P1_1
{
	P1ASF=0xff;
	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=0x89;				//CHS=001ѡ��P1^1��ΪA/D����ʹ��
	CLK_DIV=0x20;
}

//�ֱ�ȡ��U0��U1�ĵ�ѹֵ(������λС��)
void date_processP1_0()
{
			i0=f0;
			U0_bai=i0;

			i0=f0*10;
			U0_shi=i0%10;

			i0=f0*100;
			U0_ge=i0%10;
}
void date_processP1_1()
{
			i1=f1;
			U1_bai=i1;

			i1=f1*10;
			U1_shi=i1%10;

			i1=f1*100;
			U1_ge=i1%10;
}

void Timer1_Routine() interrupt 3	//3Ϊ��ʱ��1�жϱ��
{
	IE=0x00;							//�ر����ж�
	TR1=0;								//��ʱ��1ֹͣ
	TH1=(65535-40000)/256;//���¸�ֵ
	TL1=(65535-40000)%256;

	if(flag==1)
	{
		InitADC_P1_1();			//��ʼ��P1_1
	}
	else
	{
		InitADC_P1_0();			//��ʼ��P1_0
	}
	flag=-flag;

	IE=0xa8;							//�����ж�
	TR1=1;								//������ʱ��1
}

//AD�ж�
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;							//�ر��ж�
	
	if(time>2000)
	{
		if(flag==1)												 //��ʱflag=1ִ���¶Ȳ���
		{	
			P1_0=(sum_0+count_0/2)/count_0;	 //��������
			f0=(5*P1_0)/1024.0;							 //ת���ɵ�ѹ
			sum_0=0;
			count_0=0;
			time=0;
			date_processP1_0();	
		}


		if(flag==-1)												//��ʱflag=-1ִ�йⲿ��
		{
			P1_1=(sum_1+count_1/2)/count_1;		//��������
			f1=(5*P1_1)/1024.0;		 						//ת���ɵ�ѹ
			sum_1=0;
			count_1=0;
			time=0;
	   	date_processP1_1();
		}    
	}

	if(flag==1)
	{
		//��ӦP1_0����
		count_0++;
		sum_0+=ADC_RES*256+ADC_RESL;					//��count_0��ADֵ�ĺ�

	}
	if(flag==-1)
	{
		//��ӦP1_1����
		count_1++;
		sum_1+=ADC_RES*256+ADC_RESL;					//��count_1��ADֵ�ĺ�
	}

	ADC_CONTR&=~0X10;  					 //ת����ɺ�ADC_FLAG����
	ADC_CONTR|=0X08;	 					 //ת����ɺ�ADC_START��1
	IE=0xa8;					 					 //���ж�
}

void main()
{
	U0_U1();
}