/**********************
myXstep2 �ɿز������(����ת��ͣ)
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.h>
#define uint unsigned int

/*---------�궨��---------*/
#define cstKeyMaxNum 100                     //100�ζ�ȡ����ֵ

uint code arrSegSelect[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66,
                             0x6d, 0x7d, 0x07, 0x7f, 0x6f,
                             0x77, 0x7c, 0x39, 0x5e, 0x79,
                             0x71, 0x40, 0x00
                           };   //�������ʾ0~4��16���ƴ���

/*---------���ű�������---------*/
sbit sbtKey1 = P3 ^ 2;                                               //����1
sbit sbtKey2 = P3 ^ 3;                                               //����2
sbit sbtKey3 = P1 ^ 7;                                               //����3
sbit sbtS1  = P4 ^ 1;
sbit sbtS2  = P4 ^ 2;
sbit sbtS3  = P4 ^ 3;
sbit sbtS4  = P4 ^ 4;

/*---------��������---------*/
uint uiKey1Cnt;
uint uiKey2Cnt;
uint uiKey3Cnt;
uint uiKeyAllCnt;

uint uiSpeed = 1;                                    //���ƿ��Ʋ���������ٶ�

uint i = 1;                                          //���������Ӧ��IO��λ��
bit  btKey1Current = 1;                              //Key1��ǰ��״̬
bit  btKey1Past = 1;                                 //Key1ǰһ��״̬
bit  btKey2Current = 1;                              //Key2��ǰ��״̬
bit  btKey2Past = 1;                                 //Key2ǰһ��״̬
bit  btKey3Current = 1;                               //Key3��ǰ��״̬
bit  btKey3Past = 1;                                  //Key3ǰһ��״̬
bit  btRotationFlag = 0;                               //��ǲ������ת��


/*---------��ʼ������--------*/
void InitSys()           //����������IO��,������ʱ��0��1
{
    P0M0 = 0xff;                                   //P0�������������
    P0M1 = 0x00;

    P4M0 = 0x3f;                                    //P4�������������
    P4M1 = 0xe1;
    P4 = 0xe1;      //����P4��Ϊ�͵�ƽ�����⸴λʱ�������5V��ƽ�ӿڵ�Ӱ��

    P2M0 = 0x07;                                  //P2.3�������������
    P2M1 = 0x00;
    P2 = 0X07;                                     //�����������ʾ

    uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKeyAllCnt = cstKeyMaxNum;

    AUXR =  0X00;                   //��ʱ��0�Ͷ�ʱ��1 ʱ��1Tģʽ
    TMOD =  0X00;                  //���ö�ʱ��0�Ͷ�ʱ��1 16λ�Զ���װģʽ
    TL0  = ( 65536 - 250 ) % 256;       //���ö�ʱ0��ʱΪ 250��s
    TH0  = ( 65536 - 250 ) / 256;
    TH1  = ( 65536 - 5000 * uiSpeed ) / 256;       //���ö�ʱ1��ʱΪ 5ms*uiSpeed
    TL1  = ( 65536 - 5000 * uiSpeed ) % 256;
    TCON = 0X50;                        //��ʱ��0��1��ʼ��ʱ
    IE   = 0x8A;                         //��ʱ��0��1�����жϣ�CPU�����ж�
}

/*---------T1��ʱ���жϷ�������--------*/
void T1_Process() interrupt 3                //��������Ƶ��
{
    TH1  = ( 65536 - 5000 * uiSpeed ) / 256;  //ͨ���ڶ�ʱ��1��ʱ�����ı�����Ƶ��
    TL1  = ( 65536 - 5000 * uiSpeed ) % 256;
    if( btRotationFlag == 0 )                //���Ʋ��������ת
    {
        switch( i++ )                     //ѭ���ı䲻ͬIO�ŵ������ƽ
        {
            case 1:
                sbtS1 = 1; sbtS2 = 0; sbtS3 = 0; sbtS4 = 0; break;
            case 2:
                sbtS1 = 0; sbtS2 = 1; sbtS3 = 0; sbtS4 = 0; break;
            case 3:
                sbtS1 = 0; sbtS2 = 0; sbtS3 = 1; sbtS4 = 0; break;
            case 4:
                sbtS1 = 0; sbtS2 = 0; sbtS3 = 0; sbtS4 = 1; break;
        }
        if( i == 5 )
            i = 1;
    }                                                          
    else                                 //���Ʋ��������ת
    {
        switch( i++ )
        {
            case 1:
                sbtS1 = 0; sbtS2 = 0; sbtS3 = 0; sbtS4 = 1; break;
            case 2:
                sbtS1 = 0; sbtS2 = 0; sbtS3 = 1; sbtS4 = 0; break;
            case 3:
                sbtS1 = 0; sbtS2 = 1; sbtS3 = 0; sbtS4 = 0; break;
            case 4:
                sbtS1 = 1; sbtS2 = 0; sbtS3 = 0; sbtS4 = 0; break;
        }
        if( i == 5 )
            i = 1;
    }
}
/*---------T0��ʱ���жϷ�������--------*/
void T0_Process() interrupt 1  //��������+�жϴ������Ʋ������ת����ٶȣ�
{
    if( sbtKey1 == 0 )
        uiKey1Cnt--;
    if( sbtKey2 == 0 )
        uiKey2Cnt--;
    if( sbtKey3 == 0 )
        uiKey3Cnt--;                                  //�����ǰ���״̬
    uiKeyAllCnt--;                                     //�ܵĴ�����1

    if( uiKeyAllCnt == 0 )                                //100������
    {
        if( uiKey2Cnt < 0x80 )
        {
            btKey2Current = 0;
            if( btKey2Past == 1 )       //�½��أ������򲽽������ת�ٸı䣬//uiSpeedֵԽ��ת��Խ����
            {
                btKey2Past = 0;
                uiSpeed++;
                if( uiSpeed == 12 )
                    uiSpeed = 1;
            }

        }
        if( uiKey2Cnt >= 0x80 )
        {
            btKey2Current = 1;
            if( btKey2Past == 0 )
                btKey2Past = 1;          //�����أ����費�������Ǿͼ�����
        }

        if( uiKey1Cnt < 0x80 )
        {
            btKey1Current = 0;
            if( btKey1Past == 1 )          //�½��أ������ı䲽�����ת��
            {
                btKey1Past = 0;
                btRotationFlag = ~btRotationFlag;
            }

        }
        if( uiKey1Cnt >= 0x80 )
        {
            btKey1Current = 1;
            if( btKey1Past == 0 )
                btKey1Past = 1;         //�����أ����費�������Ǿͼ�����
        }

        if( uiKey3Cnt < 0x80 )
        {
            btKey3Current = 0;
            if( btKey3Past == 1 )          //�½��أ������ı䲽�����ת��
            {
                btKey3Past = 0;
                TR1 = ~TR1;
            }

        }
        if( uiKey3Cnt >= 0x80 )
        {
            btKey3Current = 1;
            if( btKey3Past == 0 )
                btKey3Past = 1;              //�����أ����費�������Ǿͼ�����
        }

        uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;          //��һ�ֵ��ж�
        uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
        uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
        uiKeyAllCnt = cstKeyMaxNum;
    }
}

/*---------������---------*/
void main()
{
    InitSys();
    while( 1 )
    {
        P0 = arrSegSelect[uiSpeed]; // P0����ʾ��������ٶȣ���Χ1-4,1���ٶ����
    }
} 
