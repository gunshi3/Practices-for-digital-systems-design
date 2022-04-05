/**********************
mysbtHall 磁场检测
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------引脚别名定义---------*/
sbit sbtLedSel = P2 ^ 3;   //发光二极管和数码管选择
sbit sbtHall = P1 ^ 2;     //霍尔引脚别名定义

/*---------变量定义---------*/
uchar ucLed;           //用于接收霍尔的值

/*---------初始化函数---------*/
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

/*---------主函数---------*/
void main()
{
    Init();
    while( 1 )
    {
        ucLed = sbtHall;
        P0 = ucLed;
    }
}
