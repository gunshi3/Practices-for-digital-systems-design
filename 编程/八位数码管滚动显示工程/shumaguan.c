/**********************
mySeg7Shift ��λ����ܹ�����ʾ
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include "STC15F2K60S2.H"
#define uint unsigned int
#define uchar unsigned char

/*---------���ű�������---------*/
sbit sbtSel0 = P2 ^ 0; //
sbit sbtSel1 = P2 ^ 1; //
sbit sbtSel2 = P2 ^ 2; //λѡ���������ſ���λ

/*---------��������---------*/
uchar ucDig1Tmp;
uchar ucDig2Tmp;
uchar ucDig3Tmp;
uchar ucDig4Tmp;
uchar ucDig5Tmp;
uchar ucDig6Tmp;
uchar ucDig7Tmp;
uchar ucDig8Tmp;//show_wi(i=1,2,3,4,����,8)�ֱ��Ƕ�Ӧ���ҵĸ���������ϵ���ʾ������
uchar ucSeg7State;
uchar ucCount;
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x40, 0x00}; //��ѡ����ʾ0-f
uchar arrDigSelect[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //λѡ,ѡ����0-7�е�һ�������

/*---------��ʼ������---------*/
void Init()
{
    P2M0 = 0xff;
    P2M1 = 0x00;
    P0M0 = 0xff;
    P0M1 = 0x00; //P0��P2������Ϊ�������

    ucSeg7State = 0;
    ucCount = 0;

    ucDig1Tmp = 0; //�ʼ����ܴ�������ʾ0-7
    ucDig2Tmp = 1;
    ucDig3Tmp = 2;
    ucDig4Tmp = 3;
    ucDig5Tmp = 4;
    ucDig6Tmp = 5;
    ucDig7Tmp = 6;
    ucDig8Tmp = 7;

    TMOD = 0x01;             //��ʱ��0����ʽ1
    ET0 = 1;                  //������ʱ���ж�
    TH0 = ( 65535 - 1000 ) / 256; //��ʱ��0�ĸ߰�λ����
    TL0 = ( 65535 - 1000 ) % 256; //��ʱ��0�ĵͰ�λ���ã���������������ö�ʱ��0�ĳ�ʼֵ��1ms
    TR0 = 1;                 //������ʱ��
    EA = 1;                  //���ܵ��ж�
}

/*---------��ʱ��T0�жϷ�����---------*/
void T0_Process() interrupt 1     //������ܵ���ʾ�ᵽ�ж���������
{
    TH0 = ( 65535 - 1000 ) / 256; //����װ�ض�ʱ��0�ĳ�ʼֵ��Ϊ����һ�ζ�ʱ�����׼��
    TL0 = ( 65535 - 1000 ) % 256;
    ucSeg7State++;         //������������ã����������Ƶ���ã���ɨ���������ʾ��ucSeg7State������ܵ�����
    if( ucSeg7State == 8 )    //���з�Ƶ��ÿ�жϰ˴β���ucCount��ֵ��һ��
    {
        ucSeg7State = 0;
        ucCount++;
    }
    if( ucCount == 100 )     //���ǵ�ɨ��Ƶ�ʺܸ������ٴη�Ƶ��ucCount�ӵ�100��ִ��
    {
        ucCount = 0;
        ucDig1Tmp++;     //�ô����Ҹ���������ϵ����ֶ���һ
        ucDig2Tmp++;
        ucDig3Tmp++;
        ucDig4Tmp++;
        ucDig5Tmp++;
        ucDig6Tmp++;
        ucDig7Tmp++;
        ucDig8Tmp++;
    }
    P0 = 0;                       //���������ʾ���Ӻã�������һ��P0����ֵ��Ӱ��
    P2 = arrDigSelect[ucSeg7State];   //λѡ��ѡ��ucSeg7State�������
    switch( ucSeg7State )           //ÿ���ж���ʾһ�����������ʾ
    {
        case 0:
            P0 = arrSegSelect[ucDig1Tmp % 10];
            break;//�����ң���һ���������ʾ
        case 1:
            P0 = arrSegSelect[ucDig2Tmp % 10];
            break;//�����ң��ڶ����������ʾ
        case 2:
            P0 = arrSegSelect[ucDig3Tmp % 10];
            break;//�����ң��������������ʾ
        case 3:
            P0 = arrSegSelect[ucDig4Tmp % 10];
            break;//�����ң����ĸ��������ʾ
        case 4:
            P0 = arrSegSelect[ucDig5Tmp % 10];
            break;//�����ң�������������ʾ
        case 5:
            P0 = arrSegSelect[ucDig6Tmp % 10];
            break;//�����ң��������������ʾ
        case 6:
            P0 = arrSegSelect[ucDig7Tmp % 10];
            break;//�����ң����߸��������ʾ
        default:
            P0 = arrSegSelect[ucDig8Tmp % 10];
            break;//�����ң��ڰ˸��������ʾ
    }
}

/*---------������---------*/
void main()
{
    Init(); //��ʼ��
    while( 1 )
    {
    }
}
