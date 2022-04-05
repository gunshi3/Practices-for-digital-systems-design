$include(STC15F2K60S2.H)  

KEY1   BIT   P3.2
KEY2   BIT   P3.3
KEY3   BIT   P1.7

LED1   BIT   P0.0
LED2   BIT   P0.1
LED3   BIT   P0.2

ORG   0000H
        LJMP   START

;---------------------------------------������----------------------------
ORG   0030H            	 
START:  MOV    P0M1, #00000000B        ;��P0��P2������ʽ  
        MOV    P0M0, #11111111B 	        ;00����׼˫���   01��������   10������������	 11����OC
        MOV    P2M1, #00000000B
	    MOV    P2M0, #00001000B
		MOV    P2,   #00001000B		   ;ѡ��LED�ƹ���
		MOV    P0,   #00000000B		   ;�ر�����LED��
MAINLOOP:
		JB     KEY1,  L1		  ;�鿴KEY1
		SETB   LED1
		JMP    L2
L1:     CLR    LED1

L2:		JB     KEY2,  L3		  ;�鿴KEY2
		SETB   LED2
		JMP    L4
L3:     CLR    LED2

L4:	   	JB     KEY3,  L5		  ;�鿴KEY3
		SETB   LED3
		JMP    L6
L5:     CLR    LED3

L6:	    JMP    MAINLOOP 

        END           

