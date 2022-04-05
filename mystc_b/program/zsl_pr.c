/*
	???????????
		ZSL 201808010205
	 ??????????????????24C02(????????)
	 ??——????????A/D???????? 
*/ 
#include <STC15F2K60S2.h>
#include <intrins.h>
/*---------???---------*/
#define uint unsigned int
#define uchar unsigned char
#define FOSC 11059200L  //????
#define T_ms 0.1        //?????0.1ms
#define NMAX_KEY 10
#define cstAdcPower 0X80     /*ADC????*/
#define cstAdcFlag 0X10      /*?A/D?????,cstAdcFlag?????*/
#define cstAdcStart 0X08     /*?A/D?????,cstAdcStart?????,??????????,?????*/
#define cstAdcSpeed90 0X60   /*ADC???? 90?????????*/
#define cstAdcChs17 0X07     /*??P1.7??A/D??*/
#define cstBaud1     9600       //???

/*???*/
sbit Key1 = P3 ^ 2;    
sbit Key2 = P3 ^ 3; 
//sbit Key3 = P1 ^ 7;        
sbit led = P2 ^ 3;      //LED?????????
sbit SDA = P4 ^ 0;      //I2C??????
sbit SCL = P5 ^ 5;      //I2C??????
sbit Beep = P3 ^ 4;
/*????*/
uchar duan[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //?????,??0-f
uchar wei[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //?????
uchar origin[]={0x00,0x00,0x40,0x76,0x79,0x38,0x38,0x3f}; 
uchar error[]={0x00,0x00,0x40,0x79,0x77,0x77,0x3f,0x77};
uchar open[]={0x00,0x00,0x40,0x40,0x3f,0x73,0x79,0x37};
uchar x=0x00;
uchar s=0x00;
/*????*/ 
uchar flag1 = 0x00;                //?????????(??)
uchar flag2 = 0xff;			//???????????(??0-7?????4-7)
uchar flag3 = 0x00;			//???????? 
uchar addr ;           //????
uchar date[]={0x00,0x00,0x00,0x00};           //????(?????) 
uchar read_date[]={0x00,0x00,0x00,0x00};            //????
uchar ui_state ;				//????(??6??????)  
bit btKey3Flag=0;             /*key3?????*/
bit btUart1SendBusy = 0 ;		//?????? 
bit btBeepFlag=0; 
uchar date_yx = 0x00;
//??????? 
void SMG0(uchar date,uchar * show)
{
	switch( flag1 )
    {
        case 0:
            P0 = P0 | duan[date / 16];   break;
        case 1:
            P0 = P0 | duan[date % 16];   break;
        default:
            P0 = P0 | show[flag1];   break;
    }
}
void SMG1( uchar date1, uchar date2,uchar date3) //??????? 1
{
    switch( flag1 )
    {
        case 0:
            P0 = P0 | duan[date1 / 16];   break;
        case 1:
            P0 = P0 | duan[date1 % 16];   break;
        case 2:
            P0 = 0x40;   break;
        case 3:
        	P0 = 0x40;   break;
        case 4:
            P0 = P0 | duan[date2 / 16];   break;
        case 5:
            P0 = P0 | duan[date2 % 16];   break;
        case 6:
            P0 = P0 | duan[date3 / 16];    break;
        default:
            P0 = P0 | duan[date3 % 16];   break;
    }
}
void SMG2(uchar date1, uchar date2,uchar date3,uchar date4)
{
    switch( flag1 )
    {
        case 0:
            P0 = P0 | duan[date1 / 16];   break;
        case 1:
            P0 = P0 | duan[date1 % 16];   break;
        case 2:
            P0 = P0 | duan[date2 / 16];   break;
        case 3:
        	P0 = P0 | duan[date2 % 16];   break;
        case 4:
            P0 = P0 | duan[date3 / 16];   break;
        case 5:
            P0 = P0 | duan[date3 % 16];   break;
        case 6:
            P0 = P0 | duan[date4 / 16];    break;
        default:
            P0 = P0 | duan[date4 % 16];   break;
    }
}
void Timer_T0() interrupt 1  //???0????
{				
    if( flag1 == 8)
    {
        led=1;
		P0=ui_state+1;
		flag1 = 0;
		return;
    }
    led=0;		 //??? 
    P2 = wei[flag1];
    //if (ui_state == 0x80) flag2 = flag1;
    if (flag1 == flag2 )	P0=0x80;
    else P0=0x00;
	if(ui_state==0x00)			//???? 
		SMG0(addr,origin);
	else if(ui_state==0x01||ui_state==0x04)		//???? ||???? 
		SMG1(addr,date[2],date[3]);
	else if(ui_state==0x03||ui_state==0x05||ui_state==0x08)   //????||???? 
		SMG2(date[0],date[1],date[2],date[3]);
	else if(ui_state==0x02)		//????
		SMG0(addr,error) ;
	else if(ui_state==0x06)			//06
		SMG0(addr,open);
	else if(ui_state == 0x80)		//???? 
		SMG0(addr,origin);
	else P0=0x40;					//??????-------- 
	
   if( btBeepFlag )
        Beep = ~Beep;            //???????????
    else
        Beep = 0;  
	flag1++;
}

/*---------??4us???--------*/
void delay()        
{
    ;;
}
/*---------??5ms???--------*/
void Delay5ms()     //@11.0592MHz  ??5ms
{
    unsigned char i, j;
    i = 54;
    j = 199;
    do
    {
        while ( --j );
    }
    while ( --i );
}
/*---------??100ms???--------*/
void Delayms(uint i) 			
{
	while(i--)
	{	
		int n=500;
		while (n--)
	    {
	        _nop_();
	        _nop_();
	        _nop_();
	        _nop_();
	        _nop_();
			_nop_();
	    }
	}
}

void IIC_init()     //I2C?????
{
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}
void start()        //??????
{
    SDA = 1;
    delay();
    SCL = 1;
    delay();
    SDA = 0;
    delay();
}
void stop()         //????
{
    SDA = 0;
    delay();
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}
void respons()      //??????
{
    uchar i = 0;
    SCL = 1;
    delay();
    //????????????????????
    //????????????????????????
    while( SDA == 1 && ( i < 255 ) ) 
        i++;                
    SCL = 0;
    delay();
}
void writebyte( uchar date ) //?24C02???????
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
uchar readbyte()            //?24C02???????
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
void write_add( uchar addr, uchar date ) //?24C02???addr,??????date
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
uchar read_add( uchar addr )      //?24C02?addr??,???????
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

/*---------??1?????--------*/
void Uart1_Init()
{
    AUXR = 0X80; //????? ?????0???????12?,??? ,1T??,T0x12=1,
    SCON |= 0X50; //????
    TL1 = ( 65536 - ( FOSC / 4 / cstBaud1 ) );
    TH1 = ( 65536 - ( FOSC / 4 / cstBaud1 ) ) >> 8;
    AUXR |= 0X40; //????? ?????1???????12?,???
    RI = 0;      //???????
    TI = 0;      //???????
    TR1 = 1;     //?????1
    ES = 0;      //??????????? 
    EA = 1;      //??????
    PS = 1 ;    //??1??????
}
void Timer0_Init()       //???0???
{
    TMOD = 0x00;        //???0????0,16???????
    EA = 1;             //????
    ET0 = 1;            //????0??
    TH0 = ( 65536 - T_ms * FOSC / 1000 ) / 256;  //???????
    TL0 = ( 65536 - T_ms * FOSC / 1000 );  
    TR0 = 1;                              //?????
}

void init()       //IO????,?????
{
    P2M1 = 0x00;    //??P0??P2^3????
    P2M0 = 0x08;
    P0M1 = 0x00;
    P0M0 = 0xff;    //??????
    P3M0 = 0x00;
    P3M1 = 0x00;
    
    P1ASF = 0x80;       //P1.7??????A/D??
    ADC_RES = 0;        //??????
    ADC_CONTR = 0x8F;   //cstAdcPower = 1
    CLK_DIV = 0X00;     //ADRJ = 0    ADC_RES???????
    
    led = 0;        //?LED??
    P0 = 0x00;
	Beep=0;            //??????
    addr = 0x00;      //???????
    ui_state=0x00; 		//???? 
    btKey3Flag=0;
    Delayms(20); 
}

/*---------??????--------*/
void SendData( unsigned char dat )
{
    while( btUart1SendBusy ); //???????UART1????PC?
    btUart1SendBusy = 1;
    SBUF = dat;
}
/*---------??1??????--------*/
void Uart1_Process() interrupt 4 using 1
{
    if( RI && flag3 ==0x00)         //?????? RI???1
    {
        RI = 0;   
		date[0] = SBUF;
		flag3 =0x01;
    }
    else if( RI && flag3 ==0x01 ){
    	RI = 0;   
		date[1] = SBUF;
		flag3 = 0x02;
	}
	else if( RI && flag3 ==0x02 ){
    	RI = 0;   
		date[2] = SBUF;
		flag3 = 0x03;
	}
	else if( RI && flag3 ==0x03 ){
    	RI = 0;   
		date[3] = SBUF;
		flag3 =0x00;
	}
    else 
    if( TI )        //?????? RI???1
    {
        TI = 0;   btUart1SendBusy = 0;
    }
}

/*---------??AD????--------*/
//?AD????,????A/D????(???IE?IP) 
unsigned char GetADC()
{
    uchar ucAdcRes;
    ADC_CONTR = cstAdcPower | cstAdcStart | cstAdcSpeed90 | cstAdcChs17;//???cstAdcFlag?1,????A/D????
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while( !( ADC_CONTR & cstAdcFlag ) ); //????A/D????
    ADC_CONTR &= ~cstAdcFlag;           //cstAdcFlagE???0
    ucAdcRes = ADC_RES;                 //??AD??
    return ucAdcRes;
}
/*---------??????????--------*/
//???????5??2??4??1????3?????3(0),?????0x07,??????  (??????)
//???????????????? 
uchar NavKeyCheck()
{
    unsigned char key;
    key = GetADC();     //??AD??
    if( key != 255 )    //??????
    {
        Delay5ms();
        key = GetADC();
        if( key != 255 )            //???? ??????
        {
            key = key & 0xE0;       //???3?,?????
            key = _cror_( key, 5 ); //????5? ??A/D??????,????
            return key;
        }
    }
    return 0x07;        //??????????0x07
}

void NavKey_Process()
{
    uchar ucNavKeyCurrent;  //?????????
    uchar ucNavKeyPast;     //?????????

    ucNavKeyCurrent = NavKeyCheck();    //????ADC?
    if( ucNavKeyCurrent != 0x07 )       //????????? ???0x07?????
    {
        ucNavKeyPast = ucNavKeyCurrent;
        while( ucNavKeyCurrent != 0x07 )        //????????
            ucNavKeyCurrent = NavKeyCheck();
         x = 0x01;	s =flag2/2;
        if(flag2%2==0) x=0x10;
        //????,?????????? 
        switch( ucNavKeyPast )
        {
            case 0x00 :                     //K3
                if( btKey3Flag == 0 )
                {
                    btKey3Flag = 1;
                }
                break;
            case 0x05 :                     //??:??????1
                if( date[s] >= 0xf0 && x == 0x10)
                    date[s] = (date[s] & 0x0f);
                else if(((date[s] & 0x0f) == 0x0f) && x == 0x01)
                	date[s] = (date[s]&0xf0) ;
                else
                    date[s]+=x;
                date_yx = 0x01;
                break;
            case 0x02 :                     //??:??????1
                if( date[s] < 0x10 && x == 0x10 )
                    date[s] = (date[s] | 0xf0);
                else if(((date[s] & 0x0f) == 0x00 )&& x==0x01 )
                	date[s] = (date[s] | 0x0f);
                else
				    date[s]-=x;
				date_yx = 0x02;
                break;
            case 0x04 :						//?? 
            	if(ui_state == 0x00) {
            		if(flag2 == 0x00) flag2=0x01;
    				else flag2=0x00;        		
				}
				else if(ui_state == 0x01 || ui_state == 0x04){		//????????? 
					if(flag2 == 0x04) flag2=0x07;
					else flag2--;
				}
				else if( ui_state ==0x05){
					if(flag2 == 0x00) flag2=0x07;
					else flag2--;
				}
				else flag2 = 0xff;
				s=flag2/2;
				date_yx = 0x03;
				break;		
			case 0x01 :						//?? 
				if(ui_state == 0x00) {
            		if(flag2 == 0x01) flag2=0x00;
    				else flag2=0x01;        		
				}
				else if(ui_state == 0x01 || ui_state == 0x04){
					if(flag2 == 0x07) flag2=0x04;
					else flag2++;
				}
				else if( ui_state ==0x05){
					if(flag2 == 0x07) flag2=0x00;
					else flag2++;
				}
				else flag2 = 0xff;
				s=flag2/2;
				date_yx = 0x04;
				break;
			default: 
					break;	
        }
    }
    if(ui_state == 0x00 && flag2 != 0xff && flag2 != 0xf0 ){
    	addr=date[0];
	}
    Delay5ms();
}
//????
 

void main()
{
	init();            //IO?????ADC??? 
	Uart1_Init();   //????:????
    Timer0_Init();
    IIC_init();            //IIC?????
    
    while(1){
    	
    	if(flag2!=0xff)  	//?????????? 
			NavKey_Process();		//???? 
			
    	//???????????——??0x00 
    	if(ui_state == 0x00){ 
			if(flag2 == 0xff && Key1 == 0){
		 		Delay5ms();
		 		if(Key1 == 0){
		 			while(!Key1);
		 			flag2 = 0x01;		//?????,???????? 
				} 	  
		  	}
		  	if(flag2 == 0xff && Key2 == 0){
		  		Delay5ms();
		  		if(Key2 == 0){
		  			while(!Key2);
		  			ui_state = 0x80;
		  			flag2 = 0xf0;			//???? 
		  			date_yx = 0x00;
		  			ES = 1;
				  }
			  }
		  	if(flag2 != 0xff && Key1 == 0){
		  		Delay5ms();
		  		if(Key1==0 ){
		  			while(!Key1);
		  			flag2=0x04;
		  			ui_state=0x01;			//????2 
		  			read_date[2]=read_add( addr*6 );
		  			Delay5ms();
					read_date[3]=read_add(addr*6+1);
				  }
			  }
			if(Key2 == 0 && flag2 !=0xff){			//????KEY2?????? 
				Delay5ms();
				if(Key2 == 0){
					while(!Key2);
					date[2]=0x00;date[3]=0x00;
					flag2=0xff;
					ui_state = 0x00;
				}
			}	  
		}
		//??2--???????????
		if(ui_state==0x01){
			if(Key1 == 0){
				Delay5ms();
				if(Key1==0){
					while(!Key1);
					if(read_date[2] == date[2]&&read_date[3] == date[3]){  //???? 
						flag2 = 0xff;
						ui_state = 0x06;
						Delayms(600);
						read_date[0]=read_add( 6*addr+2 );
						Delay5ms();
						read_date[1]=read_add( 6*addr+3 );
						Delay5ms();
						read_date[2]=read_add( 6*addr+4 ); 
						Delay5ms();
						read_date[3]=read_add( 6*addr+5 );
						date[0] = read_date[0]; date[1] = read_date[1]; 
						date[2] = read_date[2]; date[3] = read_date[3]; 
						flag2 = 0xf0;
						ui_state=0x03;
					}
					else{										//???? 
						flag2 = 0xff;
						ui_state = 0x02;
						btBeepFlag=1;
						Delayms(200);
						btBeepFlag=0;
						flag2=0x07;
						ui_state = 0x01;
					}
				}
			}
			 else if(Key2 == 0){			//????KEY2?????? 
				Delay5ms();
				if(Key2 == 0){
					while(!Key2);
					flag2=0x01;
					ui_state = 0x00;
				}
			}	
		}
		//??3--???? --????flag2 == 0xf0 
		if(ui_state == 0x03){
			if(Key2 == 0){			//???? 
				Delay5ms();
				if(Key2 == 0){
					while(!Key2);
					date[2]=0x00;date[3]=0x00;
					ui_state= 0x04;
					flag2= 0x04;
				}
			}
			if(btKey3Flag == 1){		//???? 
				Delay5ms();
				//if(Key3 == 0){
				//	while(!Key3);
					flag2 = 0x00;
					ui_state = 0x05;
				//}
			} 	
			if(Key1 == 0){		//?????? 0x08
				Delay5ms();
				if(Key1 == 0){
					while(!Key1);
					flag2 = 0xf0;
					ui_state = 0x08;
					ES = 1;
				}
			}
		}
		//????  
		if(ui_state == 0x04){
			if(Key1 == 0){
				Delay5ms();
				if(Key1 == 0){
					while(!Key1);
					write_add(addr*6,date[2]);
					Delay5ms();
					write_add(addr*6+1,date[3]);
					flag2 = 0xff;
					ui_state = 0x07;
					Delayms(1000);				//?????
					flag2 = 0xf0; 
					ui_state = 0x03;
					date[0] = read_date[0]; date[1] = read_date[1]; 
					date[2] = read_date[2]; date[3] = read_date[3]; 		//?????? 
				} 
			} 
		}
		//????
		if(ui_state == 0x05){
			if(Key1 == 0){
				Delay5ms();
				if(Key1 == 0){
					while(!Key1);
					write_add(6*addr+2,date[0]);
					Delay5ms();						//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					write_add(6*addr+3,date[1]);
					Delay5ms();
					write_add(6*addr+4,date[2]);
					Delay5ms();
					write_add(6*addr+5,date[3]);
					read_date[0] = date[0]; 
					read_date[1] = date[1]; 
					read_date[2] = date[2]; 
					read_date[3] = date[3]; 		//?????? 
					flag2 = 0xff; 
					ui_state = 0x07;
					Delayms(600);				//?????
					ui_state = 0x03;
					flag2 = 0xf0;
				}
			}
		}
		//????
		if(ui_state == 0x08){
			if(Key2 == 0){			//Key2???? 
				Delay5ms();
				if(Key2 == 0){
					while(!Key2);
					write_add(6*addr+2,date[0]);
					Delay5ms();						//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					write_add(6*addr+3,date[1]);
					Delay5ms();
					write_add(6*addr+4,date[2]);
					Delay5ms();
					write_add(6*addr+5,date[3]); 
				} 
			}
			if(btKey3Flag == 1) {	//Key3?? 
				SendData( date[0] );
                SendData( date[1] );
                SendData( date[2] );
                SendData( date[3] );
			}
			if(Key1 == 0){
				Delay5ms();
				if(Key1 == 0){
					while(!Key1);
					flag2 = 0xff;
					ui_state = 0x00;
					ES=0; 
					date[0]=0;date[1]=0;date[2]=0;date[3]=0;
					read_date[0]=0;read_date[1]=0;read_date[2]=0;read_date[3]=0;
					
				}
			} 
			 
		}
		//????
		if(ui_state == 0x80) {
		 while(1){
			if(Key1 == 0){
				Delay5ms();
				if(Key1 == 0){
					while(!Key1);
					ui_state = 0x00;
					flag2 = 0xff;
					date_yx = 0x00;
					ES = 0;
					break;
				}
			}
			NavKey_Process();		//???? 
			Delayms(5);
			if(date_yx != 0x00){
				SendData( date_yx );
				Delayms(2);
				date_yx = 0x00;
			}
		 }
		}
		if (btKey3Flag == 1) btKey3Flag = 0 ; 
	} 
}