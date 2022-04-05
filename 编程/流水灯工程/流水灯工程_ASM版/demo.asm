$include(STC15F2K60S2.H)  

ORG   0000H  //αָ��
        LJMP   START  //��ַ����

;---------------------------------------������----------------------------
ORG   0100H     
//��ʼ���������������
START:  MOV    P0M1, #00000000B            ;00����׼˫���   01��������   10������������	 11����OC
        MOV    P0M0, #11111111B 
        MOV    P2M1, #00000000B
	    MOV    P2M0, #00001000B
		MOV    P2,   #00001000B
		
MAINLOOP:
//ѭ������led��
        MOV    P0,   #00000001B
		CALL   DELAY   //����delay����
        MOV    P0,   #00000010B
		CALL   DELAY
		MOV    P0,   #00000100B
		CALL   DELAY
		MOV    P0,   #00001000B
		CALL   DELAY
		MOV    P0,   #00010000B
		CALL   DELAY
		MOV    P0,   #00100000B
		CALL   DELAY
		MOV    P0,   #01000000B
		CALL   DELAY
		MOV    P0,   #10000000B
		CALL   DELAY
        JMP    MAINLOOP  

DELAY:  MOV    R0,   #0fFH
L1:     MOV    R1,   #04FH
L2:		MOV    R2,   #20H
L3:     NOP
        DJNZ   R2,   L3
		DJNZ   R1,   L2
		DJNZ   R0,   L1
		RET        

        END           //�������

