#include "STC15F2K60S2.H"
/**************************
���ű����������£�
**************************/

sbit LED_SEL=P2^3;
/**************************
����������£�
**************************/
#define uint unsigned int
#define ulint unsigned long
//���ù��ձ���
uint n=0;						//ִ�й�Ĵ���
ulint sumn=0;				//��ADֵ���ܺ�
uint flag=0;				//���ر�־λ
uint light_old=1;		//��һ�ι�ǿ
uint light_new=1;		//��ǰ��ǿ
uint time=0;				//��ʱ


//��ʼ����ADC
void InitADC_light()
{
	P1ASF=0xff;
	ADC_RES=0;//ADת������Ĵ�������
	ADC_RESL=0;
	ADC_CONTR=0X8C;//10001100,��ADת����Դ������ADת��������λ����P1^4ΪADת������
	CLK_DIV=0X20;//����ADת����������ʽ
}

//AD�жϴ������
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;											//�ر��ж�
	
	if(time>2500)
	{
			light_new=(sumn+n/2)/n;		//��������
			sumn=0;
			n=0;
			time=0;
			if((light_old/light_new)>1.30)
			{					  
			  flag=~flag; 
			}
			time=0;	 
			light_old=light_new; 
	}

	//����ⲿ�ֵ�����
	n++;
	sumn+=ADC_RES*256+ADC_RESL; 	//��n��ADֵ�ĺ�

	ADC_CONTR&=~0X10;  					 //ת����ɺ�ADC_FLAG����
	ADC_CONTR|=0X08;	 					 //ת����ɺ�ADC_START��1
	IE=0xa0;					 					 //���жϣ�EA=1�����жϣ�EADC=1��AD�ж�
}

void main()
{	
	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
	LED_SEL=1;								//Ϩ��LED��

	IE=0xa0;							//���жϣ�EA=1�����жϣ�EADC=1��AD�ж�
	
	InitADC_light();
	while(1)
	{
		P0=0x00;
		if(flag)
		{
			P0=0x55;
		}
	}
}

