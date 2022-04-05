/**********************
myMusic ���ֲ���
�ͺ�:IAP15F2K60S2 ��Ƶ:12MHz
************************/
#include <STC15F2K60S2.h>
#define uint unsigned int
#define uchar unsigned char

/*---------���ű�������---------*/
sbit sbtBeep = P3 ^ 4;    //������

/*---------��������---------*/
uchar ucTimerH, ucTimerL;   //���嶨ʱ������װֵ
uchar code arrMusic[] =     //���ִ��룬����Ϊ��ͬһ�׸衷����ʽΪ: ����, ����
{
//������ʮλ�����ǵ��и߰˶ȣ�1����߰˶ȣ�2�����а˶ȣ�3����߰˶�
//��λ������׵�����������0x15����Ͱ˶ȵ�S0��0x21�����а˶ȵ�DO��
//�������Ǵ������������磺0x10����һ�ģ�0x20�������ģ�0x08����1/2��
0x15,0x20,0x21,0x10,	 //������ʮλ�����ǵͰ˶ȣ��а˶Ȼ��Ǹ߰˶ȣ�1����Ͱ˶ȣ�2�����а˶ȣ�3����߰˶�
0x22,0x10,0x23,0x18,	 //��λ������׵�����������0x15����Ͱ˶ȵ�S0��0x21�����а˶ȵ�DO��
0x24,0x08,0x23,0x10,	 //�������Ǵ������������磺0x10����һ�ģ�0x20�������ģ�0x08����1/2��
0x21,0x10,0x22,0x20,
0x21,0x10,0x16,0x10,
0x21,0x40,0x15,0x20,
0x21,0x10,0x22,0x10,
0x23,0x10,0x23,0x08,
0x24,0x08,0x25,0x10,
0x21,0x10,0x24,0x18,
0x23,0x08,0x25,0x10,
0x22,0x08,0x23,0x08,
0x23,0x08,0x22,0x08,
0x22,0x30,0x23,0x20,
0x25,0x10,0x31,0x10,
0x27,0x18,0x26,0x08,
0x26,0x20,0x25,0x10,
0x25,0x08,0x26,0x08,
0x27,0x10,0x26,0x08,
0x25,0x08,0x23,0x40,
0x24,0x18,0x24,0x08,
0x25,0x10,0x26,0x10,
0x25,0x10,0x24,0x08,
0x23,0x08,0x22,0x20,
0x17,0x10,0x17,0x08,
0x16,0x08,0x15,0x10,
0x16,0x10,0x21,0x40,
0x00,0x00
};
uchar code arrMusicToTimerNum[] =  
{
    //����������Ϊ���������ڶ�ʱ���е���װֵ����һ���Ǹ�λ���ڶ����ǵ�λ
    0xf8, 0x8c,   //�Ͱ˶ȣ���1
    0xf9, 0x5b,
    0xfa, 0x15,   //��3
    0xfa, 0x67,
    0xfb, 0x04,   //��5
    0xfb, 0x90,
    0xfc, 0x0c,   //��7
    0xfc, 0x44,   //����C��
    0xfc, 0xac,   //��2
    0xfd, 0x09,
    0xfd, 0x34,   //��4
    0xfd, 0x82,
    0xfd, 0xc8,   //��6
    0xfe, 0x06,
    0xfe, 0x22,   //�߰˶ȣ���1
    0xfe, 0x56,
    0xfe, 0x6e,   //��3
    0xfe, 0x9a,
    0xfe, 0xc1,   //��5
    0xfe, 0xe4,
    0xff, 0x03    //��7
};

/*---------��ʱ�Ӻ���---------*/
void DelayMs( unsigned int xms )
{
    uint i, j;
    for( i = xms; i > 0; i-- )
        for( j = 124; j > 0; j-- );
}

/*---------ȡַ�Ӻ���---------*/
//ȡ��tem������arrMusicToTimerNum�����е�λ��ֵ
uchar GetPosition( uchar tem ) 
{
    uchar ucBase, ucOffset, ucPosition;     //����������������λ��
    ucBase = tem / 16;            //��4λ������ֵ,��ַ
    ucOffset = tem % 16;          //��4λ��������ƫ����
    if( ucBase == 1 )              //������ֵΪ1ʱ�����ǵͰ˶ȣ���ַΪ0
        ucBase = 0;
    else if( ucBase == 2 )          //������ֵΪ2ʱ�������а˶ȣ���ַΪ14
        ucBase = 14;
    else if( ucBase == 3 )          //������ֵΪ3ʱ�����Ǹ߰˶ȣ���ַΪ28
        ucBase = 28;
    //ͨ����ַ����ƫ���������ɶ�λ��������arrMusicToTimerNum�����е�λ��
ucPosition = ucBase + ( ucOffset - 1 ) * 2; 
    return ucPosition;            //������һ��λ��ֵ
}

/*---------�������ֹ��ܺ���---------*/
void PlayMusic()
{
    uchar ucNoteTmp, ucRhythmTmp, tem; // ucNoteTmpΪ������ucRhythmTmpΪ����
    uchar i = 0;
    while( 1 )
    {
        ucNoteTmp = arrMusic[i];    //�������������,��ʱ1��,�ص���ʼ����һ��
        if( ucNoteTmp == 0x00 )
        {
            i = 0;
            DelayMs( 1000 );
        }
        else if( ucNoteTmp == 0xff )  //��������ֹ��,��ʱ100ms,����ȡ��һ����
        {
            i = i + 2;
            DelayMs( 100 );
            TR0 = 0;
        }
        else                     //���������ȡ�����ͽ���
        {
            //ȡ����ǰ������arrMusicToTimerNum�����е�λ��ֵ
tem = GetPosition( arrMusic[i] );              
//��������Ӧ�ļ�ʱ����װ��ֵ����ucTimerH��ucTimerL
ucTimerH = arrMusicToTimerNum[tem];  
            ucTimerL = arrMusicToTimerNum[tem + 1];
            i++;
            TH0 = ucTimerH;           //��ucTimerH��ucTimerL�����ʱ��
            TL0 = ucTimerL;
            ucRhythmTmp = arrMusic[i];      //ȡ�ý���
            i++;
        }
        TR0 = 1;                          //����ʱ��1
        DelayMs( ucRhythmTmp * 180 );      //�ȴ��������, ͨ��P3^4�������Ƶ
        TR0 = 0;                          //�ض�ʱ��1

    }
}

/*---------��ʼ���Ӻ���---------*/
//����������IO��
void InitSys()
{
    P0M0 = 0xff;
    P0M1 = 0x00;
    P2M0 = 0x08;
    P2M1 = 0x00;
    P3M0 = 0x10;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
}

/*---------��ʱ��0��ʼ���Ӻ���---------*/
void InitT0()
{
    TMOD = 0x01;
    TH0 = 0xD8;
    TL0 = 0xEF;
    EA = 1;
    ET0 = 1;
    TR0 = 0;
}

/*---------������---------*/
void main()
{
    InitSys();
    InitT0();
    P0 = 0x00;
    PlayMusic();
    while( 1 );
}

/*---------��ʱ��0�жϴ�����---------*/
//����װֵ������sbtBeepֵȡ������������
void T0_Process() interrupt 1                     //��ʱ������Ƶ��
{
    TH0 = ucTimerH;
    TL0 = ucTimerL;
    sbtBeep = ~sbtBeep;
}
