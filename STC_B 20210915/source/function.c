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
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
	}
	if (GetAdcNavAct(enumAdcNavKeyDown) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 2;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
	}
	if (GetAdcNavAct(enumAdcNavKeyLeft) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 3;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
	}
	if (GetAdcNavAct(enumAdcNavKeyRight) == enumKeyPress)
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 4;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
	}
	if (GetAdcNavAct(enumAdcNavKeyCenter) == enumKeyPress) //���˵���һ��
	{
		SetBeep(1000, 20);
		*(rxd + 0) = 5;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
	}
	if (GetAdcNavAct(enumAdcNavKey3) == enumKeyPress) //ʾ��������3:(����ʱ���������ֲ���"��ͣ/����"
	{
		SetMusic(Music_PM, Music_tone, &song, sizeof(song), enumMscDrvSeg7andLed); //�������Žڴպ�����
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
	if (GetKeyAct(enumKey1) == enumKeyPress) //ʾ��������1:������ʱ����ǰ�ؿ����¿�ʼ
	{
		*(rxd + 0) = 6;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
		SetBeep(1000, 20);
	}
	if (GetKeyAct(enumKey2) == enumKeyPress) //ʾ��������2:������ʱ����ʼ/��ͣ��Ϸ
	{
		*(rxd + 0) = 7;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
		SetBeep(1000, 20);
	}
}