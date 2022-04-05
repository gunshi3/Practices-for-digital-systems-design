//******* �û������1���û���������ļ� *************//
#include "main.H" //���롣��дӦ�ó���ʱ�������д main.h �� main.c�ļ�
#include "song.c" //������song.c�б�д�����֣�ͬһ�׸裩����
//******* �û������2���û��Զ��庯������ *************//

//******* �û������3���û�����ȫ�ֱ������� *************//
unsigned char Music_tone, Music_PM; //������ ���ֲ����������ڴգ�ÿ���ӽ�������
unsigned int count;
unsigned char rxd[1] = {0x01};
unsigned char rtx[2];
unsigned char rxdhead[1] = {0xaa};

//******* �û������4���û��Զ��庯��ԭ�� *************//
#include "function.c"

void my1S_callback() //������1S�¼��ص�����
{
	count++;
	if (count == 60)
	{
		count = 0;
		if(GetPlayerMode() == enumModePlay)
			SetPlayerMode(enumModePause);
	 	SetDisplayerArea(0, 7);
		Seg7Print(23, 16, 34, 20, 31, 20, 5, 32);
		*(rxd + 0) = 8;
		Uart1Print(&rxd, sizeof(rxd)); //�Ӵ���1���ͳ�ȥ
		SetBeep(1000, 20);   
		Delay300ms();
		Seg7Print(10, 10, 10, 23, 20, 26, 26, 0);
	}
}

void myKN_callback() //���������������¼��ص�����
{
	dealwithmyKN();
}

void mykey_callback() // ������Key1��Key2���¼��ص�����
{
	dealwithmykey();
}

void myADC_callback() //������ADC�¼��ص�����
{
}

void mySV_callback() //ʾ��: ���¼��ص��������������ֲ���/��ͣ
{
	if (GetVibAct())
		if (GetPlayerMode() == enumModePause)
			SetPlayerMode(enumModePlay);
		else
			SetPlayerMode(enumModePause);
}

void myUart1_callback()
{
	SetBeep(1000, 30);
	if(GetPlayerMode() == enumModePlay)
		SetPlayerMode(enumModePause);
	Seg7Print(34, 20, 31, 35, 22, 28, 28, 19);
	Delay1000ms();
	if(GetPlayerMode() == enumModePause)
		SetPlayerMode(enumModePlay);
}

//******* main()���� *************//
void main()
{ //������ main() ��ʼ         //���б��룡����

	//******* �û������5���û�main()�����ڲ��ֲ��������� *************//

	//******* �û������6���û�main()��������ʼ������� *************//
	//1,������Ҫ�õ�ģ��(�ɸ�ģ���ṩ���غ���)
	Key_Init();			//��������Ҫ�õ���ģ�鼰�亯������������������ʼ��������׼������������Ӧ����̨����
	VibInit();			//����
	DisplayerInit();	//����
	BeepInit();			//����
	MusicPlayerInit();	//����
	AdcInit(ADCexpEXT); //������ADCģ���ʼ�����в�����ѡ����չ�ӿ�EXT��P1.0��P1.1�Ƿ�Ҳ��ADC����
	Uart1Init(1200);	//����������1��ʼ�����в���������Uart1ͨ�Ų�����

	//2,�����¼��ص�����(��sys�ṩ���ú���SetEventCallBack())
	SetEventCallBack(enumEventKey, mykey_callback);	 //����
	SetEventCallBack(enumEventSys1S, my1S_callback); //����
	SetEventCallBack(enumEventNav, myKN_callback);	 //���������õ��������ص�����
	SetEventCallBack(enumEventVib, mySV_callback);
	SetEventCallBack(enumEventXADC, myADC_callback); //��չ�ӿ����µ�ADֵ�¼�
	SetEventCallBack(enumEventUart1Rxd, myUart1_callback);

	//3,�û�����״̬��ʼ��
	SetDisplayerArea(0, 7);
	Seg7Print(10, 10, 10, 23, 20, 26, 26, 0);
	SetUart1Rxd(&rtx, sizeof(rtx), rxdhead, sizeof(rxdhead));
	//���ô��ڽ��շ�ʽ�����ݰ��������������ݰ�������rxd�У����ݰ���Сrxd��С�����ݰ�ͷ��Ҫ��rxdheadƥ�䣬ƥ������rxdhead��С

	//4,�û����������ʼ��
	Music_PM = 90;
	Music_tone = 0xFC;

	/*****************  MySTC_OS ��ʼ������ؿ�ʼ  ********************************/
	MySTC_Init(); // MySTC_OS ��ʼ��         //���б��룡����
	while (1)	  // ϵͳ��ѭ��              //���б��룡����
	{
		MySTC_OS(); // MySTC_OS ����           //���б��룡����
		/*****************  MySTC_OS ��ʼ������ؽ���  ********************************/

		//******* �û������7���û�main()��������ѭ������ *************//

	} //��ѭ��while(1)����         //���б��룡����
} //������ main() ����         //���б��룡����
