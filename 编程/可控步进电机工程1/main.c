/**********************
myXstep2 可控步进电机(正反转暂停)
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.h>
#define uint unsigned int

/*---------宏定义---------*/
#define cstKeyMaxNum 100                     //100次读取按键值

uint code arrSegSelect[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66,
                             0x6d, 0x7d, 0x07, 0x7f, 0x6f,
                             0x77, 0x7c, 0x39, 0x5e, 0x79,
                             0x71, 0x40, 0x00
                           };   //数码管显示0~4的16进制代码

/*---------引脚别名定义---------*/
sbit sbtKey1 = P3 ^ 2;                                               //按键1
sbit sbtKey2 = P3 ^ 3;                                               //按键2
sbit sbtKey3 = P1 ^ 7;                                               //按键3
sbit sbtS1  = P4 ^ 1;
sbit sbtS2  = P4 ^ 2;
sbit sbtS3  = P4 ^ 3;
sbit sbtS4  = P4 ^ 4;

/*---------变量定义---------*/
uint uiKey1Cnt;
uint uiKey2Cnt;
uint uiKey3Cnt;
uint uiKeyAllCnt;

uint uiSpeed = 1;                                    //控制控制步进电机的速度

uint i = 1;                                          //控制脉冲对应的IO口位置
bit  btKey1Current = 1;                              //Key1当前的状态
bit  btKey1Past = 1;                                 //Key1前一个状态
bit  btKey2Current = 1;                              //Key2当前的状态
bit  btKey2Past = 1;                                 //Key2前一个状态
bit  btKey3Current = 1;                               //Key3当前的状态
bit  btKey3Past = 1;                                  //Key3前一个状态
bit  btRotationFlag = 0;                               //标记步进电机转向


/*---------初始化函数--------*/
void InitSys()           //功能是配置IO口,启动定时器0和1
{
    P0M0 = 0xff;                                   //P0口设置推挽输出
    P0M1 = 0x00;

    P4M0 = 0x3f;                                    //P4口设置推挽输出
    P4M1 = 0xe1;
    P4 = 0xe1;      //设置P4口为低电平，避免复位时步进电机5V电平接口的影响

    P2M0 = 0x07;                                  //P2.3口设置推挽输出
    P2M1 = 0x00;
    P2 = 0X07;                                     //设置数码管显示

    uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKeyAllCnt = cstKeyMaxNum;

    AUXR =  0X00;                   //定时器0和定时器1 时钟1T模式
    TMOD =  0X00;                  //设置定时器0和定时器1 16位自动重装模式
    TL0  = ( 65536 - 250 ) % 256;       //设置定时0定时为 250μs
    TH0  = ( 65536 - 250 ) / 256;
    TH1  = ( 65536 - 5000 * uiSpeed ) / 256;       //设置定时1定时为 5ms*uiSpeed
    TL1  = ( 65536 - 5000 * uiSpeed ) % 256;
    TCON = 0X50;                        //定时器0和1开始计时
    IE   = 0x8A;                         //定时器0和1开启中断，CPU开启中断
}

/*---------T1定时器中断服务处理函数--------*/
void T1_Process() interrupt 3                //控制脉冲频率
{
    TH1  = ( 65536 - 5000 * uiSpeed ) / 256;  //通过在定时器1的时间来改变脉冲频率
    TL1  = ( 65536 - 5000 * uiSpeed ) % 256;
    if( btRotationFlag == 0 )                //控制步进电机正转
    {
        switch( i++ )                     //循环改变不同IO脚的脉冲电平
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
    else                                 //控制步进电机反转
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
/*---------T0定时器中断服务处理函数--------*/
void T0_Process() interrupt 1  //按键消抖+中断处理（控制步进电机转向和速度）
{
    if( sbtKey1 == 0 )
        uiKey1Cnt--;
    if( sbtKey2 == 0 )
        uiKey2Cnt--;
    if( sbtKey3 == 0 )
        uiKey3Cnt--;                                  //按键是按下状态
    uiKeyAllCnt--;                                     //总的次数减1

    if( uiKeyAllCnt == 0 )                                //100次完了
    {
        if( uiKey2Cnt < 0x80 )
        {
            btKey2Current = 0;
            if( btKey2Past == 1 )       //下降沿（按键则步进电机的转速改变，//uiSpeed值越大，转速越慢）
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
                btKey2Past = 1;          //上升沿（假设不做动作那就继续）
        }

        if( uiKey1Cnt < 0x80 )
        {
            btKey1Current = 0;
            if( btKey1Past == 1 )          //下降沿（按键改变步进电机转向）
            {
                btKey1Past = 0;
                btRotationFlag = ~btRotationFlag;
            }

        }
        if( uiKey1Cnt >= 0x80 )
        {
            btKey1Current = 1;
            if( btKey1Past == 0 )
                btKey1Past = 1;         //上升沿（假设不做动作那就继续）
        }

        if( uiKey3Cnt < 0x80 )
        {
            btKey3Current = 0;
            if( btKey3Past == 1 )          //下降沿（按键改变步进电机转向）
            {
                btKey3Past = 0;
                TR1 = ~TR1;
            }

        }
        if( uiKey3Cnt >= 0x80 )
        {
            btKey3Current = 1;
            if( btKey3Past == 0 )
                btKey3Past = 1;              //上升沿（假设不做动作那就继续）
        }

        uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;          //新一轮的判断
        uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
        uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
        uiKeyAllCnt = cstKeyMaxNum;
    }
}

/*---------主函数---------*/
void main()
{
    InitSys();
    while( 1 )
    {
        P0 = arrSegSelect[uiSpeed]; // P0口显示步进电机速度，范围1-4,1档速度最快
    }
} 
