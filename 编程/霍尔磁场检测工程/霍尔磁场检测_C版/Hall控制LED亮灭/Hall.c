/**********************
mysbtHall �ų����
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------���ű�������---------*/
sbit sbtLedSel = P2 ^ 3;   //��������ܺ������ѡ��
sbit sbtHall = P1 ^ 2;     //�������ű�������

/*---------��������---------*/
uchar ucLed;           //���ڽ��ջ�����ֵ

/*---------��ʼ������---------*/
void Init()
{
    P2M1 = 0x00;
    P2M0 = 0xff;
    P0M1 = 0x00;
    P0M0 = 0xff;
    P1M1 = 0x00;
    P1M0 = 0xff;
    sbtLedSel = 1;
}

/*---------������---------*/
void main()
{
    Init();
    while( 1 )
    {
        ucLed = sbtHall;
        P0 = ucLed;
    }
}
