$include(STC15F2K60S2.H) 
FOSC      EQU   11059  
TCLICK    EQU   50      ;������ʱ��   uS
NMAX_KEY  EQU   20      ;��
                            			           
DATA_TH0  EQU   256-(FOSC/120*TCLICK+50)/100  ;��ʱ��0 ������ʽ2��8bit��װ������256uS���ڶ�ʱ  ��ʱ��ֵ����256��Fosc����ʱʱ��/12��
 
CLICK_50uS       BIT            00H      ;=100uS�δ��ź�
CLICK_1mS        BIT            01H      ;=1mS  �δ��ź�
CLICK_50mS       BIT            02H      ;=20mS �δ��ź�
CLICK_200mS      BIT            03H
CLICK_1S         BIT            04H      ;=1S   �δ��ź�
CLICK_1Min       BIT            05H      ;=1Min �δ��ź�
CLICK_1HOUR      BIT            06H 

beep             bit            p3.4

KEY1             BIT            P3.2      ;������ʱֵ
KEY2             BIT            P3.3
KEY3             BIT            0AH
KEYR             BIT            0BH
KEYD             BIT            0CH
KEYC             BIT            0DH
KEYL             BIT            0EH
KEYU             BIT            0FH   

KEY1_C           BIT            10H		;������ǰ״̬
KEY2_C           BIT            11H
KEY3_C           BIT            12H
KEYR_C           BIT            13H
KEYD_C           BIT            14H
KEYC_C           BIT            15H
KEYL_C           BIT            16H
KEYU_C           BIT            17H 

KEY1_P           BIT            18H		;������һ��״̬
KEY2_P           BIT            19H
KEY3_P           BIT            1AH
KEYR_P           BIT            1BH
KEYD_P           BIT            1CH
KEYC_P           BIT            1DH
KEYL_P           BIT            1EH
KEYU_P           BIT            1FH         
 
VKEY1            BIT            20H      ;�����¼�ֵ
VKEY2            BIT            21H
VKEY3            BIT            22H
VKEYR            BIT            23H
VKEYD            BIT            24H
VKEYC            BIT            25H
VKEYL            BIT            26H
VKEYU            BIT            27H 

KEY_ON           BIT            28H

TIME_50uS        EQU            30H      ;100uS����
TIME_1mS         EQU            31H      ;1mS  ����
TIME_50mS        EQU            32H      ;20mS ����
TIME_1S          EQU            34H      ;1S   ����
TIME_1MIN        EQU            35H      ;1MIN  ����
TIME_1HOUR       EQU            36H      ;1HOUR ����

SEC_1            EQU            40H
SEC_10           EQU            41H
MIN_1            EQU            42H
MIN_10           EQU            43H
HOUR_1           EQU            44H
HOUR_10          EQU            45H
MID_LINE         EQU            46H
NO_DIS           EQU       5AH

KEY_COUNTER      EQU            47H	    ;������������

KEY1_COUNTER     EQU            48H		;����ֵͳ��
KEY2_COUNTER     EQU            49H
KEY3_COUNTER     EQU            4AH
KEYR_COUNTER     EQU            4BH
KEYD_COUNTER     EQU            4CH
KEYC_COUNTER     EQU            4DH
KEYL_COUNTER     EQU            4EH
KEYU_COUNTER     EQU            4FH

GROUP_Dis        EQU            57H     ;��ǰ��ʾ������  ��  0��N
N_MaxDis         EQU            58H		;���������ʾλ������С��8���ɴ���8�ɿ�����ʾ����
CurDis           EQU            59H     ;��������¼��ʾλ��1��8 ��Ч  ����8����ʾ
COUNT1           EQU            5BH
COUNT2           EQU            5CH
COUNT3           EQU            5DH
COUNT4           EQU            5EH

;��ʾ���ݶ���
;GROUP_Dis=0
G0_A1            EQU			NO_DIS
G0_A2            EQU			NO_DIS
G0_A3            EQU			NO_DIS
G0_A4            EQU			NO_DIS
G0_A5            EQU			NO_DIS
G0_A6            EQU			NO_DIS
G0_A7            EQU			NO_DIS
G0_A8            EQU			COUNT1

;GROUP_Dis=1
G1_A1            EQU			NO_DIS	; ����嶨������
G1_A2            EQU			NO_DIS
G1_A3            EQU			NO_DIS
G1_A4            EQU			NO_DIS
G1_A5            EQU			COUNT4
G1_A6            EQU			COUNT3
G1_A7            EQU			COUNT2
G1_A8            EQU			COUNT1

;GROUP_Dis=2
G2_A1            EQU			MID_LINE
G2_A2            EQU			MID_LINE
G2_A3            EQU			MID_LINE
G2_A4            EQU			GROUP_Dis
G2_A5            EQU			MID_LINE
G2_A6            EQU			MID_LINE
G2_A7            EQU			MID_LINE
G2_A8            EQU			SEC_1

ORG   0000H
        LJMP   START
ORG   0003H
		RETI
ORG   0013H
		RETI
;-----------------------------------------TIMER0�ж�----------------------
ORG   000BH
        JMP   INT_T0

;---------------------------------------������----------------------------
ORG   0100H            	 
START: 
;-----------------------ϵͳ��ʼ��-----------------
        MOV    P0M1, #00000000B            ;00����׼˫���   01��������   10������������	 11����OC
        MOV    P0M0, #11111111B 
        MOV    P2M1, #00000000B
	    MOV    P2M0, #00000001B
        MOV    P3M1, #00000000B
	    MOV    P3M0, #00010000B

;----------------------������ʼ��--------------------
		MOV   TIME_50uS,    #20
        MOV   TIME_1mS,     #50
        MOV   TIME_50mS,    #20   
        MOV   TIME_1S,      #60
		MOV   TIME_1MIN,    #60
		MOV   TIME_1HOUR,   #24

		CLR   CLICK_50uS
        CLR   CLICK_1mS        
        CLR   CLICK_50mS  
		CLR   CLICK_1S
		CLR   CLICK_1Min
		CLR   CLICK_1HOUR

		CLR   VKEY1
		CLR   VKEY2
		CLR   VKEY3
		CLR   VKEYR
		CLR   VKEYD
		CLR   VKEYC
		CLR   VKEYL
		CLR   VKEYU

        SETB  KEY1_P
        SETB  KEY2_P
        SETB  KEY3_P
        SETB  KEYR_P
        SETB  KEYD_P
        SETB  KEYC_P
        SETB  KEYL_P
        SETB  KEYU_P

		MOV   KEY_COUNTER,  #NMAX_KEY

		MOV   KEY1_COUNTER, #(80H+NMAX_KEY/3*2)
	    MOV   KEY2_COUNTER, #(80H+NMAX_KEY/3*2)
		MOV   KEY3_COUNTER, #(80H+NMAX_KEY/3*2)
		MOV   KEYR_COUNTER, #(80H+NMAX_KEY/3*2)
		MOV   KEYD_COUNTER, #(80H+NMAX_KEY/3*2)
	    MOV   KEYC_COUNTER, #(80H+NMAX_KEY/3*2)
		MOV   KEYL_COUNTER, #(80H+NMAX_KEY/3*2)
		MOV   KEYU_COUNTER, #(80H+NMAX_KEY/3*2)

        MOV   CurDis,       #1        ;��ʼ��ʾλ	
		MOV   N_MaxDis,     #8
		MOV   GROUP_Dis,    #0
			     
		MOV   SEC_1,        #0		  ;ʾ����������ʾ���ݸ�����Ӧ����
		MOV   SEC_10,       #0
		MOV   MIN_1,        #0
		MOV   MIN_10,       #0
		MOV   HOUR_1,       #0
		MOV   HOUR_10,      #0
		MOV   MID_LINE,     #10
		MOV   NO_DIS,       #11

	    CLR   KEY_ON
		MOV   COUNT1,       #0
;------------------ϵͳ�豸��ʼ��----------------------------------
        MOV   TMOD, #22H    ; C/T0 mode is 2 and C/T1 'S MOD is 2
		MOV   PCON, #80H     ;  
		MOV   TH0,  #DATA_TH0 
		SETB  IT0					     ;����  INT0  Ϊ�½��ش���
		SETB  IT1     
        SETB  TR0						 ;����ʱ��
        MOV   IE,   #10000010B  	 	 ;���жϣ� 		      	          
;-----------------------------------��ѭ��------------------------------------------
MAINLOOP:JBC   CLICK_50uS,  C_50uS 
         JBC   CLICK_1mS,   C_1mS  
         JBC   CLICK_50mS,  C_50mS
	     JBC   CLICK_1S,    C_1S
 	     JBC   CLICK_1Min,  C_1Min 
		 JBC   CLICK_1HOUR, C_1HOUR 
		 JBC   VKEY1,       C_KEY1  
		 JBC   VKEY2,       C_KEY2 
		 JBC   VKEY3,       C_KEY3
		 JBC   VKEYR,       C_KEYR
		 JBC   VKEYD,       C_KEYD
		 JBC   VKEYC,       C_KEYC
		 JBC   VKEYL,       C_KEYL
		 JBC   VKEYU,       C_KEYU
         JMP   MAINLOOP				   
;-----------------------------------------------------
C_50uS:  JMP   TASK_50uS 
C_1mS:   JMP   TASK_1mS  
C_50mS:  JMP   TASK_50mS
C_1S:    JMP   TASK_1S
C_1Min:  JMP   TASK_1Min
C_1HOUR: JMP   TASK_1HOUR
C_KEY1:  JMP   TASK_KEY1
C_KEY2:  JMP   TASK_KEY2
C_KEY3:  JMP   TASK_KEY3
C_KEYR:  JMP   TASK_KEYR
C_KEYD:  JMP   TASK_KEYD
C_KEYC:  JMP   TASK_KEYC
C_KEYL:  JMP   TASK_KEYL
C_KEYU:  JMP   TASK_KEYU
;-----------------------------------�յ�ָ��-------------------------
NEW_COMMAND:
		                                          ;�����������Ҫ��ʾ��ȫ������
         JMP    MAINLOOP
;------------------------------------����------------------------------
TASK_KEY1:
         INC    COUNT1
		 MOV    A,   COUNT1
		 CJNE   A,   #10,  T_KEY1_5
		 MOV    COUNT1,   #0
T_KEY1_5:JMP    MAINLOOP

TASK_KEY2:
		 CPL    KEY_ON
         JMP    MAINLOOP

TASK_KEY3:
         
         JMP    MAINLOOP
TASK_KEYR:
         JMP    MAINLOOP
TASK_KEYD:
         JMP    MAINLOOP
TASK_KEYC:
         JMP    MAINLOOP
TASK_KEYL:
         JMP    MAINLOOP
TASK_KEYU:
         JMP    MAINLOOP
;-------------------------------------��ʱ����-------------------------
TASK_50uS:
         JB    KEY_ON,   T50uS_2
		 CLR   BEEP
		 JMP   T50uS_1
T50uS_2:
         DJNZ  R3,     T50uS_1
		 CPL   BEEP
         MOV   A,     COUNT1
		 ADD   A,     #5
		 MOV   R3,    A
T50uS_1: JMP   MAINLOOP
;------------------------------  
TASK_1mS:MOV    P0,     #00000000B        ;����ʾ 
         CLR    P2.3
         MOV    A,      CurDis	
		 	
		 CJNE   A,      #0FFH,   T1mS_1
		 MOV    A,      #00000001B
		 MOV    R0,     GROUP_Dis
		 INC    R0
T1mS_0:  RR     A
		 DJNZ   R0,     T1mS_0    

		 MOV    P0,     N_MaxDis		   ;CurDis=0FFHʱ����ʾLED��

         SETB   P2.3	 
		 JMP    T1mS_2

T1mS_1:	 ANL    A,      #11111000B		 
		 JNZ    T1mS_2					  ;����ʾ����8��λ��

         MOV    A,      GROUP_Dis		   ;�Ͷ�ѡ
		 RL     A						          ;ѡ��ʾ��
		 RL     A
		 RL     A
		 ADD    A,      CurDis		     
		 MOV    DPTR,   #TAB_GROUP_Dis
		 MOVC   A,      @A+DPTR
		 MOV    R0,     A
		 MOV    A,      @R0
		 MOV    DPTR,   #TAB_Decode
		 MOVC   A,      @A+DPTR
		 MOV    P0,     A				           ;��ѡ ��P0��

         MOV    P2,      CurDis			  	       ;λѡ�� P2��
				  
T1mS_2:  INC    CurDis                    ;ȷ����һ��Ҫ��ʾ��λ 	    
		 MOV    A,      CurDis
		 CJNE   A,      N_MaxDis,  T1mS_3 
		 MOV    CurDis, #0FFH

T1mS_3:	 JB     KEY1,   T1mS_4             ;������
		 DEC    KEY1_COUNTER			   ;����ͳ��
T1mS_4:  JB     KEY2,   T1mS_5
         DEC    KEY2_COUNTER

T1mS_5:  DJNZ   KEY_COUNTER,  T1mS_END 
		 MOV    KEY_COUNTER,  #NMAX_KEY   ;ͳ�ƴ�����������ͳ�ƴ���

         MOV   A,      KEY1_COUNTER	   ;����KEY1
	  	 MOV   KEY1_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEY1_C,  C		       ;�õ����� KEY1 ��ǰ��ͳ��״̬
		 JB    KEY1_C,  T1mS_6		   ;������ KEY1 ��Ӧ�Ĳ���������Ӧ����ͼ��
		 JNB   KEY1_P,  T1mS_7	
		 CLR   KEY1_P
		 SETB  VKEY1		            ;����KEY1�¼��������½��أ�
         JMP   T1mS_7
T1mS_END: JMP    MAINLOOP
T1mS_6:  JB    KEY1_P,  T1mS_7	
		 SETB  KEY1_P

T1mS_7:  MOV   A,      KEY2_COUNTER	   ;����KEY2
	  	 MOV   KEY2_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEY2_C,  C		       ;�õ����� KEY2 ��ǰ��ͳ��״̬
		 JB    KEY2_C,  T1mS_8		   ;������ KEY2 ��Ӧ�Ĳ���������Ӧ����ͼ��
		 JNB   KEY2_P,  T1mS_9	
		 CLR   KEY2_P
		 SETB  VKEY2		            ;����KEY2�¼��������½��أ�
         JMP   T1mS_9
T1mS_8:  JB    KEY2_P,  T1mS_9	
		 SETB  KEY2_P

T1mS_9:	 MOV   A,      KEY3_COUNTER	   ;����KEY3
	  	 MOV   KEY3_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEY3_C,  C		       
		 JB    KEY3_C,  T1mS_A		   
		 JNB   KEY3_P,  T1mS_B	
		 CLR   KEY3_P
		 SETB  VKEY3		            
         JMP   T1mS_B
T1mS_A:  JB    KEY3_P,  T1mS_B	
		 SETB  KEY3_P

T1mS_B:	 MOV   A,      KEYL_COUNTER	   ;����KEYL
	  	 MOV   KEYL_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEYL_C,  C		       
		 JB    KEYL_C,  T1mS_C		   
		 JNB   KEYL_P,  T1mS_D	
		 CLR   KEYL_P
		 SETB  VKEYL		            
         JMP   T1mS_D
T1mS_C:  JB    KEYL_P,  T1mS_D	
		 SETB  KEYL_P

T1mS_D:	 MOV   A,      KEYD_COUNTER	   ;����KEYD
	  	 MOV   KEYD_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEYD_C,  C		       
		 JB    KEYD_C,  T1mS_E		   
		 JNB   KEYD_P,  T1mS_F	
		 CLR   KEYD_P
		 SETB  VKEYD		            
         JMP   T1mS_F
T1mS_E:  JB    KEYD_P,  T1mS_F	
		 SETB  KEYD_P

T1mS_F:	 MOV   A,      KEYC_COUNTER	   ;����KEYC
	  	 MOV   KEYC_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEYC_C,  C		       
		 JB    KEYC_C,  T1mS_G		   
		 JNB   KEYC_P,  T1mS_H	
		 CLR   KEYC_P
		 SETB  VKEYC		            
         JMP   T1mS_H
T1mS_G:  JB    KEYC_P,  T1mS_H	
		 SETB  KEYC_P

T1mS_H:	 MOV   A,      KEYL_COUNTER	   ;����KEYL
	  	 MOV   KEYL_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEYL_C,  C		       
		 JB    KEYL_C,  T1mS_I		   
		 JNB   KEYL_P,  T1mS_J	
		 CLR   KEYL_P
		 SETB  VKEYL		            
         JMP   T1mS_J
T1mS_I:  JB    KEYL_P,  T1mS_J	
		 SETB  KEYL_P

T1mS_J:	 MOV   A,      KEYU_COUNTER	   ;����KEYU
	  	 MOV   KEYU_COUNTER, 	#(80H+NMAX_KEY/3*2)	   ;������һ��ͳ�Ƴ�ֵ
		 RLC   A
		 MOV   KEYU_C,  C		       
		 JB    KEYU_C,  T1mS_K		   
		 JNB   KEYU_P,  T1mS_L	
		 CLR   KEYU_P
		 SETB  VKEYU		            
         JMP   T1mS_L
T1mS_K:  JB    KEYU_P,  T1mS_L	
		 SETB  KEYU_P

T1mS_L:	 JMP    MAINLOOP

TAB_GROUP_Dis:   DB   G0_A1,G0_A2,G0_A3,G0_A4,G0_A5,G0_A6,G0_A7,G0_A8
                 DB   G1_A1,G1_A2,G1_A3,G1_A4,G1_A5,G1_A6,G1_A7,G1_A8
			     DB   G2_A1,G2_A2,G2_A3,G2_A4,G2_A5,G2_A6,G2_A7,G2_A8

TAB_Decode:	 DB   03FH			; 0
             DB   06H			; 1
			 DB   5BH		    ; 2
			 DB   4FH			; 3
			 DB   66H			; 4
			 DB   6DH			; 5
			 DB   7DH			; 6
			 DB   07H			; 7
			 DB   7FH			; 8 
			 DB   6FH			; 9
			 DB   40H			; -
			 DB   00H           ; ����ʾ
;------------------------------
TASK_50mS:
        JMP   MAINLOOP
;------------------------------
TASK_1S:
        INC   SEC_1
		MOV   A,        SEC_1
		CJNE  A,        #10,    T_1S_1
		MOV   SEC_1,    #0
		INC   SEC_10
		MOV   A,        SEC_10
		CJNE  A,        #6,     T_1S_1
		MOV   SEC_10,   #0
T_1S_1: JMP   MAINLOOP
;------------------------------
TASK_1Min:
        INC   MIN_1
		MOV   A,        MIN_1
		CJNE  A,        #10,    T_1M_1
		MOV   MIN_1,    #0
		INC   MIN_10
		MOV   A,        MIN_10
		CJNE  A,        #6,     T_1M_1
		MOV   MIN_10,   #0
 T_1M_1:JMP   MAINLOOP
;------------------------------
TASK_1HOUR:
        INC   HOUR_1
		MOV   A,        HOUR_1
		CJNE  A,        #10,    T_1H_1
		MOV   HOUR_1,   #0
		INC   HOUR_10
		MOV   A,        HOUR_10
		CJNE  A,        #2,     T_1H_1
		MOV   HOUR_10,  #0
 T_1H_1:JMP   MAINLOOP
;--------------------------------------INT_T0--------------------------
INT_T0: SETB   CLICK_50uS
  	    DJNZ   TIME_50uS,    EXIT_T0
        MOV    TIME_50uS,    #20
        SETB   CLICK_1mS 
  	    DJNZ   TIME_1mS,     EXIT_T0
        MOV    TIME_1mS,     #50
	    SETB   CLICK_50mS
	    DJNZ   TIME_50mS,    EXIT_T0
        MOV    TIME_50mS,    #20
	    SETB   CLICK_1S
	    DJNZ   TIME_1S,      EXIT_T0
        MOV    TIME_1S,      #60
	    SETB   CLICK_1Min 
	    DJNZ   TIME_1MIN,    EXIT_T0
        MOV    TIME_1MIN,    #60
	    SETB   CLICK_1HOUR			  
EXIT_T0:RETI

        END           

