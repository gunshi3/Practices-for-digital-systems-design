#include <STC15F2K60S2.h>
#define uint unsigned int
#define uchar unsigned char

uint i=0;
uchar duanxuan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f}; //��ʾ0-8
uchar weixuan[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};			 //�����0-7

void Delay(int n)				//��ʱ����
{
	while(n--);
}

void main()
{
	P2M0=0xff;					//�����������
   	P2M1=0x00;
    P0M0=0xff;
    P0M1=0x00;

	while(1)
	{
		for(i=0;i<8;i++)
		{
			P0=0;
			P2=weixuan[i];		//ѡ������ܵ�λ��
			P0=duanxuan[i+1];	//��ʾ��Ӧ����ֵ
			Delay(600);
		}
	}
}