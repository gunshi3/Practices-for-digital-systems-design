	   ;******************************************************************************
;	***************ģ�ⰴ�������ְ����ۺϿ�������ܳ���********************
;			**************************************************
$include(STC15F2K60S2.H) 
KEY EQU 50H
RESULT EQU 51H
ORG 0000H
	LJMP MAIN
ORG 0003H;����ⲿ�ж����
	LJMP OUT_INTURRETP0
ORG 000BH;��Ŷ�ʱ���ж����
	LJMP TIME_INTURREPT
ORG 0013H;һ���ⲿ�ж����
	LJMP OUT_INTURREPT1

;****************0���ⲿ�жϳ��򣨶�Ӧ�ڰ���key1��********************
OUT_INTURRETP0 :
	CALL DELAY1MS
	JB P32,RETURN1
	JNB P32,$
	CJNE R1,#01H,RETURN1
	INC R2
	CJNE R2,#08H,RETURN1
	MOV R2,#00H
RETURN1 :
	RETI
	
;****************��ʱ���жϳ���������ʾ�����*******************
TIME_INTURREPT :
	PUSH ACC
	MOV TL0,#0A4H		
	MOV TH0,#0FFH		
	MOV A,R3
	MOV DPTR,#DUAN_DATA
	MOVC A,@A+DPTR
	MOV P0,A
	MOV P2,R2
	POP ACC
	RETI

;****************һ���ⲿ�жϳ��򣨶�Ӧ�ڰ���key2��********************
OUT_INTURREPT1:
	CALL DELAY1MS
	JB P33,RETURN2
	JNB P33,$	
	CJNE R1,#01H,RETURN2
	DEC R2
	CJNE R2,#0FFH,RETURN2
	MOV R2,#07H
RETURN2:
	RETI
	
;*************��ȡA/Dת��ֵ�ӳ���**********
GET_ADC:
	MOV A,ADC_CONTR
	ANL A,#00010000B
	CJNE A,#10H,GET_ADC
	MOV A,ADC_RES
	ANL A,#0E0H		;ͨ���������Լ�ѭ����������λ�������λ��ֵ������ת�����
	RR A
	RR A
	RR A
	RR A
	RR A
	MOV KEY,A
	MOV ADC_CONTR,#8FH
	RET
	
;*************ģ�������������ӳ���********************
FUN_KEYCHECK:
	CALL GET_ADC
	MOV A,KEY
	CJNE A,#7,CONTINUE1
	LJMP RETURN3
CONTINUE1:
	CALL DELAY1MS
	CALL GET_ADC
RETURN3:
	RET
	
;*************����ȷ���ӳ���*********
Search_Keyscan:
	CALL FUN_KEYCHECK
	MOV A,KEY
	CJNE A,#7,CONTINUE3
	LJMP RETURN4
CONTINUE3:
	MOV RESULT,A
CALL_SK:
	CALL FUN_KEYCHECK
	MOV A,KEY
	CJNE A,#7,CALL_SK
	MOV A,RESULT
	CJNE A,#5,NEXT_COMPARE1;�ж��Ƿ��ǵ�����������
	INC R3
	CJNE R3,#16,RETURN4
	MOV R3,#0
	LJMP RETURN4
NEXT_COMPARE1:
	CJNE A,#2,NEXT_COMPARE2;�ж��Ƿ��ǵ�����������
	DEC R3
	CJNE R3,#0FFH,RETURN4
	MOV R3,#15
	LJMP RETURN4
NEXT_COMPARE2:
	CJNE A,#0,RETURN4;�ж��Ƿ��ǰ���3
	INC R1
	CJNE R1,#02H,RETURN4
	MOV R1,#00H
RETURN4:
	RET

;***********��ʱ�ӳ�����ʱ1MS*******************
DELAY1MS:			
	PUSH 30H
	PUSH 31H
	MOV 30H,#200	
NEXT:
	MOV 31H,#200
	DJNZ 31H,$
	DJNZ 30H,NEXT
	POP 31H
	POP 30H
	RET
	
;�ӳ���*******��ʼ������**********************
INITDATA:
	MOV KEY,#00H
	MOV	RESULT,#00H
	MOV R0,#00H
	MOV R1,#00H
	MOV R2,#00H
	MOV R3,#00H
	MOV P2M1,#00000000B;���ö˿������ʽ
	MOV P2M0,#11111111B
	MOV P0M1,#00000000B
	MOV P0M0,#11111111B
	ANL AUXR,#7FH	;��ʼ����ʱ���ж�
	ANL TMOD,#0F0H		
	ORL TMOD,#01H		
	MOV TL0,#0A4H		
	MOV TH0,#0FFH	
	CLR TF0			
	SETB TR0		
	SETB IT0;����  INT0  Ϊ�½���\�����ش���
	SETB IT1	
	MOV IE,#087H;���ж�
	MOV P0,#00H
	MOV P1ASF,#0FFH		;��ʼ��AD��ģת��
	MOV ADC_RES,#0
	MOV ADC_CONTR,#8FH
	MOV CLK_DIV,#00H
	CALL DELAY1MS
	RET
	
;***********������***����ִ�п�ʼ��*******
MAIN :
	CALL INITDATA
	MOV R1,#01H
MAINLOOP :	
	CALL Search_Keyscan
	CALL DELAY1MS
	LJMP MAINLOOP
DUAN_DATA: DB 3FH,06H,5BH,4fH,66H,6dH,7dH,07H,7fH,6fH,77H,7cH,39H,5eH,79H,71H
END