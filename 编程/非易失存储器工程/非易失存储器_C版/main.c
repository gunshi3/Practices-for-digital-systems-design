#include <STC15F2K60S2.h>
#define uint unsigned int
#define uchar unsigned char
#define FOSC 11059200L  //����Ƶ��
#define T_ms 0.1        //��ʱʱ��Ϊ0.1ms
#define NMAX_KEY 10
/*λ����*/
sbit Key1 = P3 ^ 2;     //����key1����洢��д�����ݲ���ȡ�õ�ַ�����ݣ���ʾ��������ϡ�
sbit Key2 = P3 ^ 3;     //����key2��Ҫд������ݼ�1
sbit Key3 = P1 ^ 7;     //����key3��Ҫд��ĵ�ַ��1
sbit led = P2 ^ 3;      //LED����������л�����
sbit SDA = P4 ^ 0;      //I2C���ߵ�������
sbit SCL = P5 ^ 5;      //I2C���ߵ�ʱ����
sbit Beep = P3 ^ 4;
/*��������*/
uchar duan[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //����ܶ�ѡ����ʾ0-f
uchar wei[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //�����λѡ
uchar flag1;                //�����ѭ��ɨ�����
uchar write_addr;           //д���ַ
uchar write_date;           //д������
uchar read_date;            //��������
/*����������������*/
uchar G_count;              //��ʱ��0�жϼ���ֵ
uint Key1_count;            //KEY1����1ms�ڴﵽ�͵�ƽ�Ĵ���
uint Key2_count;            //KEY2����1ms�ڴﵽ�͵�ƽ�Ĵ���
uint Key3_count;            //KEY3����1ms�ڴﵽ�͵�ƽ�Ĵ���
uint Key_count;             //��100�ݼ���0
bit flg_1ms;                //��ʾ1msʱ�䵽
bit Key1_C;/*key1��ǰ��״̬*/
bit Key1_P;/*key1ǰһ��״̬*/
bit Key2_C;/*key2��ǰ��״̬*/
bit Key2_P;/*key2ǰһ��״̬*/
bit Key3_C;/*key3��ǰ��״̬*/
bit Key3_P;/*key3ǰһ��״̬*/

void SMG1( uchar date1, uchar date2, uchar date3 ) //�������ʾ����
{
    flag1++;
    if( flag1 == 8 )
    {
        flag1 = 0;
    }
    P0 = 0x00;
    P2 = wei[flag1];
    switch( flag1 )
    {
        case 0:
            P0 = duan[date1 / 16];   break;
        case 1:
            P0 = duan[date1 % 16];   break;
        case 2:
            P0 = 0x40;   break;
        case 3:
            P0 = duan[date2 / 16];   break;
        case 4:
            P0 = duan[date2 % 16];   break;
        case 5:
            P0 = 0x40;   break;
        case 6:
            P0 = duan[date3 / 16];    break;
        default:
            P0 = duan[date3 % 16];   break;
    }
}

void KEY_init() //��������ģ���ʼ��
{
    G_count = 0;
    flg_1ms = 0;
    Key1_C = 1; /*key1��ǰ��״̬*/
    Key1_P = 1; /*key1ǰһ��״̬*/
    Key2_C = 1; /*key2��ǰ��״̬*/
    Key2_P = 1; /*key2ǰһ��״̬*/
    Key3_C = 1; /*key3��ǰ��״̬*/
    Key3_P = 1; /*key3ǰһ��״̬*/
    Key1_count = 0x80 + NMAX_KEY / 3 * 2;
    Key2_count = 0x80 + NMAX_KEY / 3 * 2;
    Key3_count = 0x80 + NMAX_KEY / 3 * 2;
    Key_count = NMAX_KEY;
}

void delay()        //��ʱ4us
{
    ;;
}

void IIC_init()     //I2C���߳�ʼ��
{
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}
void start()        //���������ź�
{
    SDA = 1;
    delay();
    SCL = 1;
    delay();
    SDA = 0;
    delay();
}
void stop()         //ֹͣ�ź�
{
    SDA = 0;
    delay();
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}
void respons()      //�ӻ�Ӧ���ź�
{
    uchar i = 0;
    SCL = 1;
    delay();
    while( SDA == 1 && ( i < 255 ) ) //��ʾ����һ��ʱ����û���յ���������Ӧ����
        i++;                //������Ĭ�ϴ��ڼ��Ѿ��յ����ݶ����ٵȴ�Ӧ���źš�
    SCL = 0;
    delay();
}
void writebyte( uchar date ) //��24C02дһ���ֽ�����
{
    uchar i, temp;
    temp = date;
    for( i = 0; i < 8; i++ )
    {
        temp = temp << 1;
        SCL = 0;
        delay();
        SDA = CY;
        delay();
        SCL = 1;
        delay();
    }
    SCL = 0;
    delay();
    SDA = 1;
    delay();
}
uchar readbyte()            //��24C02��һ���ֽ�����
{
    uchar i, k;
    SCL = 0;
    delay();
    SDA = 1;
    delay();
    for( i = 0; i < 8; i++ )
    {
        SCL = 1;
        delay();
        k = ( k << 1 ) | SDA;
        delay();
        SCL = 0;
        delay();
    }
    delay();
    return k;
}
void write_add( uchar addr, uchar date ) //��24C02�ĵ�ַaddr��д��һ������date
{
    start();
    writebyte( 0xa0 );
    respons();
    writebyte( addr );
    respons();
    writebyte( date );
    respons();
    stop();
}
uchar read_add( uchar addr )      //��24C02��addr��ַ����һ���ֽ�����
{
    uchar date;
    start();
    writebyte( 0xa0 );
    respons();
    writebyte( addr );
    respons();
    start();
    writebyte( 0xa1 );
    respons();
    date = readbyte();
    stop();
    return date;
}

void IO_init()       //IO�ڳ�ʼ����������ʼ��
{
    P2M1 = 0x00;    //����P0�ں�P2^3�������
    P2M0 = 0x08;
    P0M1 = 0x00;
    P0M0 = 0xff;    //������������
    led = 0;        //��LED��ʾ
    P0 = 0x00;
	Beep=0;            //ȷ����������
    write_addr = 0x00;      //д���ַ��ʼ��
    write_date = 0x00;      //д�����ݳ�ʼ��
}

void Timer0_Init()       //��ʱ��0��ʼ��
{
    TMOD = 0x00;        //��ʱ��0������ʽ0��16λ�Զ���װ����
    AUXR = 0x80;        //1Tģʽ,T0x12=1,
    EA = 1;             //�����ж�
    ET0 = 1;            //����ʱ��0�ж�
    TH0 = ( 65536 - T_ms * FOSC / 1000 ) / 256;  //����ʱ������ֵ
    TL0 = ( 65536 - T_ms * FOSC / 1000 );  
    TR0 = 1;                              //������ʱ��
}

void Timer_T0() interrupt 1  //��ʱ��0�жϺ���
{
    G_count++;
    if( G_count == 10 )
    {
        flg_1ms = 1;       //1msʱ�䵽��fla_1ms=1
        G_count = 0x00;
    }
    SMG1( write_addr, write_date, read_date ); //�ڶ�ʱ���ж��е����������ʾ����
}

int main()                 //������
{
    IO_init();            //IO�ڳ�ʼ��
    Timer0_Init();        //��ʱ��0��ʼ��
    KEY_init();           //��������ģ���ʼ��
    IIC_init();            //IIC���߳�ʼ��
    while( 1 )
    {
        while( flg_1ms )  //1msʱ�䵽 �жϰ���״̬
        {
            flg_1ms = 0;
            read_date = read_add( write_addr ); //������ַΪwrite_addr������
            if( Key1 == 0 )
                Key1_count--;
            if( Key2 == 0 )
                Key2_count--;
            if( Key3 == 0 )           //�����ǰ���״̬
                Key3_count--;
            Key_count--;           //�ܵĴ�����1
            if( Key_count == 0 )      //10������
            {
                if( Key1_count < 0x80 )
                {
                    Key1_C = 0;
                    if( Key1_P == 1 )       //�½��أ�������������
                    {
                        Key1_P = 0;
                        write_add( write_addr, write_date ); //���ַwrite_addrд������
                    }
                }
                if( Key1_count >= 0x80 )
                {
                    Key1_C = 1;
                    if( Key1_P == 0 )
                        Key1_P = 1;          //�����أ����費�������Ǿͼ�����
                }
                if( Key2_count < 0x80 )
                {
                    Key2_C = 0;
                    if( Key2_P == 1 )         //�½��أ�������������
                    {
                        Key2_P = 0;
                        write_date++;           //�������ݼ�1
                        if( write_date == 0xff ) //������������ݴ���0xff�����Ϊ0x00��
                            write_date = 0x00;
                    }
                }
                if( Key2_count >= 0x80 )
                {
                    Key2_C = 1;
                    if( Key2_P == 0 )
                        Key2_P = 1;  //�����أ����費�������Ǿͼ�����
                }
                if( Key3_count < 0x80 )
                {
                    Key3_C = 0;
                    if( Key3_P == 1 ) //�½��أ�������������
                    {
                        Key3_P = 0;
                        write_addr++;           //����д���ַ+1
                        if( write_addr == 0xff )
                            write_addr = 0;
                    }
                }
                if( Key3_count >= 0x80 )
                {
                    Key3_C = 1;
                    if( Key3_P == 0 )
                        Key3_P = 1;  //�����أ����費�������Ǿͼ�����
                }
                Key1_count = 0x80 + NMAX_KEY / 3 * 2; //��һ�ֵ��ж�
                Key2_count = 0x80 + NMAX_KEY / 3 * 2;
                Key3_count = 0x80 + NMAX_KEY / 3 * 2;
                Key_count = NMAX_KEY;
            }
        }//read_date=read_add(read_addr); �����ݺ������ڴ˴�����ܻ��޷���ʱ��ʾ
    }
} 
