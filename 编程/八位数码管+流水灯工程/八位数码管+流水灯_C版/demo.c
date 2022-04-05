/**********************
    :��λ�����+��ˮ��
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.h>
#define uint unsigned int
#define uchar unsigned char

uchar arrSeg7Select[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f}; //��ʾ0-8
uchar arrDigitSelect[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};   //�����0-7

/*---------���ű�������---------*/
sbit sbtLedSel = P2 ^ 3;  //�������LED���л�����

/*---------��������---------*/
uchar uiLed = 0x01;        //LED��ֵ�Ĵ�
uint  uiLedCnt = 0;        //LED���ۼƼ�����
uchar i = 0;               //�����ɨ����ʾѭ��

/*---------��ʼ������---------*/
void Init()
{
    P0M1 = 0x00;
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0x08;
    sbtLedSel = 0;  //��ѡ���������
}

/*---------��ʱ����---------*/
//��Ϊ����1ms����ʱ������ͨ���������n������������ʱn����
void delay_ms( uint n )
{
    while( n )
    {
        uchar i, j;
        i = 11;
        j = 190;
        do
        {
            while ( --j );
        }
        while ( --i );
        n--;
    }
}

/*---------������---------*/
void main()
{
    Init();
    while( 1 )
    {
        sbtLedSel = 0;
        for( i = 0; i < 8; i++ )
        {
            P0 = 0;
            P2 = arrDigitSelect[i];     //ѡ������ܵ�λ��
            P0 = arrSeg7Select[i + 1];  //��ʾ��Ӧ����ֵ
            delay_ms( 1 );
        }
        uiLedCnt++;
        sbtLedSel = 1;
        P0 = uiLed;                     //LED��ʾ
        delay_ms( 1 );                  //��ʱ200ms
        if( uiLedCnt == 50 )
        {
            if( uiLed == 0x80 )        //value����0x80ʱ�����¸���ֵ0x01
                uiLed = 0x01;
            else
                uiLed = uiLed << 1;    //valueֵ��һ����
            uiLedCnt = 0;
        }
    }
}
