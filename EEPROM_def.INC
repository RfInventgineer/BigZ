
#IFNDEF	EEPROM_DEF
#define	EEPROM_DEF

;****************************************************************
;	EEPROM REGISTER MAPPING:
; EEPROM REGISTERS LOCATIONS FOR THE NRF DEFAULT, POWER-UP SETTINGS
;****************************************************************

#define EE_CONFIG      0x00		; 
#define EE_EN_AA       0x01
#define EE_EN_RXADDR   0x02
#define EE_SETUP_AW    0x03
#define EE_SETUP_RETR  0x04
#define EE_RF_CH       0x05
#define EE_RF_SETUP    0x06

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;NUMBER OF BYTES IN RX PAYLOAD IN DATA PIPES. 1 TO 32 BYTES.
;	0 = PIPE NOT USED
;	1 = 1 BYTE...
;	...32 = 32 BYTES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define EE_LOC_RX_PW_P0	0x07
#define EE_LOC_PW_P1	0x08
#define EE_LOC_PW_P2	0x09
#define EE_LOC_PW_P3	0x0A
#define EE_LOC_PW_P4	0x0B
#define EE_LOC_PW_P5	0x0C
#define EE_DYNPD		0X0D
#define EE_FEATURE		0X0E


#define EE_TX_ADD_0		0x10	;R/W TRANSMIT ADDRESS. USED FOR A PTX DEVICE ONLY.
#define EE_TX_ADD_1		0x11	;(LSBYTE IS WRITTEN FIRST) SET RX_ADDR_P0 EQUAL TO 
#define EE_TX_ADD_2		0x12	; THIS ADDRESS TO HANDLE AUTOMATIC ACKNOWLEDGE IF THIS 
#define EE_TX_ADD_3		0x13	;IS A PTX DEVICE WITH ENHANCED SHOCKBURST™ ENABLED
#define EE_TX_ADD_4		0x14

#define EE_RXP0_ADD_0	EE_TX_ADD_0	; RECEIVE PIPE 0 HAS A 5-BYTE ADDRESS.
#define EE_RXP0_ADD_1	EE_TX_ADD_1	; RECEIVE PIPE 0 ADDRESS BYTES 1-4
#define EE_RXP0_ADD_2	EE_TX_ADD_2	;
#define EE_RXP0_ADD_3	EE_TX_ADD_3	;
#define EE_RXP0_ADD_4	EE_TX_ADD_4	;

#define EE_RXP1_ADD_0	0x15	; RECEIVE PIPES 1-5 HAVE A 5 BYTE ADDRESS TOO.
#define EE_RXP2_ADD_0	0x16	; BYTE 0 FOR EACH PIPE IS UNIQUE,
#define EE_RXP3_ADD_0	0x17	; 
#define EE_RXP4_ADD_0	0x18	;
#define EE_RXP5_ADD_0	0x19	;


#define EE_RXPx_ADD_1	0x1A	; BUT FOR ADDRESS BYTES 1-4, THEY ALL SHARE
#define EE_RXPx_ADD_2	0x1B	; THE SAME VALUES.
#define EE_RXPx_ADD_3	0x1C	;
#define EE_RXPx_ADD_4	0x1D	;

; Addresses for CONFIG PARAMATERS FOR THE PIC, STORED IN LOCATIONS 0X30-0X3F
#DEFINE SPBRGL_EE			0X30
#DEFINE SPBRGH_EE			0X31
#DEFINE OP_MODE_0_EE		0X32

#DEFINE FHOP_MODE_FAC_DEF	0X0		; 0x0 = No Frequency Hopping
#DEFINE FHOP_MODE_EE		0X33	; Address it's stored in
#DEFINE OP_MODE_2_EE		0X34	; 
#DEFINE MISC_FLAGS_EE		0X35	

#DEFINE	APP_OPERATION		0X40	; Addreess for APP_OPERATION_VAL, either
									;BigZ_APP, Dragrace_Tmr,LCR_METER, or Android_led_cont
#DEFINE	DRAGSTER_FLAGS		0X41	; Address ...
#DEFINE	LCR_FLAGS			0X42	; Address ...
#DEFINE BIGZ_FLAGS			0X43	; Address ...
#DEFINE AndLedContFlags 	0x44	; Address ...


;
; THE EEPROM_STATUS REGISTER INDICATES WHETHER THE EEPROM IS UNINITIALIZED, FACTORY RESET, OR USER-CUSTOMIZED	
#DEFINE EEPROM_STATUS	0XFF
#DEFINE EE_FW_VERSION_LOC	0XD0
;
;EEPROM_STATUS BITS:
#DEFINE EE_BLANK			0
#DEFINE EE_INITIALIZED_N	1
#DEFINE EE_CUSTOMIZED_N		2
;****************************************************************
EEPROM_STATUS_VAL	SET	0X02
;****************************************************************
#DEFINE	EEPROM_START_ADD			0X0F000
#DEFINE	FACTORY_RESET_TABLE_START	0X80
#DEFINE	FACTORY_RESET_TABLE_END		0XAF
;**************************************************************************

#DEFINE SPBRGL_VAL			d'69'	; DEFAULT VALUES, POVIDES BAUD RATE OF 57600
#DEFINE SPBRGH_VAL			d'0'	;/


	; OP_MODE_VAL BITS: 
	EN_FUNC_0		EQU		B'1'		;	EN_FUNC_3:0 MAP to project-specific enhanced functions. 
	EN_FUNC_1		EQU		B'10'		;		Current values are: b'0000:Basic Zigbee
	EN_FUNC_2		EQU		B'100'		;							b'0001: LCR Meter,	
	EN_FUNC_3		EQU		B'1000'		;					;		b'0010: Chronometer, 
	;																b'0011: Android led panel controller
	
	CAN_CONFIG		EQU		B'10000'	; 	0 = CANT CONFIGURE FROM NORMAL OPERATION. 1 = CONFIGURABLE AT STARTUP
	COMPARATOR1_ON	EQU		B'100000'	;	1 = enable comparator2, 0 = no action needed
	COMPARATOR2_ON	EQU		B'1000000'	;	1 = enable comparator2, 0 = no action needed
	TIMER_1_EN		EQU		B'10000000'	;	1 = enable timer1, 0 = no action needed

	OP_MODE_0_VAL	EQU CAN_CONFIG+EN_FUNC_0+EN_FUNC_1+EN_FUNC_2+COMPARATOR1_ON+COMPARATOR2_ON
	
	#DEFINE META_DATA_MODE_2_VAL	0X0
	#DEFINE MISC_FLAGS_VAL			0X03	;	MISC_FLAGS_VAL BITS: Bit 0: Is configuarable at Power Up, 
											;	Bit 1: Enters CONFIG Mode if ? Pin is held low at Power up, 
											;	Bits 2-7: Reserved
;	DRAGSTER_FLAGS BITS
	DRAGSTER_TMR_1	EQU	0X01
	DRAGSTER_TMR_2	EQU	0X02
	DRAGSTER_RCVR	EQU	0X03
	USE_LEAD_PULSE	EQU	0X4
	USE_TRAIL_PULSE	EQU	0X8
	USE_MID_PULSE	EQU	0X0C

	
;	LCR METER FLAGS	
	LCR_USE_ANALOG_SW	EQU	0X01
	
	DRAGSTER_FLAGS_VAL	EQU		USE_LEAD_PULSE + DRAGSTER_TMR_1
	LCR_FLAGS_VAL		EQU		0X01
	BIGZ_FLAGS_VAL		EQU		0X00
#endif