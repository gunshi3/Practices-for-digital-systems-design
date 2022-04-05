/**********************
myUsbUart 串口测试
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include<STC15F2K60S2.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int

/*---------宏定义---------*/
#define cstFocs      11059200L  //晶振频率
#define cstBaud1     9600       //波特率
#define cstKeyMaxNum 100        //按键抖动次数

/*---------引脚别名定义---------*/
sbit sbtKey1 = P3 ^ 2 ;             //启动发送
sbit sbtKey2 = P3 ^ 3 ;             //数字减少
sbit sbtKey3 = P1 ^ 7 ;             //数字增加
sbit sbtLedSel = P2 ^ 3;

/*---------变量定义---------*/
uchar ucT100usTimes;

uint uiKey1Cnt;            //按键1计数
uint uiKey2Cnt;            //按键2计数
uint uiKey3Cnt;            //按键3计数
uint uiKeyAllCnt;          //按键总的抖动次数
bit btT1msFlag;            //1ms的标志
bit btKey1Current;/*key1当前的状态*/
bit btKey1Past;   /*key1前一个状态*/
bit btKey2Current;/*key2当前的状态*/
bit btKey2Past;   /*key2前一个状态*/
bit btKey3Current;/*key3当前的状态*/
bit btKey3Past;   /*key3前一个状态*/

/*收发显示数据相关*/
bit btUart1SendBusy = 0 ;
uchar ucDateTmp;               //传输数据暂存
uchar ucDateDigState;
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //显示0-f

/*---------定时器T0中断处理函数--------*/
void T0_Process() interrupt 1
{
    TH0 = ( 65535 - 1000 ) / 256; //定时器初始值
    TL0 = ( 65535 - 1000 ) % 256;
    ucT100usTimes++;
    if( ucT100usTimes == 10 )        //中断10次对应1ms
    {
        ucT100usTimes = 0;
        btT1msFlag = 1;
    }
    ucDateDigState++;
    if( ucDateDigState == 2 )
        ucDateDigState = 0;
    P0 = 0;
    switch( ucDateDigState )
    {
        case 0:
            P2 = 0x00;  P0 = arrSegSelect[ucDateTmp / 16];  
break;
        case 1:
            P2 = 0x01;  P0 = arrSegSelect[ucDateTmp % 16];  break;
    }
}

/*---------串口1初始化函数--------*/
void Uart1_Init( void )
{
    AUXR = 0X80; //辅助寄存器 此时定时器0的速度是传统的12倍，不分频
    SCON |= 0X50; //允许接收
    TL1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) );
    TH1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) ) >> 8;
    AUXR |= 0X40; //辅助寄存器 此时定时器1的速度是传统的12倍，不分频
    RI = 0;      //接收中断标志位
    TI = 0;      //发送中断标志位
    TR1 = 1;     //启动定时器1
    ES = 1;      //串口中断允许位
    EA = 1;      //总中断允许位
    PS = 1 ;    //串口1中断高优先级
}

/*---------发送数据函数--------*/
void SendData( unsigned char dat )
{
    while( btUart1SendBusy ); //发送单个字符给UART1以发送到PC机
    btUart1SendBusy = 1;
    SBUF = dat;
}

/*---------串口1中断处理函数--------*/
void Uart1_Process() interrupt 4 using 1
{
    if( RI )         //接受完数据后 RI自动制1
    {
        RI = 0;   ucDateTmp = SBUF;
    }
    if( TI )        //发送完数据后 RI自动制1
    {
        TI = 0;   btUart1SendBusy = 0;
    }
}

/*---------初始化函数--------*/
void Init()
{
    P3M0 = 0x00;
    P3M1 = 0x00;
    P2M0 = 0xff;
    P2M1 = 0x00;
    P0M0 = 0xff;
    P0M1 = 0x00;

    TMOD = 0x01; //定时器0，方式1
    ET0 = 1; //开启定时器中断
    TH0 = ( 65535 - 1000 ) / 256;
    TL0 = ( 65535 - 1000 ) % 256;
    TR0 = 1; //启动定时器

    Uart1_Init();   //外部中断：低优先级

    ucDateTmp = 0x00;
    sbtLedSel = 0;
    btT1msFlag = 0;

    /*初始化所有按键的当前状态、前一个状态*/
    btKey1Current = 1; /*key1当前的状态*/
    btKey1Past = 1; /*key1前一个状态*/
    btKey2Current = 1; /*key2当前的状态*/
    btKey2Past = 1; /*key2前一个状态*/
    btKey3Current = 1; /*key3当前的状态*/
    btKey3Past = 1; /*key3前一个状态*/

    uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKeyAllCnt = cstKeyMaxNum;
}

/*---------主函数--------*/
void main()
{
    Init();
    while( 1 )
    {
        if( btT1msFlag )
        {
            btT1msFlag = 0;
            if( sbtKey1 == 0 )
                uiKey1Cnt--;
            if( sbtKey2 == 0 )
                uiKey2Cnt--;
            if( sbtKey3 == 0 ) //按键是按下状态
                uiKey3Cnt--;
            uiKeyAllCnt--;//总的次数减1

            if( uiKeyAllCnt == 0 ) //100次完了
            {

                if( uiKey1Cnt < 0x80 )
                {
                    btKey1Current = 0;
                    if( btKey1Past == 1 ) //下降沿（按键做动作）
                    {
                        btKey1Past = 0;
                        SendData( ucDateTmp ) ;
                    }
                }
                if( uiKey1Cnt >= 0x80 )
                {
                    btKey1Current = 1;
                    if( btKey1Past == 0 )
                        btKey1Past = 1;  //上升沿（假设不做动作那就继续）
                }
                if( uiKey2Cnt < 0x80 )
                {
                    btKey2Current = 0;
                    if( btKey2Past == 1 ) //下降沿（按键做动作）
                    {
                        btKey2Past = 0;
                        ucDateTmp--;
                    }
                }
                if( uiKey2Cnt >= 0x80 )
                {
                    btKey2Current = 1;
                    if( btKey2Past == 0 )
                        btKey2Past = 1;  //上升沿（假设不做动作那就继续）
                }

                if( uiKey3Cnt < 0x80 )
                {
                    btKey3Current = 0;
                    if( btKey3Past == 1 ) //下降沿（按键做动作）
                    {
                        btKey3Past = 0;
                        ucDateTmp++;
                    }
                }
                if( uiKey3Cnt >= 0x80 )
                {
                    btKey3Current = 1;
                    if( btKey3Past == 0 )
                        btKey3Past = 1;  //上升沿（假设不做动作那就继续）
                }

                /*新一轮的判断*/
                uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
                uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
                uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
                uiKeyAllCnt = cstKeyMaxNum;
            }
        }
    }
}
