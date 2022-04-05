/**********************
myVib �񶯴�����
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.h>
#include <intrins.h>      //_nop_();
#define uchar unsigned char

uchar code arrLed[] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff}; //Ledֵ
/*---------���ű�������---------*/
sbit sbtVib = P2 ^ 4;     //�񶯴�����
sbit sbtLedSel = P2 ^ 3;  //�������LED���л�����

/*---------��ʱ����---------*/
void Delay40ms()        //��ʱ40ms@11.0592MHz
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
    i = 2;
    j = 175;
    k = 75;
    do
    {
        do
        {
            while ( --k );
        }
        while ( --j );
    }
    while ( --i );
}

/*---------��ʼ������---------*/
void Init()
{
    P0M0 = 0xff;
    P0M1 = 0x00;
    P2M0 = 0x08;    //P2.3���������
    P2M1 = 0x00;
    sbtLedSel = 1;
}

/*---------������---------*/
void main()
{
    uchar i = 0;
    Init();
    sbtVib = 1;
    P0 = 0x00;                  //��ʼLED��Ϊ��
    while( 1 )
    {
        if( sbtVib == 0 )      //����⵽�͵�ƽ˵���񶯷���������LED
        {
            i = 0;
            while( i < 9 )
            {
                P0 = arrLed[i];
                Delay40ms();    //��ʱ
                i++;
            }
        }
        else
            P0 = 0x00;
    }
}
