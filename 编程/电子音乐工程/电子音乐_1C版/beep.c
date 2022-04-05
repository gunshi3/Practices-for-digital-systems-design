/**********************
myMusic 音乐播放
型号:IAP15F2K60S2 主频:12MHz
************************/
#include <STC15F2K60S2.h>
#define uint unsigned int
#define uchar unsigned char

/*---------引脚别名定义---------*/
sbit sbtBeep = P3 ^ 4;    //蜂鸣器

/*---------变量定义---------*/
uchar ucTimerH, ucTimerL;   //定义定时器的重装值
uchar code arrMusic[] =     //音乐代码，歌曲为《同一首歌》，格式为: 音符, 节拍
{
//音符的十位代表是低中高八度，1代表高八度，2代表中八度，3代表高八度
//个位代表简谱的音符，例如0x15代表低八度的S0，0x21代表中八度的DO。
//节拍则是代表音长，例如：0x10代表一拍，0x20代表两拍，0x08代表1/2拍
0x15,0x20,0x21,0x10,	 //音符的十位代表是低八度，中八度还是高八度，1代表低八度，2代表中八度，3代表高八度
0x22,0x10,0x23,0x18,	 //个位代表简谱的音符，例如0x15代表低八度的S0，0x21代表中八度的DO。
0x24,0x08,0x23,0x10,	 //节拍则是代表音长，例如：0x10代表一拍，0x20代表两拍，0x08代表1/2拍
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
    //此数组数据为各个音符在定时器中的重装值，第一列是高位，第二列是低位
    0xf8, 0x8c,   //低八度，低1
    0xf9, 0x5b,
    0xfa, 0x15,   //低3
    0xfa, 0x67,
    0xfb, 0x04,   //低5
    0xfb, 0x90,
    0xfc, 0x0c,   //低7
    0xfc, 0x44,   //中央C调
    0xfc, 0xac,   //中2
    0xfd, 0x09,
    0xfd, 0x34,   //中4
    0xfd, 0x82,
    0xfd, 0xc8,   //中6
    0xfe, 0x06,
    0xfe, 0x22,   //高八度，高1
    0xfe, 0x56,
    0xfe, 0x6e,   //高3
    0xfe, 0x9a,
    0xfe, 0xc1,   //高5
    0xfe, 0xe4,
    0xff, 0x03    //高7
};

/*---------延时子函数---------*/
void DelayMs( unsigned int xms )
{
    uint i, j;
    for( i = xms; i > 0; i-- )
        for( j = 124; j > 0; j-- );
}

/*---------取址子函数---------*/
//取出tem音符在arrMusicToTimerNum数组中的位置值
uchar GetPosition( uchar tem ) 
{
    uchar ucBase, ucOffset, ucPosition;     //定义曲调，音符和位置
    ucBase = tem / 16;            //高4位是曲调值,基址
    ucOffset = tem % 16;          //低4位是音符，偏移量
    if( ucBase == 1 )              //当曲调值为1时，即是低八度，基址为0
        ucBase = 0;
    else if( ucBase == 2 )          //当曲调值为2时，即是中八度，基址为14
        ucBase = 14;
    else if( ucBase == 3 )          //当曲调值为3时，即是高八度，基址为28
        ucBase = 28;
    //通过基址加上偏移量，即可定位此音符在arrMusicToTimerNum数组中的位置
ucPosition = ucBase + ( ucOffset - 1 ) * 2; 
    return ucPosition;            //返回这一个位置值
}

/*---------播放音乐功能函数---------*/
void PlayMusic()
{
    uchar ucNoteTmp, ucRhythmTmp, tem; // ucNoteTmp为音符，ucRhythmTmp为节拍
    uchar i = 0;
    while( 1 )
    {
        ucNoteTmp = arrMusic[i];    //如果碰到结束符,延时1秒,回到开始再来一遍
        if( ucNoteTmp == 0x00 )
        {
            i = 0;
            DelayMs( 1000 );
        }
        else if( ucNoteTmp == 0xff )  //若碰到休止符,延时100ms,继续取下一音符
        {
            i = i + 2;
            DelayMs( 100 );
            TR0 = 0;
        }
        else                     //正常情况下取音符和节拍
        {
            //取出当前音符在arrMusicToTimerNum数组中的位置值
tem = GetPosition( arrMusic[i] );              
//把音符相应的计时器重装载值赋予ucTimerH和ucTimerL
ucTimerH = arrMusicToTimerNum[tem];  
            ucTimerL = arrMusicToTimerNum[tem + 1];
            i++;
            TH0 = ucTimerH;           //把ucTimerH和ucTimerL赋予计时器
            TL0 = ucTimerL;
            ucRhythmTmp = arrMusic[i];      //取得节拍
            i++;
        }
        TR0 = 1;                          //开定时器1
        DelayMs( ucRhythmTmp * 180 );      //等待节拍完成, 通过P3^4口输出音频
        TR0 = 0;                          //关定时器1

    }
}

/*---------初始化子函数---------*/
//功能是配置IO口
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

/*---------定时器0初始化子函数---------*/
void InitT0()
{
    TMOD = 0x01;
    TH0 = 0xD8;
    TL0 = 0xEF;
    EA = 1;
    ET0 = 1;
    TR0 = 0;
}

/*---------主函数---------*/
void main()
{
    InitSys();
    InitT0();
    P0 = 0x00;
    PlayMusic();
    while( 1 );
}

/*---------定时器0中断处理函数---------*/
//重新装值，并把sbtBeep值取反，产生方波
void T0_Process() interrupt 1                     //计时器控制频率
{
    TH0 = ucTimerH;
    TL0 = ucTimerL;
    sbtBeep = ~sbtBeep;
}
