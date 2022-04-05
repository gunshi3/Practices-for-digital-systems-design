#include <intrins.h>
void Delay1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay300ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 13;
	j = 156;
	k = 83;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void dealwithDisp()
{
	Seg7Print(34, 20, 31, 35, 22, 28, 28, 19);
}

void dealwithmyKN()
{
	if (GetAdcNavAct(enumAdcNavKeyUp) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 1;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
	}
	if (GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 2;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
	}
	if (GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 3;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
	}
	if (GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 4;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
	}
	if (GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress) //回退到上一步
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 5;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
	}
	if (GetAdcNavAct(enumAdcNavKey3) == enumKeyPress) //示例。按键3:(按下时）控制音乐播放"暂停/继续"
	{
		SetMusic(Music_PM, Music_tone, &song, sizeof(song), enumMscDrvSeg7andLed); //调整播放节凑和音调
		if (GetPlayerMode() == enumModePlay)
		{
			SetPlayerMode(enumModePause);
		}
		else
			SetPlayerMode(enumModePlay);
	}
}

void dealwithmykey()
{
	if (GetKeyAct(enumKey1) == enumKeyPress) //示例。按键1:（按下时）当前关卡重新开始
	{
		*(rxd + 0) = 6;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
		SetBeep(1000, 20);
	}
	if (GetKeyAct(enumKey2) == enumKeyPress) //示例。按键2:（按下时）开始/暂停游戏
	{
		*(rxd + 0) = 7;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
		SetBeep(1000, 20);
	}
}