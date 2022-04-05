/**********************
myIr  红外通信测试
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uchar unsigned char
/*---------引脚别名定义---------*/
sbit sbtLedSel = P2 ^ 3;            //LED灯与数码管显示切换
sbit sbtKey1 = P3 ^ 2;              //按键1 对应外部中断
sbit sbtGetIr = P3 ^ 6;             //P3.6连接红外接收头
sbit sbtPutIr = P3 ^ 5;             //P3.5连接红外线发光二极管

/*---------变量定义---------*/
uchar ucPutIrFlagN = 1;       //标记位，标记是否发送脉冲。0：发送、   1：不发送

/*--------初始化定时器T0函数---------*/
void InitT0()               //定时器T0初始化
{
    AUXR |= 0x80;                   //1T
    TMOD &= 0xF0;                  //清除之前的设置
    TMOD |= 0x02;                  //设置为8位自动重装
    TL0 = 0x70;                     //定时器初始值
    TH0 = 0x70;                     //自动重装置
    TF0 = 0;                        //清除TF0标记
    TR0 = 1;                        //T0开始运行
}

/*--------延时函数---------*/
void Delay()  //延时100ms
{
    unsigned char i, j, k;
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

/*--------定时器0中断函数---------*/
void T0_Processs() interrupt 1
{
    if( ucPutIrFlagN == 0 )    //如果ucPutIrFlagN=0
        sbtPutIr = ~sbtPutIr; //P3.5根据定时器中断的频率产生翻转（频率设为38kHz）
    else
        sbtPutIr = 0;       //不发射
}

/*--------主函数---------*/
void main()
{
    //设置推挽输出
    P0M0 = 0XFF;
    P0M1 = 0X00;
    P2M0 = 0XFF;
    P2M1 = 0X00;

    P0 = 0;             //初始化P0
    sbtLedSel = 1;       //选择LED发光

    InitT0();
    ET0 = 1;            //打开定时器T0中断
    EA = 1;             //打开总中断

    while( 1 )
    {
        P0 = !sbtGetIr;      //P0显示值
        while( !sbtKey1 )     //如果sbtKey1保存按下
        {
            ucPutIrFlagN = 0;                   //发射红外线脉冲
            Delay();
            ucPutIrFlagN = 1;                   //不发射红外线脉冲
            Delay();
        }
    }
}
