/*-----------------------------------------------------------------------------
   * ʵ �� ��    : WideMouth 51��Ƭ���ַ����շ�
   * ʵ��˵��  : ��Ƭ�������շ����ݿ��Ƶ�����
   * ʵ��ģ��  : STC89C52RC��Ƭ����Сϵͳ��HC-06����ģ��
   * ���ӷ�ʽ  ������ģ��RXD��TXD�˷ֱ����ӵ�Ƭ��TXD��RXD�ˣ��������������ӵ�Ƭ��������
   * ע    ��  : ��Ƭ������ʹ�õ���11.0592
  *******************************************************************************/
#include <STC15F2K60S2.h>
// #include <reg52.h>
#include <string.h> //�ַ�������������
// #include <intrins.h>
#define uchar unsigned char // �궨��  51��Ƭ���ڴ���Դ����
#define uint unsigned int   //�迼���ڴ���Դ���⣬��ʹ���޷�������
uchar receive_data[8];
uint bytes = 0;
uint bytes_old;
//--����ʹ�õ�IO--//
sbit Led301 = P2 ^ 0;
sbit Led302 = P2 ^ 3;
//--����ȫ�ֺ���--//
void uart_init();
void Delay_1ms(uint i);
void SendChar(uchar character);
void SendString(uchar *p);
/********************************************************************
 * ���� : Com_Int()
 * ���� : �����жϺ��������ܺͷ�������ʱ������ش˺�����
***********************************************************************/
void Com_Int(void) interrupt
{
    EA = 0;
    if (RI == 1) //�˾��жϲ���ʡ�ԣ�����������ʱҲ��ִ�д���䣬����ַ������󣡣�����RIΪ�������ݱ�־�������յ������Զ���Ϊ1��
    {
        RI = 0;
        receive_data[bytes] = SBUF; //���յ�������
        bytes++;
    }
    EA = 1;
}
/*******************************************************************************
  * �� �� ��      : main
  * ��������      : ��������������ڣ�
*******************************************************************************/
void main()
{
    bytes_old = 0;
    uart_init(); //��ʼ������
    while (1)
    {
        while (1)
        {
            bytes_old = bytes;
            Delay_1ms(30);          //�����Ƭ��������������ģ�鲻����1�ֽڵĴ������ݣ�һ��Ҫ����ʱ�����ж��Ƿ���ʱʱ���Լ��ɰ��������
                                    //���������ж�һ����������30ms��
            if (bytes_old == bytes) //�������30ms�������ݽ��գ�˵���ַ����ѽ�����ϣ��������������Ľ���һ֡��������
            {
                receive_data[bytes] = 0; //�ַ���ĩβ�ӽ�β��־0����Ӧ/0��
                if (bytes)
                    break; //����ѭ����ִ����Ӧ�߼�����
            }
        }
        SendString(receive_data);              //�����յ����������ݷ��ظ����Ͷ�
        if (strcmp(receive_data, "3011") == 0) //strcmp(str1,str2)�ַ����ȽϺ���.���� str1 �� str2 �ǲ���Ƚϵ������ַ�����
        {                                      //��str1=str2���򷵻��㣻
            Led301 = 0;                        //��str1<str2���򷵻ظ�����
        }                                      //��str1>str2���򷵻�������
        if (strcmp(receive_data, "3010") == 0)
        {
            Led301 = 1;
        }
        if (strcmp(receive_data, "3021") == 0)
        {
            Led302 = 0;
        }
        if (strcmp(receive_data, "3020") == 0)
        {
            Led302 = 1;
        }
        bytes_old = 0; //��bytes_old��bytes��Ϊ��ʼ��ֵ���ȴ�������һ�ַ���
        bytes = 0;
    }
}
/*******************************************************************************
 * �� �� ��      : uart_init()
 * ��������     : ��ʼ�����ô���
 * ��ע         ����ֱ�Ӹ���
*******************************************************************************/
void uart_init() //ֱ�Ӹ��Ƽ���
{
    SCON = 0X50; //����Ϊ������ʽ1,8λ���ݣ��ɱ䲨����
    TMOD = 0X20; //���ü�����������ʽ2
    PCON = 0X00; //�����ʲ��ӱ�
    TH1 = 0XFd;  //��������ʼֵ���ã�9600@11.0592MHz
    TL1 = 0XFd;
    TR1 = 1; //�򿪼�����
    ES = 1;  //�������ж�
    EA = 1;  //�����ж�
}
/********************************************************************
 * ���� : Delay_1ms()
 * ���� : ��ʱ�ӳ�����ʱʱ��Ϊ 1ms
 * x * ���� : x (��λΪ����)
***********************************************************************/
void Delay_1ms(uint i) //1ms��ʱ��ֱ�Ӹ��Ƽ���
{
    uint x, j;
    for (j = 0; j < i; j++)
        for (x = 0; x <= 148; x++);
}
/********************************************************************
 * ���� : SendChar(uchar character)
 * ���� �������ַ�
 * ���� : ���͵����ַ�
***********************************************************************/
void SendChar(uchar character)
{
    SBUF = character; //���͵����ַ�
    while (!TI);
    TI = 0; //������ɱ�־
}
/********************************************************************
 * ���� : SendString(uchar String[]) 
 * ���� : �����ַ���
 * ���� ���ַ���
***********************************************************************/
void SendString(uchar String[])
{
    uchar *p = String;
    while (*p) //��ָ��ָ��ĵ�ַΪ�գ�������ѭ��
    {
        SendChar(*p); //ָ���һ��Ĭ��ָ���׵�ַ
        Delay_1ms(2); //��ʱ������Ϊ��߷���׼ȷ��
        p++;
    }
}