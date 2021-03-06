
#IFNDEF	DRAGRACE_TIMER
#DEFINE	DRAGRACE_TIMER

;LIST

sync_C2OUT		EQU	b'00000011'
sync_C1OUT		EQU	b'00000010'
DR_STAGE_0		EQU	0X00		; RACE HASN'T STARTED, OR JUST STARTED
DR_STAGE_1		EQU	0X01		; CAR HAS REACHED THE 1ST SENSOR, TMR1 STARTED, START TMR4. UPDATE APP_STATUS<0..2> TO DR_STAGE_1
DR_STAGE_2		EQU	0X02		; CAR HAS PASSED THE 1ST SENSOR, TMR1 STOPPED.	STORE TMR1.
DR_STAGE_3		EQU	0X03		; CAR HAS REACHED THE 2ND SENSOR, TMR1 STARTED, STOP TMR4.
DR_STAGE_4		EQU	0X04		; CAR HAS PASSED THE 2ND SENSOR, TMR1 STOPPED. SUCCESS! REPORT RACE. CLR TIMR1. CLR TMR4.
DR_STAGE_5		EQU	0X05		; ERROR. DR_STAGE_2 WAS REACHED, AND A 2ND PULSE INTERRUPT FROM THE 1ST COMP. REPORT ERROR. A
DR_STAGE_6		EQU	0X06		; ERROR. DR_STAGE_2 WAS REACHED, AND A 2ND PULSE INTERRUPT FROM THE 2ND COMP. REPORT ERROR. 
DR_STAGE_7		EQU	0X07

;**********************************************************************************
;	DRAGRACE APP OVERVIEW
;		THIS APP MONITORS THE TIMING RELATIONSHIPS OF AND BETWEEN TWO PULSES 
;		GENERATED BY THE COMPARATOR 1 AND COMPARATOR 2 CIRCUITS, WHEN A 
;		MAGNET-EQUIPPED HOT-WHEELS CAR ROLLS OVER TWO DIFFERENT MAGNETIC SENSORS.
;		SEQUENCE IS AS FOLLOWS:
;
;		COMP1 OUT GOES - .... TMR1 STARTS. TMR4 STARTS. 
;		COMP1 OUT GOES + .... TMR1 STOPS. TMR1 VALUE IS READ, STORED, AND CLEARED.
;		COMP2 OUT GOES - .... TMR1 STARTS. TMR4 STOPS. 
;		COMP2 OUT GOES = .... TMR1 STOPS. TMR1 VALUE IS READ, STORED, AND CLEARED.
;							  TMR4'S VALUE IS READ, STORED, AND CLEARED.
;		ALL 3 TIMES (PER_1,PER_2, AND PULSE_SPACING) ARE REPORTED OVER BIGZ.
;**********************************************************************************
DRAGRACE	CODE


;**********************************************************************************
;	FUNCTION: DETERMINE INTERRUPT SOURCE AND RETUN APPROPRIATE VECTOR
;**********************************************************************************
DETERM_INT_SOURCE
  		BANKSEL	PIR2
  		BTFSC	PIR2,	C1IF	  		; CHECK FOR COMPARATOR INTERRUPT
  		RETLW	SERV_C1IF				;...
  		BTFSC	PIR2,	C2IF			;...
  		RETLW	SERV_C2IF				;...
  		BTFSC	PIR1,	TMR1IF			; CHECK FOR COMPARATOR INTERRUPT
  		RETLW	SERV_TMR1IF 			
  		CLRW
  	RETURN
;**********************************************************************************

GLOBAL  DRAGRACE_INT_ISR

;**********************************************************************************
;	FUNCTION:  DRAGRACE_ISR
;	DESC:	RESPOND TO COMPARATOR PULSES AND TIMER1 INTERRUPTS
;			CAUSED WHEN CAR ROLLS OVER SENSORS
;**********************************************************************************
DRAGRACE_INT_ISR:
		FARCALL	DETERM_INT_SOURCE
  		BTFSS	STATUS,	Z
		CALLW
	RETURN	;	NOT A COMPARATOR OR TIMER1 INTERRUPT
	
; COMPARATOR 2 INTERRUPT 
SERV_C2IF
		BANKSEL	CMOUT
		BTFSC	CMOUT,	MC2OUT		;C2OUT
		FARCALL	FALL_EDGE_2
		FARCALL	RISE_EDGE_2
	RETURN
	
FALL_EDGE_2
	m_If_FeqL	APP_STATUS_REG,	DR_STAGE_0	ELSE1
		MOVLW	DR_STAGE_1
		FARCALL	SETSTAGE
ELSE1
	m_If_FeqL	APP_STATUS_REG,	DR_STAGE_3	ELSE2
		movlw	DR_STAGE_5		;	ERROR CONDITION
		FARCALL	SETSTAGE
		GOTO	DR_STATUS_ERR_HANDLER
		m_IMMEDIATE_NRF_XT	RACE_IN_PROCESS
	
			
RISE_EDGE_2
	BANKSEL	TMR1L
	MOVFW	TMR1L
	MOVWF	CONFIG_ARG_1
	BANKSEL	TMR1H
	MOVFW	TMR1H
	MOVWF	CONFIG_ARG_2
	

SERV_C1IF
		BANKSEL	PORTA
		BTFSC	PORTA,	RA4		;C1OUT
		GOTO	FALL_EDGE_1

FALL_EDGE_1
	BANKSEL	APP_STATUS_REG
	movlw	DR_STATE_0
	XORWF	APP_STATUS_REG,	W
	BTFSS	STATUS,	Z
	GOTO	DR_INT_STATUS_err
	m_IMMEDIATE_NRF_XT	RACE_IN_PROCESS
	BANKSEL	T1CON
	BCF		T1CON,	TMR1ON		;
	BANKSEL	TMR1H
	CLRF	TMR1H
	BANKSEL	TMR1L
	CLRF	TMR1L
	
	BANKSEL	T1GCON
	BCF		T1GCON,	T1GSS0
	BANKSEL	T1CON
	BSF		T1CON,	TMR1ON		;
	
			
RISE_EDGE_1
	BANKSEL	TMR1L
	MOVFW	TMR1L
	MOVWF	CONFIG_ARG_1
	BANKSEL	TMR1H
	MOVFW	TMR1H
	MOVWF	CONFIG_ARG_1

SERV_TMR1IF


DR_INT_STATUS_err
DR_STATUS_ERR_HANDLER
	m_IMMEDIATE_NRF_XT	UNHANDLED_ERR
	CALL	DR_RESET
 	RETURN

DR_RESET
RETURN
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
;		DRAGRACE ROUTINE
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

DRAGRACE_ROUTINE
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
;	SET UP FOR DRAGRACE TIMER APP, UNLESS IT'S JUST THE DRAGRACE RECEIVER,
;		IN WHICH CASE IT'S JUST A BigZ.
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
	m_EEPROM_READ	DRAGSTER_FLAGS
	BANKSEL			APPLICATION_FLAGS
	movwf			APPLICATION_FLAGS
	XORLW			DRAGSTER_RCVR
	SKIP_IF_ZERO
	GOTO			NOT_JUST_A_BigZ
	MOVLW			BigZ_APP				;	THAT'S ALL THE RECEIVER WILL BE, AT LEAST FOR NOW
	
NOT_JUST_A_BigZ
	PAGESEL			SETUP_FOR_DRAGRACE
	CALL			SETUP_FOR_DRAGRACE
	PAGESEL			$
	MOVLW			DR_STATE_0
	MOVWF			BKLS_REG3
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
;	SET UP INTERRUPT VECTOR FOR DRAGRACE APP
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
	MOVLW	DRAGRACE_ISR
	BANKSEL	APP_INT_ISR_VECTOR
	MOVWF	APP_INT_ISR_VECTOR	; MAKE DRAGRACE ISR THE INTERRUPT VECTOR
;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

	m_IMMEDIATE_NRF_XT	Start_Your_Engines

DRAGRACE_LOOP

GOTO	DRAGRACE_LOOP
RETURN
;--------------------------------------------------------------------------
;-----------END OF DRAGRACE_ROUTINE----------------------------------------
;--------------------------------------------------------------------------

SETSTAGE
	ANDLW	0X07
	BRW
	GOTO	SET_STAGE_0
	GOTO	SET_STAGE_1
	GOTO	SET_STAGE_2
	GOTO	SET_STAGE_3
	GOTO	SET_STAGE_4
	GOTO	SET_STAGE_5
	GOTO	SET_STAGE_6
	GOTO	SET_STAGE_7
	
SET_STAGE_0
	CALL	CLR_TMR1
	CALL	RESET_TMR4
	
SET_STAGE_1

SET_STAGE_2
SET_STAGE_3
SET_STAGE_4
SET_STAGE_5
SET_STAGE_6
SET_STAGE_7

;**********************************************************************************
;	FUNCTION:	SETUP_FOR_DRAGRACE
;	DESC:	SETS UP THE COMPARATORS, TIMERS, AND INTERRUPTS NEEDED
;			IN THE DRAGRACE TIMER APP.
;**********************************************************************************
SETUP_FOR_DRAGRACE

	CALL	DRAG_RACE_INTERRUPTS_OFF	; TIL THINGS ARE SET UP
;	SETUP Comparators
	FARCALL	COMP1_ENABLE
	FARCALL	COMP2_ENABLE
	CALL	INIT_DRAGRACE_TMR1
	CALL	INIT_DRAGRACE_TMR4
	CALL	DRAG_RACE_INTERRUPTS_ON
	RETURN
	
;**********************************************************************************
;	FUNCTION:	DRAG_RACE_INTERRUPTS_OFF
;		DESC:	CLEAR/DISABLE INTERRUPTS
;		
;**********************************************************************************
;GLOBAL DRAG_RACE_INTERRUPTS_OFF
DRAG_RACE_INTERRUPTS_OFF
;	CLEAR/DISABLE COMPARATOR INTERRUPS	
	BANKSEL	PIR2
	BCF		PIR2,	C1IF
	BCF		PIR2,	C2IF
	BANKSEL	PIE2
	BCF		PIE2,	C1IE
	BCF		PIE2,	C2IE
	
;	CLEAR/DISABLE TIMER1 INTERRUPS		
	BANKSEL	PIE1
	BCF		PIE1,	TMR1GIE
	BANKSEL	PIR1
	BCF		PIR1,	TMR1GIF
 RETURN
 
;**********************************************************************************
;	FUNCTION:	DRAG_RACE_INTERRUPTS_ON
;		DESC:	ENABLE INTERRUPTS
;		
;**********************************************************************************
GLOBAL DRAG_RACE_INTERRUPTS_ON
DRAG_RACE_INTERRUPTS_ON:
;	ENABLE COMPARATOR INTERRUPS	
	BANKSEL	PIR2
	BCF		PIR2,	C1IF
	BCF		PIR2,	C2IF
	BANKSEL	PIE2
	BSF		PIE2,	C1IE
	BSF		PIE2,	C2IE
	
;	ENABLE TIMER1 INTERRUPS		
	BANKSEL	PIR1
	BCF		PIR1,	TMR1IF
	BANKSEL	PIE1
	BSF		PIE1,	TMR1IE
 RETURN
 
;**********************************************************************************
;	FUNCTION:	INIT_DRAGRACE_TMR1
;	DESC:	SETUP ROUTINE FOR TIMER 1
;	WITH A XTAL OF 16MHz
;	PUTTING TMR1CS<1:0> = 00 DIVIDES THE 16MHz BY 4, 
;	PUTTING T1CKPS<1:0> = 11 DIVIDE THE 4MHz BY 4, GIVES A 1MHz COUNT RATE
;	
;**********************************************************************************
 INIT_DRAGRACE_TMR1
 	BANKSEL	T1CON
	BCF	T1CON,	TMR1CS0	;'0' SELECTS FOSC/4 Internal Clock
	BCF	T1CON,	TMR1CS1	;
	; SET PRESCALE. 00=1:1, 01=1:2,10=1:4,11=1:8
	BSF		T1CON,	T1CKPS1		; T1CKPS<1>....WHEN SET, PRESCALE CLOCK IS 1:8 OR 1:4\
	BSF		T1CON,	T1CKPS0		; T1CKPS<0>....WHEN SET, PRESCALE CLOCK IS 1:2 OR 1:8>\ b'11 = 1:8
	BCF		T1CON,	NOT_T1SYNC	; 1 = Do not synchronize external clock input
								; 0 = Synchronize external clock input with system clock (FOSC)
	BANKSEL	T1GCON
	MOVLW	sync_C1OUT	
	IORLW	T1GCON
	BSF		T1GCON,	TMR1GE
	BCF		T1GCON,	T1GPOL
	BCF		T1GCON,	T1GTM
	BCF		T1GCON,	T1GSPM
	BSF		T1GCON,	T1GSS0
	BSF		T1GCON,	T1GSS1	
  	CALL	RESET_TMR1
	BSF		T1CON,	TMR1ON		; 1 = TURN TMR1 ON
  RETURN
  
;**********************************************************************************
;	FUNCTION:	SET_UP_DRAGRACE_TMR4
;	DESC:	SETUP ROUTINE FOR TIMER 4
;	
;**********************************************************************************
 INIT_DRAGRACE_TMR4
	CALL	TMR4_OFF
	BCF	T4CON,	T4OUTPS0	;'0' SELECTS FOSC/4 Internal Clock
	BCF	T4CON,	T4OUTPS1	;
	; SET PRESCALE. 00=1:1, 01=1:2,10=1:4,11=1:8
	BSF	T4CON,	T4CKPS1		; T1CKPS<1>....WHEN SET, PRESCALE CLOCK IS 1:8 OR 1:4\
	BSF	T4CON,	T4CKPS0		; T1CKPS<0>....WHEN SET, PRESCALE CLOCK IS 1:2 OR 1:8>\ b'11 = 1:8
	BSF	T4CON,	NOT_T1SYNC	; 1 = Do not synchronize external clock input
  RETURN
  
  CLR_TMR1
	BANKSEL	TMR1H
	CLRF	TMR1H
	BANKSEL	TMR1L
	CLRF	TMR1L  
  RETURN
  
TMR4_OFF  
 	BANKSEL	T4CON
	BCF	T4CON,	TMR4ON
RETURN

#ENDIF