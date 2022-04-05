/**********************
myBeep2 �ɱ���������
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------���ű�������---------*/
sbit sbtBeep = P3 ^ 4;                  //����������
sbit sbtKey1 = P3 ^ 2;                  //����1����
sbit sbtKey2 = P3 ^ 3;                  //����2����
sbit sbtSel0 = P2 ^ 0;                  //λѡ�ź�λ
sbit sbtSel1 = P2 ^ 1;                  //λѡ�ź�λ
sbit sbtSel2 = P2 ^ 2;                  //λѡ�ź�λ
sbit sbtSel3 = P2 ^ 3;                  //LED���������ʾ�Ŀ���λ

/*---------��������---------*/
uint sbtKey1_state = 0;         //0��Key1δ����         1��Key1�Ѱ���
uint sbtKey2_state = 0;         //0��Key2δ����         1��Key1�Ѱ���
bit  btBeepFlag;                //���Ʒ��������صı�־λ
uint uiToneNum = 0;             //����
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
                       }; //��ѡ0-f

/*---------��ʼ������--------*/
void Init()
{
    P0M0 = 0xff;
    P0M1 = 0x00;
    P2M0 = 0x08;
    P2M1 = 0x00;
    //����P3^4Ϊ����ģʽ
    P3M1 = 0x00;
    P3M0 = 0x10;

    AUXR |= 0x80;                       //��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;                       //���ö�ʱ��ģʽΪ16λ�Զ���װ
    TL0 = 0xCD;                         //���ö�ʱ��ֵ
    TH0 = 0xF4;                         //���ö�ʱ��ֵ
    TF0 = 0;                            //���TF0��־
    TR0 = 1;                            //��ʱ��0��ʼ��ʱ

    btBeepFlag = 0;
    P0 = 0x00;                          //����ܺ�LED��ʾ����

    sbtSel0 = 1;                         //λѡ����Ϊ����λ
    sbtSel1 = 1;
    sbtSel2 = 1;

    sbtBeep = 0;                        //������������0���Ա���������
    ET0 = 1;
    EA = 1;
}

/*---------��ʱ�Ӻ���--------*/
void DelayMs( uint xms )
{
    uchar i;
    for( ; xms > 0; xms-- )
        for( i = 114; i > 0; i-- )
        {
            ;
        }
}

/*---------��ʾ�Ӻ���--------*/
void DisplaySeg7Led()
{
    P0 = 0;
    sbtSel3 = 0;
    P0 = arrSegSelect[uiToneNum];
    DelayMs( 1 );

    P0 = 0;
    sbtSel3 = 1;
    P0 = 0x08;
    DelayMs( 1 );
}

/*---------������--------*/
void main()
{
    Init();
    while( 1 )
    {
        if( sbtKey1 == 0 )
        {
            if( sbtKey1_state == 0 )           //�жϰ���1�Ƿ���
            {
                DelayMs( 10 );              //��ʱ��������
                if( sbtKey1 == 0 )
                {
                    uiToneNum++;            //�����ı�
                    if( uiToneNum == 10 )
                        uiToneNum = 0;
                    TH0 = 0xF4 - uiToneNum; //��С��װֵ���Ӷ���С
//��ʱ���жϣ��������񶯣�Ƶ��
                    sbtKey1_state = 1;      //���ð���1Ϊ�Ѱ���
                }
            }
        }
        else
            sbtKey1_state = 0;

        if( sbtKey2 == 0 )
        {
            if( sbtKey2_state == 0 )            //�жϰ���2�Ƿ��� 
            {
                DelayMs( 10 );               //��ʱ��������
                if( sbtKey2 == 0 )
                {
                    btBeepFlag = !btBeepFlag;   //�����������л�
                    sbtKey2_state = 1;          //���ð���1Ϊ�Ѱ���
                }
            }
        }
        else
            sbtKey2_state = 0;

        DisplaySeg7Led();
    }
}

/*---------T0��ʱ���жϷ�������--------*/
void T0_Process() interrupt 1
{
    if( btBeepFlag )
    {
        sbtBeep = ~sbtBeep;            //��������ʹ�÷���������
    }
    else
        sbtBeep = 0;                  //������عر�,��������ϵ��Ա���������

}
