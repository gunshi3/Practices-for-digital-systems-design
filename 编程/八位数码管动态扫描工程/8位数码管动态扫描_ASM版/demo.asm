$include(STC15F2K60S2.H)  

ORG   0000H
        LJMP   START

;---------------------------------------������----------------------------
ORG   0100H            	 
START:  MOV    P0M1, #00000000B            ;00����׼˫���   01��������   10������������	 11����OC
        MOV    P0M0, #11111111B 
        MOV    P2M1, #00000000B
	    ;MOV    P2M0, #00001000B
		;MOV    P2,   #00001000B
MAINLOOP:
        MOV    P2,   #00000000B			;λѡ1
		mov    P0,   #00000110B		    ;��ʾ1
		CALL   DELAY 
        MOV    P2,   #00000001B		    ;λѡ2
		mov    P0,   #01011011B		    ;��ʾ2
		CALL   DELAY
		MOV    P2,   #00000010B		    
		mov    P0,   #01001111B
		CALL   DELAY
		MOV    P2,   #00000011B
		mov    P0,   #01100110B
		CALL   DELAY
		MOV    P2,   #00000100B
		mov    P0,   #01101101B
		CALL   DELAY
		MOV    P2,   #00000101B
		mov    P0,   #01111101B
		CALL   DELAY
		MOV    P2,   #00000110B
		mov    P0,   #00000111B
		CALL   DELAY
		MOV    P2,   #00000111B
		mov    P0,   #01111111B
		CALL   DELAY
        JMP    MAINLOOP

DELAY:  MOV    R0,   #00FH
L1:     MOV    R1,   #02FH
L2:		MOV    R2,   #04H
L3:     NOP
        DJNZ   R2,   L3
		DJNZ   R1,   L2
		DJNZ   R0,   L1
		RET        

        END           

