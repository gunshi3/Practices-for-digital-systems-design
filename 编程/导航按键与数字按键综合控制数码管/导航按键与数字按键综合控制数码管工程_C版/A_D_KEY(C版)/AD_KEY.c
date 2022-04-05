/**********************
myNavKey ��������+���ְ������������
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#include <intrins.h>
#define uint  unsigned int
#define uchar unsigned char

/*---------�궨��---------*/
#define cstAdcPower 0X80     /*ADC��Դ����*/
#define cstAdcFlag 0X10      /*��A/Dת����ɺ�cstAdcFlagҪ�������*/
#define cstAdcStart 0X08     /*��A/Dת����ɺ�cstAdcStart���Զ����㣬����Ҫ��ʼ��һ��ת��������Ҫ��λ*/
#define cstAdcSpeed90 0X60   /*ADCת���ٶ� 90��ʱ������ת��һ��*/
#define cstAdcChs17 0X07     /*ѡ��P1.7��ΪA/D����*/

/*---------���ű�������---------*/
sbit sbtLedSel = P2 ^ 3;     /*����ܺͷ��������ѡ��λ*/
sbit sbtSel0 = P2 ^ 0;
sbit sbtSel1 = P2 ^ 1;
sbit sbtSel2 = P2 ^ 2;
sbit sbtKey1 = P3 ^ 2;
sbit sbtKey2 = P3 ^ 3;

/*---------��������---------*/
uchar ucSegSelectState;         /*��ѡ��־*/
uchar ucDigSelectState;         /*λѡ��־*/
bit btKey3Flag;             /*key3�����±�־*/
//0123456789ABCDEF
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00};
//ѡ����һλ�����
uchar arrDigSelect[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

/*---------��ʼ������--------*/
void Init()
{
    P0M1 = 0x00;
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0xff;

    sbtLedSel = 0;      //ѡ���������Ϊ���
    P1ASF = 0x80;       //P1.7��Ϊģ�⹦��A/Dʹ��
    ADC_RES = 0;        //ת���������
    ADC_CONTR = 0x8F;   //cstAdcPower = 1
    CLK_DIV = 0X00;     //ADRJ = 0    ADC_RES��Ÿ߰�λ���

    btKey3Flag = 0;
    ucDigSelectState = 0;
    ucSegSelectState = 0;

    IT0 = 0;            //����IT0�����ش���
    IT1 = 0;
    EA = 1;             //CPU�����ж�
}

/*---------��ʱ5ms�Ӻ���--------*/
void Delay5ms()     //@11.0592MHz  ��ʱ5ms
{
    unsigned char i, j;
    i = 54;
    j = 199;
    do
    {
        while ( --j );
    }
    while ( --i );
}

/*---------��ʱ100ms�Ӻ���--------*/
void Delay100ms()       //@11.0592MHz  ��ʱ100ms
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
    i = 5;
    j = 52;
    k = 195;
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

/*---------��ȡADֵ�Ӻ���--------*/
unsigned char GetADC()
{
    uchar ucAdcRes;
    ADC_CONTR = cstAdcPower | cstAdcStart | cstAdcSpeed90 | cstAdcChs17;//û�н�cstAdcFlag��1�������ж�A/D�Ƿ����
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while( !( ADC_CONTR & cstAdcFlag ) ); //�ȴ�ֱ��A/Dת������
    ADC_CONTR &= ~cstAdcFlag;           //cstAdcFlagE�����0
    ucAdcRes = ADC_RES;                 //��ȡAD��ֵ
    return ucAdcRes;
}

/*---------��ȡ��������ֵ�Ӻ���--------*/
uchar NavKeyCheck()
{
    unsigned char key;
    key = GetADC();     //��ȡAD��ֵ
    if( key != 255 )    //�а�������ʱ
    {
        Delay5ms();
        key = GetADC();
        if( key != 255 )            //�������� ���а�������
        {
            key = key & 0xE0;       //��ȡ��3λ������λ����
            key = _cror_( key, 5 ); //ѭ������5λ ��ȡA/Dת������λֵ����С���
            return key;
        }
    }
    return 0x07;        //û�а�������ʱ����ֵ0x07
}

/*---------�������������Ӻ���--------*/
void NavKey_Process()
{
    uchar ucNavKeyCurrent;  //����������ǰ��״̬
    uchar ucNavKeyPast;     //��������ǰһ��״̬

    ucNavKeyCurrent = NavKeyCheck();    //��ȡ��ǰADCֵ
    if( ucNavKeyCurrent != 0x07 )       //���������Ƿ񱻰��� ������0x07��ʾ�а���
    {
        ucNavKeyPast = ucNavKeyCurrent;
        while( ucNavKeyCurrent != 0x07 )        //�ȴ����������ɿ�
            ucNavKeyCurrent = NavKeyCheck();

        switch( ucNavKeyPast )
        {
            case 0x00 :                     //K3
                if( btKey3Flag == 0 )
                {
                    btKey3Flag = 1;
                }
                else
                {
                    btKey3Flag = 0;
                }
                break;
            case 0x05 :                     //�ϼ�����ʾ�����ּ�1
                if( ucSegSelectState == 15 )
                {
                    ucSegSelectState = 0;
                }
                else
                    ucSegSelectState++;
                break;
            case 0x02 :                     //�¼�����ʾ�����ּ�1
                if( ucSegSelectState == 0 )
                    ucSegSelectState = 15;
                else
                    ucSegSelectState--;
                break;
        }
    }

    Delay100ms();
}

/*---------������--------*/
void main()
{
    Init();
    P0 = 0x00;
    while( 1 )
    {
        NavKey_Process();         //��ȡ�����������
        P2 = arrDigSelect[ucDigSelectState];           //��ʾλ
        P0 = arrSegSelect[ucSegSelectState];       //��ʾ����
        if( btKey3Flag == 0 )   //KEY3=0��K1,K2������������K1,K2������
        {
            if( sbtKey1 == 0 )    //sbtKey1���£���ʾλ����һλ
            {
                Delay5ms();
                if( sbtKey1 == 0 )
                {
                    while( !sbtKey1 );

                    if( ucDigSelectState == 7 )
                        ucDigSelectState = 0;
                    else
                        ucDigSelectState++;
                }
            }
            if( sbtKey2 == 0 )   //sbtKey2���£���ʾλ����һλ
            {
                Delay5ms();
                if( sbtKey2 == 0 )
                {
                    while( !sbtKey2 );
                    if( ucDigSelectState == 0 )
                        ucDigSelectState = 7;
                    else
                        ucDigSelectState--;
                }
            }
        }
    }
}
