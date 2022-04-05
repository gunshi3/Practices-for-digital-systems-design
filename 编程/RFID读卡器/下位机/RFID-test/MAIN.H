#ifndef _MAIN_H
#define _MAIN_H

//uchar code data1[16] ={0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反 
//uchar code money[4]      = {1,0,0,0};
#define uchar unsigned char
#define uint unsigned int
#define Machine_Focs 11059200L	//晶振频率
#define BAUD1        9600				//波特率
//*********全局变量定义*********************************//

extern unsigned char Rec_Buffer[REC_BUFFER_SIZE];		//串口接收缓冲区
extern unsigned char Recvd_Count;					    //串口已接收数据的个数
extern bit Uart1_Sendbusy;
extern unsigned char g_ucTempbuf1[4];  					//返回卡号	
extern unsigned char g_ucTempbuf2[16]; 					//返回块中16位数据                     
extern unsigned char flag;
extern bit  time_flag;//1秒时间标志位
extern bit  mode     ;//模式选择标志位，0为定时计费模式，1为流量计费模式
extern unsigned char gather_money;//汇总金额变量，需要充分考虑停电问题？？？？？？？？？？？？？？？？？？？？

extern char PcdReset_flag; 							    //初始化标志位声明
extern char PcdRequest_flag;           					//询卡标志位
extern char PcdAntennaOn_flag; 							//打开天线标志位声明
extern char PcdAntennaOff_flag; 						//关闭天线标志位声明
extern char write_data_flag; 							//写数据标志位声明
extern char read_date_flag; 							//读数据标志位声明
extern char flag1;

void Init();
void Sent_Buffer(uchar *string,uchar len);
void Sent_String(unsigned char *pt_send);
void Sent_Byte(unsigned char Sdata);

#endif