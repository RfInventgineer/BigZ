;**********************************************************************
;    FILENAME:	    main.ASM                                          *
;    FILE VERSION:                                                    *
;                                                                     *
;    AUTHOR: LYLE LARSON                                              *
;    COMPANY:                                               		  *
;**********************************************************************
;**********************************************************************
	LIST   P=PIC16LF1933
	#INCLUDE	<P16LF1933.INC>	; PROCESSOR SPECIFIC VARIABLE DEFINITIONS

	LIST
	errorlevel 0 
	errorlevel -205
	errorlevel -207
	errorlevel -302
	errorlevel -305
;	errorlevel -203

;********************IMPORTANT!!!!!!!!!!!!!******************************
;*************************************************************************
;	ADJUSTABLE USER VARIABLES FOR TESTING AND DEVELOPMENT
;*************************************************************************



#DEFINE	TIMESTAMP $(NOW())
; IF DEBUGGING, CONTROLL BEHAVIOUR HERE
#IFDEF __DEBUG
;		#DEFINE	STATISTICS
#ENDIF

;*************************************************************************
	#Define BigZ_APP 			0x00
	#Define LCR_METER			0x01
	#Define Dragrace_Tmr		0x02
	#Define Android_led_cont	0x03
	
;	INITIAL PROGRAMMED  SETTING. ChoosE one of the following, and comment the rest.
	APP_OPERATION_VAL	SET		BigZ_APP
;	APP_OPERATION_VAL	SET		Dragrace_Tmr
;	APP_OPERATION_VAL	SET		LCR_METER
;	APP_OPERATION_VAL	SET		Android_led_cont
;*************************************************************************

;	#DEFINE DEBUG
	#DEFINE VERIFY_EEPROM_WRITES
	#DEFINE TX_INDICAT_LIGHT	RA5
	#DEFINE	VERBOSE
;	#DEFINE	SUPERVERBOSE
	#DEFINE	REPORT_TX_SUCCESS
;	#DEFINE	REPORT_DATA_RECEIVED
	#DEFINE	REPORT_MAX_RT
	#DEFINE	REPORT_NRF_STATUS
;	#DEFINE	REPORT_SPI_STATUS_BIT_ERROR
	#DEFINE	REPORT_RX_FIFO_OVERFLOW
	#DEFINE	REPORT_TX_FIFO_OVERFLOW
	#DEFINE BAUDRATE d'56'
;	#define INTOSCTEST
;	#DEFINE BAUDRATE115
;	#DEFINE BAUDRATE192
;#ENDIF

;#IFNDEF __DEBUG
;	#DEFINE DEBUG
;	#DEFINE	VERBOSE
;	#DEFINE	SUPERVERBOSE

;	#DEFINE	REPORT_MAX_RT
;	#DEFINE	REPORT_SPI_STATUS_BIT_ERROR
;	#DEFINE	REPORT_RX_FIFO_OVERFLOW
;	#DEFINE	REPORT_TX_FIFO_OVERFLOW
;	#DEFINE	REPORT_TX_SUCCESS
;	#DEFINE	BZ_CONFIGURABLE	
;
;	#DEFINE VERIFY_EEPROM_WRITES
;	#DEFINE TX_INDICAT_LIGHT	RA5
;	#DEFINE	VERBOSE
;	#DEFINE	SUPERVERBOSE
;	#DEFINE	REPORT_TX_SUCCESS
;	#DEFINE	REPORT_MAX_RT
;	#DEFINE	REPORT_NRF_STATUS
;	#DEFINE	REPORT_SPI_STATUS_BIT_ERROR
;	#DEFINE	REPORT_RX_FIFO_OVERFLOW
;	#DEFINE	REPORT_TX_FIFO_OVERFLOW
;	;#DEFINE	XMIT_HEARTBEAT
;	;#DEFINE	REPORT_DATA_RECEIVED
;	;#DEFINE	REPORT_NRF_STATUS
;	;#DEFINE BAUDRATEEE
;	;#DEFINE DEBUG_READ_ALL_NRF	; MONITORS ALL REGISTERS IN THE NRF CHIP
;	;#DEFINE REPORT_PORTS_A_B_D
;	;#DEFINE REPORT_PORT_A
;	;#DEFINE REPORT_PORT_B
;	;#DEFINE REPORT_PORT_C
;#ENDIF
;
	
 __CONFIG _CONFIG1, _FOSC_HS & _WDTE_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
 __CONFIG _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_ON & _BORV_19 & _LVP_OFF

	
	
;***** VARIABLE DEFINITIONS

#DEFINE DYNAMIC_PAYLOAD_LENGTH;	USE A DYNAMIC PAYLOAD LENGTH

#define CONFIG_KEY_LENGTH 0xC

;------^^^^^24 chars long^^^^^^^^^^^^^^^^^^^^
#define TX_FAIL_MESSAGE_LEN .24

XTAL_FREQ		EQU		D'16000000'	; EXTERNAL OSC freq in Hz
HI              SET     1                       ;
LO              SET     0                       ;
ACTIVETOIDLE    SET     1                       ;
IDLETOACTIVE    SET     0                       ;
MIDS            SET     0                       ;
ENDS            SET     1                       ;


		global	RF24_COMMAND
		global	NRF_STORE_REG_ADD	
		global	DATA_SIZE			; PLACE HOLDER TO INSURE THAT TX_FIFO DOESNT OVERFLOW
		global	NRF_STATE_REG		; FLAGS FOR STATUS OF THE CURRENT TRANSMITTOR STATE
		global	TABLE_POINTER			
		global	REGVAL			
		global	ACTIVE_PIPE 	

		global	NRF_CONFIG			;CONFIGURATION REGISTER
		global	NRF_EN_AA 			;ENHANCED SHOCKBURST™, ENABLE ‘AUTO ACKNOWLEDGMENT’
		global	NRF_EN_RXADDR 		;ENABLED RX ADDRESSES
		global	NRF_SETUP_AW 		;SETUP OF ADDRESS WIDTHS(COMMON FOR ALL DATA PIPES)
		global	NRF_SETUP_RETR 		;SETUP OF AUTOMATIC RETRANSMISSION
		global	NRF_RF_CH 			;RF CHANNEL	
		global	NRF_RF_SETUP 		;RF SETUP REGISTER
		global	NRF_STATUS 			;STATUS REGISTER
		global	NRF_OBSERVE_TX 		; OBSERVE_TX Transmit observe register:
									;		PLOS_CNT 7:4 0 R Count lost packets. The counter 
									;		is overflow protected
									;		to 15, and discontinues at max until reset.
									;	The counter is reset by writing to RF_CH.
									;		ARC_CNT 3:0 0 R Count retransmitted packets. The 
									;		counter is reset when transmission of a new packet 
									;		starts.
									
		global	NRF_RPD				;RECEIVED POWER DETECT (BIT 0)
		global	NRF_RX_ADDR_P0 	
		global	NRF_RX_ADDR_P1 	
		global	NRF_RX_ADDR_P2 	
		global	NRF_RX_ADDR_P3 	
		global	NRF_RX_ADDR_P4 	
		global	NRF_RX_ADDR_P5 	
		global	NRF_TX_ADDR   	
		global	NRF_RX_PW_P0 	
		global	NRF_RX_PW_P1 	
		global	NRF_RX_PW_P2 	
		global	NRF_RX_PW_P3 	
		global	NRF_RX_PW_P4 	
		global	NRF_RX_PW_P5
		
		global	DELAYREGA		
		global	DELAYREGB		
		global	COUNT_REG		
		global	JUNK_STORAGE		

		global	NRF_FIFO_STATUS 
		global	NRF_DYNPD 		
		global	NRF_FEATURE 	
		global	BYTE_COUNT_TO_NRF
		global	BYTE_COUNT_FROM_NRF
		GLOBAL	BYTE_COUNT_FROM_UART
		GLOBAL	BYTE_COUNT_TO_UART
		GLOBAL	UART_OFLO_ERR_COUNT
		GLOBAL	UART_FRM_ERR_COUNT
		GLOBAL	COMMENCE_XMIT_COUNTER	
		GLOBAL	TX_DS_INT_COUNTER		
		GLOBAL	NRF_INT_COUNTER			
		GLOBAL	TX_IN_PROGRESS_CLEARED	
		GLOBAL	TX_IN_PROGRESS_SET		
		GLOBAL	LAST_NRF_INDEX
		GLOBAL	APPLICATION_FLAGS			
		GLOBAL	APP_INT_ISR_VECTOR
		GLOBAL	TMR1_HH
;
		MAINASM30	UDATA  0X20
		RF24_COMMAND			RES 1
		NRF_STORE_REG_ADD		RES 1
		DATA_SIZE				RES	1		
		NRF_STATE_REG			RES	1
		TABLE_POINTER			RES 1
		REGVAL					RES 1
		ACTIVE_PIPE				RES	1		
		UART_OFLO_ERR_COUNT		RES	2
		UART_FRM_ERR_COUNT		RES	2
		CFGKEY_CHR_CNT			RES	1
		
;********************************************************************************************************************************************
; THE FOLLOWING NRF* VARIABLES ARE DEDICATED PLACEHOLDERS IN THE PIC THAT REFLECT THE LAST READ OF THE CORRESPONDING REGISTERS IN THE NRF24L01+.
;	They are arranged in order of their addresses on the NRF IC so that the FSR can be used to store the values of the registers every time
;	they're read.
;********************************************************************************************************************************************
NRF_SHADOW_REGS		UDATA	30
		NRF_CONFIG		RES 1		;00 CONFIGURATION REGISTER
		NRF_EN_AA 		RES 1		;01 ENHANCED SHOCKBURST™, ENABLE ‘AUTO ACKNOWLEDGMENT’
		NRF_EN_RXADDR 	RES 1		;02 ENABLED RX ADDRESSES
		NRF_SETUP_AW	RES 1 		;03 SETUP OF ADDRESS WIDTHS(COMMON FOR ALL DATA PIPES)
		NRF_SETUP_RETR	RES 1 		;04 SETUP OF AUTOMATIC RETRANSMISSION
		NRF_RF_CH 		RES 1		;05 RF CHANNEL	
		NRF_RF_SETUP 	RES 1		;06 RF SETUP REGISTER
		NRF_STATUS 		RES 1		;07 STATUS REGISTER (UPDATES EVERY TIME COMMUNICATION IS INITIATED, IMMEDIATELY AFTER THE FIRST SPI_TRANSFER)
		NRF_OBSERVE_TX 	RES 1		;08 TRANSMIT OBSERVE REGISTER (MONITORS NUMBER OF RE-TRANSMITS, BITS 3:0, AND COUNTS LOST PACKETS, BITS 7:4)
		NRF_RPD			RES 1		;09 Received Power Detector (BIT 0, INDICATES POWER LEVEL RECEIVED IS ABOVE(1) OR BELOW (0) -65DbM.)
		NRF_RX_ADDR_P0 	RES	5		;0A RX_ADDR_P0 39:0 0xE7E7E7E7E7 R/W Receive address data pipe 0.
		NRF_RX_ADDR_P1 	RES	5		;0B RX_ADDR_P1 39:0 0xC2C2C2C2C2 R/W Receive address data pipe 1
		NRF_RX_ADDR_P2 	RES 1		;0C RX_ADDR_P2 7:0 0xC3 R/W Receive address data pipe 2. Only LSB.MSBytes are equal to RX_ADDR_P1[39:8]
		NRF_RX_ADDR_P3 	RES 1		;0D RX_ADDR_P3 7:0 0xC4 R/W Receive address data pipe 3. Only LSB. MSBytesare equal to RX_ADDR_P1[39:8]
		NRF_RX_ADDR_P4 	RES 1		;0E RX_ADDR_P3 7:0 0xC5 R/W Receive address data pipe 4. Only LSB. MSBytesare equal to RX_ADDR_P1[39:8]
		NRF_RX_ADDR_P5 	RES 1		;0F RX_ADDR_P3 7:0 0xC6 R/W Receive address data pipe 5. Only LSB. MSBytesare equal to RX_ADDR_P1[39:8]
		NRF_TX_ADDR   	RES 5		;10 TX_ADDR 39:0 0xE7E7E7E7E7 R/W Transmit address. Used for a PTX device only. (LSByte is written first)
		NRF_RX_PW_P0 	RES	1		;11 RX_PW_P0.  RX_PW_P0 5:0 R/W Number of bytes in RX payload in data pipe 0 (1 to 32 bytes).0 Pipe not used
									;	1 = 1 byte … D'32' = 32 bytes
		NRF_RX_PW_P1 	RES 1		;12 RX_PW_P1. BITS 5:0 Number of bytes in RX payload in data pipe 1.
		NRF_RX_PW_P2 	RES 1		;13 LIKE NRF_RX_PW_P1, BUT FOR PIPE 2
		NRF_RX_PW_P3 	RES 1		;14 LIKE NRF_RX_PW_P1, BUT FOR PIPE 3		
		NRF_RX_PW_P4 	RES 1 		;15 LIKE NRF_RX_PW_P1, BUT FOR PIPE 4
		NRF_RX_PW_P5 	RES 1 		;16 LIKE NRF_RX_PW_P1, BUT FOR PIPE 5
		NRF_FIFO_STATUS RES 1		;17 FIFO_STATUS FIFO Status Register	

		DELAYREGA		RES 1		;18, 19,1A, and 1B. These can be used as temporary registers.
		DELAYREGB		RES 1
		COUNT_REG		RES	1
		JUNK_STORAGE	RES 1
		NRF_DYNPD 		RES 1		;1C DYNPD Enable dynamic payload length
		NRF_FEATURE 	RES	1		;1D FEATURE R/W Feature Register		
		
		BYTE_COUNT_TO_NRF		RES		2
		BYTE_COUNT_FROM_NRF		RES		2
		BYTE_COUNT_FROM_UART	RES		2
		BYTE_COUNT_TO_UART		RES		2
		COMMENCE_XMIT_COUNTER	RES		2
		TX_DS_INT_COUNTER		RES		2
		NRF_INT_COUNTER			RES		2
		TX_IN_PROGRESS_CLEARED	RES		2
		TX_IN_PROGRESS_SET		RES		2
		LAST_NRF_INDEX			RES		1
		APPLICATION_FLAGS		RES		1
		TMR1_HH					RES		1

		global	NTOU_BUFFER      
		GLOBAL	NUUNBUFSTAT     
		GLOBAL	NTOU_BUF_DATA_CNT  
		GLOBAL	NTOU_BUF_WT_PTR    
		GLOBAL	NTOU_BUF_RD_PTR                         
		GLOBAL	UTON_BUFFER      
		GLOBAL	UTON_BUF_DATA_CNT  
		GLOBAL	UTON_BUF_WT_PTR    
		GLOBAL	UTON_BUF_RD_PTR    
		GLOBAL	TEMP1
		GLOBAL	temp3


		global	TBL_START_PTR
		global	TABLE_POSITION_PTR
		global	BK_INDEX_REG
		global	BKLS_REG1
		global	BKLS_REG2
		global	BKLS_REG3
		global	BL_REG_PCLATH
		global	CONFIG_STATUS_REG
		global	CONFIG_COMMAND_REG
		global	CONFIG_ARG_1
		global	CONFIG_ARG_2
		global	APP_STATUS_REG
		global	APPLICATION_TEMP

BANKLESS_REGISTERS	UDATA_shr	0X71
	BKLS_REG3				RES	1
	BKLS_REG1				RES	1
	BKLS_REG2				RES	1
	CONFIG_ARG_1			RES	1
	CONFIG_ARG_2			RES	1
	CONFIG_COMMAND_REG		RES	1
	BK_INDEX_REG			RES	1
	BL_REG_PCLATH			RES	1
	TBL_START_PTR			RES	1
	TABLE_POSITION_PTR		RES	1
	PCLATH_TEMP				RES	1		; VARIABLE USED FOR CONTEXT SAVING
	APP_STATUS_REG			RES	1		; USED FOR DRAGRACE STATUS, OTHER USES POSSIBLE
	APPLICATION_TEMP		RES	1		; TEMP PLACEHOLDER
	APP_INT_ISR_VECTOR		RES	1		; DETERMINES THE ISR VECTOR, SPECIFIC TO EACH APP
	CONFIG_STATUS_REG		RES	1

OP_MODE_STAT_CONFIG				EQU	0		;	1 = CONFIG MODE, 0 = NORMAL MODE
OP_MODE_STAT_NRF_SETUP_CHG_REQ	EQU	1	; SET IF ANY OF THE NRF PARAMATERS ARE TO BE ALTERED
OP_MODE_STAT_NRF_SETUP_CHG_COMP	EQU	2	; SET IF ANY OF THE NRF PARAMATERS ARE TO BE ALTERED

REGISTER_COUNT	equ	0X17

;***********************************************;
;SPIMPOLERROR BITS                              ;
;***********************************************;
SPIMErrWriteCollision   EQU     0x01             ;WRITE COLLISION HAS OCCURED
SPIMDATANOTREADY        EQU     0x02             ;DATA NOT YET READY
SPIMTRANSMITNOTOVER     EQU     0x02             ;DATA NOT YET FULLY TRANSMITTED
                                              ;
;***********************************************;

;************************************************************************
;************************************************************************
; 
#IFNDEF CLOCK_FREQ
#define CLOCK_FREQ      D'16000000'
#ENDIF
; 
;#define UARTINT_TXON
;#define UARTINT_RXON
#define NTOU_BUFFER_SIZE .32
#define UTON_BUFFER_SIZE .32
; 

;#DEFINE BRGH_VAL_HI          

N2UBuffers	    UDATA	0xA1
NUUNBUFSTAT	    	RES      1					;Used to store status flags
NTOU_BUFFER	    	RES     NTOU_BUFFER_SIZE    ;Transmit Data FIFO buffer
NTOU_BUF_DATA_CNT   RES     01					;Number of Bytes in Buffer
NTOU_BUF_WT_PTR     RES     01          	;Transmit Buffer Write Position
NTOU_BUF_RD_PTR     RES     01          	;Transmit Buffer Read Position

UTON_BUFFER        	RES     UTON_BUFFER_SIZE ;Receive Data FIFO buffer
UTON_BUF_DATA_CNT   RES     01				;Number of Bytes in Buffer          	
UTON_BUF_WT_PTR     RES     01				;Transmit Buffer Write Position
UTON_BUF_RD_PTR     RES     01				;Transmit Buffer Read Position

;----- NUUNBUFSTAT Bits ---------------------------------------
U2NBF            EQU  H'0'			;For Receive Buffer Empty Flag in
U2NBE            EQU  H'1'			;For Data Receive Error Flag in
U2NERR           EQU  H'2'			;For Data Buffer over flow indiaction
U2NBOF           EQU  H'3'			;bit. It indicates that Data received
N2UBF            EQU  H'4'			;when Rxbuffer was full. So it shows
N2UBE            EQU  H'5'			;that in between data is missing.
N2UERR           EQU  H'6'			;For N2U Buffer Full Flag in
N2UBOF           EQU  H'7'			;For N2U Buffer Empty Flag in
									;For N2U Data Receive Error Flag in
									;For N2U Buffer over flow indiaction
									;bit. It indicates that Data received
									;when Rxbuffer was full. So it shows
									;that in between data is missing.       
				                   	
;************************************************************************
;For Receive Buffer Full Flag in             

UDATA_OVR

;This data is temporarily only. It can be kept in over-writable RAM.
;But in that case linker will define same RAM address for other
;parameters. As these are used in ISR (Interrupt Service Routine),
;If interrupt occurs while other parameter is used then one will loose
;value of that parameter. So preferably keep it in UDATA only unless you
;are sure that ISR will not be invoked at the time of use of that
;parameters
TEMP1           RES     01
temp3           RES     01
ISR_FSR0L   	RES     01      ;For saving FSR0L value
HEX2UARTREG		RES		04

;************************************************************************
;************************************************************************
;************************************************************************

#IFNDEF 	nrf24L01_DEF
#INCLUDE 	nrf24L01_DEF.INC
#ENDIF
					      					      
#IFNDEF		MACROS
#INCLUDE 	MACROS.INC
#ENDIF
;
RST	CODE	0X0
;**********************************************************************
;	errorlevel 2 ;+302 ; Enable banking message
;**********************************************************************
;	ORG  0X000             ; PROCESSOR RESET VECTOR
	GOTO SETUP 
;**********************************************************************
ISR	CODE	0X004
;**********************************************************************
MAIN_ISR
	MOVLP	MAIN_ISR
	MOVFW	APP_INT_ISR_VECTOR
	MOVWF	PCL	

;**********************************************************************

DRAGRACE_ISR
	FARCALL	DRAGRACE_INT_ISR
BigZ_ISR
NORMAL_OPERATION
	; Check UART. It is uncontrolled, and the NRF communication is fast.
	FARCALL    	UARTIntISR	 ; Call general purpose UART interrupt service routine

;**********************************************************************
;	CHECK TO SEE IF NORMAL OPERATION, OR IF THE uC IS, OR COULD GO INTO IN CONFIG MODE
;**********************************************************************
	BANKSEL		CONFIG_STATUS_REG
	BTFSC		CONFIG_STATUS_REG,	PIC_IN_NORMAL_MODE	; AT POWER-UP, THIS BIT STARTS AS LOW.
	GOTO		EXIT_INTERRUPT;	
;**********************************************************************
;	LINES BELOW SHOULD ONLY EXECUTE AFTER POWER-UP, 
;		UNTIL A NON-QUALIFYING BYTE IS RECEIVED FROM THE UART.
;**********************************************************************
	FARCALL	CONFIG_ISR_HANDLER		
	BANKSEL	CFGKEY_CHR_CNT			;	
	MOVFW	CFGKEY_CHR_CNT			;   IF THIS VALUE IS GREATER THAN
	SUBLW	CONFIG_KEY_LENGTH		;	CONFIG_COUNTDOWN		; UUU_BigZ_Config
 	BTFSC	STATUS,	C				;	
	GOTO	EXIT_INTERRUPT			;	EXIT_INTERRUPT
;**********************************************************************
EXIT_INTERRUPT					;
	RETFIE                    	; RETURN FROM INTERRUPT
;***********************************************; 
;	END OF ISR
;***********************************************; 

__SETUP	CODE	0X1C
;***********************************************; 
SETUP
;***********************************************;
;***********************************************;
;OSC_SETUP AND RUNNING
;***********************************************;
	BANKSEL	OSCCON
	MOVLW	0X70
#ifdef	INTOSCTEST
	MOVLW	0X7A
#endif
	MOVWF	OSCCON

#IFNDEF	__DEBUG
WAITFOREXTCLOCK
	BTFSS	OSCSTAT,	OSTS	;IS THE SYSTEM CLOCK RUNNING OFF OF SOURCE DEFINED BY FOSC<2:0> OF _CONFIG1?
	GOTO	WAITFOREXTCLOCK		;	NO, STILL RUNNEN FROM INTERNAL OSCILLATOR
CLOCK_RUNNING			;	YES. WE CAN PROCEED
#ENDIF
;***********************************************;

;***********************************************;
;	PORTA SETUP
;***********************************************;
	BANKSEL	TRISA
	MOVLW	0Xff
	MOVWF	TRISA		;RA0 thru RA5 can be outputs, down the road...
	BANKSEL	ANSELA
	CLRF	ANSELA
	BANKSEL PORTA
	CLRF	PORTA
	BSF		PORTA,	0
	
	
;***********************************************;
;	PORTB SETUP
;***********************************************;
	BANKSEL	TRISB
	BCF		TRISB,	0X1
	BANKSEL	ANSELB			; SET PORT B AS DIGITAL I/O
	MOVLW	0XFF
	MOVWF	ANSELB
	MOVLW	0X00
	MOVWF	ANSELB
	BANKSEL	OPTION_REG		; 
	BCF		OPTION_REG,7	; ENABLE WEAK PULLUPS ON PORTB
	BANKSEL	WPUB			; 
	MOVLW	0XF8			; PORTB 0,1,2 NO PULLUP
	MOVWF	WPUB
	BANKSEL	PORTB
	BSF		PORTB,1			; ASSERTS RXEN LINE ON THE RF FRONT END...1/7/16

;***********************************************;
;PORT C/SPI_SETUP/USART SETUP
;***********************************************;
	CE_LO
	BANKSEL	TRISC
	BCF		TRISC, 6			; USART TX
	BCF		TRISC, 5			; SDO OUTPUT
 	BCF		TRISC, 3			; SCK OUTPUT
	BCF		TRISC, 2			; /CS FOR SLAVE
	BCF		TRISC, 1			; SLAVE CHIP ENABLE
	FARCALL 	SPI_INIT
;***********************************************;
;TODO:
;	CLEAR ALL GPR'S
;	needS to be written for the '1933...
	FARCALL	CLEAR_ALL_GPRS
;***********************************************;
;***********************************************;
; CLEAR BUFFER CONENTS
	FARCALL	INIT_BUFFERS	
;***********************************************;
	FARCALL		UARTIntInit		
;***********************************************;
;***********************************************;
	banksel	CONFIG_STATUS_REG
	clrf	CONFIG_STATUS_REG
;***********************************************;

SKIP_USER_CONFIG
;***********************************************;
	FARCALL	NRF24L01_INIT

;	SET UP THE INTERRUPTS
;***********************************************;
	BANKSEL		OPTION_REG		
	BCF			OPTION_REG,	INTEDG	; SET TO INTERRUPT OF THE FALLING EDGE OF RB0
	BCF			INTCON,		INTF	 
;***********************************************;

	;MOVLP		NRF_ENTER_RCV_MODE	
	FARCALL		NRF_ENTER_RCV_MODE	; RETURN TO PRIMARY RECEIVER MODE

    GOTO	MAIN
;***********************************************;

__MAIN	CODE	0X80
;***********************************************;
MAIN
		BANKSEL			CFGKEY_CHR_CNT		
		CLRF			CFGKEY_CHR_CNT
			
		m_EEPROM_READ	APP_OPERATION
		XORLW			BigZ_APP
		SKIP_NOT_ZERO
		CALL			BigZ_ROUTINE
		XORLW			BigZ_APP
	
		XORLW			LCR_METER
		SKIP_NOT_ZERO
		CALL			LCR_METER_ROUTINE
		XORLW			LCR_METER
	
		XORLW			Android_led_cont
		SKIP_NOT_ZERO
		CALL			Android_led_cont_ROUTINE
		XORLW			Android_led_cont
	
		XORLW			Dragrace_Tmr
		SKIP_NOT_ZERO
		CALL			DRAGRACE_ROUTINE_LAUNCH
		XORLW			Dragrace_Tmr
	
		CLRF			TABLE_POSITION_PTR
		BANKSEL			LAST_NRF_INDEX
		CLRF			LAST_NRF_INDEX

	HALT
;***********************************************;
DRAGRACE_ROUTINE_LAUNCH
	FARCALL		DRAGRACE_ROUTINE
	HALT
;***********************************************;
BigZ_ROUTINE
		BSF			INTCON,		GIE		; ENABLE INTERRUPTS
	    bsf     	INTCON,		PEIE     ;Enable Peripheral Interrupt
	
	;	SET UP THE INTERRUPT VECTOR FOR A BigZ
	;#IFDEF	VERBOSE
		m_IMMEDIATE_PRINTF	IM_ALIVE
		FARCALL	READ_ALL_NRF
		FARCALL	PRINTF_ALL_NRF_REGISTERS
	;#ENDIF
	
	
		MOVLW	BigZ_ISR
		BANKSEL	APP_INT_ISR_VECTOR
		MOVWF	APP_INT_ISR_VECTOR
		PAGESEL	BigZ_ISR
		MOVFW	PCLATH
		MOVWF	APP_INT_ISR_VECTOR + 1
		PAGESEL	$
	
;********************************************************
;---------------------------------
BigZ_LOOP
		FARCALL		NRF_DATA_HANDLER
	GOTO			BigZ_LOOP			
;---------------------------------
;********************************************************

;**************************************************************	
NRF_DATA_HANDLER
		m_RD_NRF_REG	CONFIG,		NRF_CONFIG		;	THIS UPDATES THE FIFO_STATUS AND THE STATUS REGISTERS
		BTFSS		NRF_CONFIG,		PRIM_RX			; IF IT IS IN PRIM_RX MODE, FIFO_STATUS.TX_EMPTY READS 1 ALWAYS
		GOTO		IN_TX_MODE						; SO SKIP THESE LINES

check_for_data 
		BANKSEL		UTON_BUF_DATA_CNT
		MOVF		UTON_BUF_DATA_CNT, F			; SET THE ZERO FLAG, IF F = 0.
		BTFSC		STATUS, Z						; IS THERE DATA WAITING IN THE UTON BUFFER?
		GOTO		NO_DATA_TO_SEND					; NO. GO SEE IF DATA HAS BEEN RECEIVED

DATA_TO_SEND
		BANKSEL		NRF_STATE_REG
		FARCALL		XMIT_UTON_BUF_DATA					; TRANSFER DATA TO NRF, AND INITIATE TRANSMISSION OF BUFFER
		BSF			NRF_STATE_REG,	TX_IN_PROGRESS	; (BIT 3)
	RETURN

IN_TX_MODE
		BANKSEL		NRF_STATE_REG
		BTFSS		NRF_STATE_REG,	TX_IN_PROGRESS	; (BIT 3)
		GOTO		TX_IS_IDLE	
		BANKSEL		PORTB
		BTFSC		PORTB,	RB0			; IF RB0 - 0, A TRANSMISSION HAS COMPLETED
		GOTO		IN_TX_MODE
		FARCALL		NRFIntISR
		BANKSEL		NRF_STATE_REG
		BCF			NRF_STATE_REG,	TX_IN_PROGRESS	; (BIT 3)
		
TX_IS_IDLE	
		m_RD_NRF_REG	FIFO_STATUS,		NRF_FIFO_STATUS	;THIS UPDATES THE FIFO_STATUS AND THE STATUS REGISTERS
		BTFSC			NRF_FIFO_STATUS,	TX_EMPTY	; (BIT 4)
		GOTO			TX_FIFO_EMPT
		FARCALL			TRANSMIT_LOADED_DATA			;
		
TX_FIFO_EMPT
		banksel		UTON_BUF_DATA_CNT
		movfw		UTON_BUF_DATA_CNT
		btfsc		STATUS, Z
		GOTO		NO_DATA_TO_SEND
		GOTO		DATA_TO_SEND

NO_DATA_TO_SEND
		BANKSEL		NRF_STATE_REG
		BCf			NRF_STATE_REG,	TX_IN_PROGRESS		; THIS SHOULD ALREADY BE CLEARED!!!
		FARCALL		NRF_ENTER_RCV_MODE
CHECK_FOR_DATA_RECEIVED
		BANKSEL		NRF_STATUS
		BTFSS		NRF_STATUS, 	RX_DR
	  RETURN	
		FARCALL		RX_DR_SR					;	SERVICE THE RECEIVED DATA
	RETURN
	
;********************************************************
;---------------------------------
;	END OF BigZ_LOOP
;---------------------------------
;********************************************************
;********************************************************
;	
;********************************************************
NRF_INTERRUPT_CHECK
;		FARCALL	NRFGetStatus
		m_RD_NRF_REG	CONFIG,			NRF_CONFIG		;	THIS UPDATES THE FIFO_STATUS AND THE STATUS REGISTERS
		m_RD_NRF_REG	FIFO_STATUS,	NRF_FIFO_STATUS		;	THIS UPDATES THE FIFO_STATUS AND THE STATUS REGISTERS

		MOVLW	0X70				; MASK OFF ALL BUT BITS 5,6, & 7
		BANKSEL	NRF_STATUS
		ANDWF	NRF_STATUS			; 
		BTFSC	STATUS,	Z
		RETLW	0X00
		
NRF_INT_ASSERTED
		BTFSS	NRF_STATUS,	TX_DS
		GOTO	NO_TX_DS
		FARCALL	TX_DS_SR
		RETLW	0X02
NO_TX_DS
 		BTFSs	NRF_STATUS,	RX_DR
		GOTO	NO_RX_DR
		FARCALL	RX_DR_SR
		RETLW	0X03
NO_RX_DR		
		BTFSS	NRF_STATUS,	MX_RT
		GOTO	NO_INTS
		FARCALL	MAX_RT_SR
		RETLW	0X01
NO_INTS
		RETLW	0X00
	RETURN
	
;-----------------------------------------------------------
;CHECK TO SEE IF NRF IS CURRENTLY TRANSMITTING:
;-----------------------------------------------------------
NRF_IS_XMITTING
		BANKSEL	NRF_STATE_REG			; LETS SEE WHAT'S HAPPENING BETWEEN THE INS AND OUTS
		BTFSC	NRF_STATE_REG, TX_IN_PROGRESS	;	ARE WE TRANSMITING?
		RETLW	0X01
		RETLW	0X00
	RETURN

;-----------------------------------------------------------
NO_ACTIVE_TX
;-----------------------------------------------------------
; CHECK FOR UNTRANSMITTED DATA IN NRF'S TRANSMIT BUFFER
TX_FIFO_DATA_PENDING
		CALL	IS_DATA_WAITING_IN_TX_FIFO	;
		SKIP_IF_YES
		RETLW	0X00
		FARCALL	TRANSMIT_LOADED_DATA		;
		RETLW	0X01
	RETURN	;GOTO	BigZ_LOOP
;-----------------------------------------------------------
NO_DATA_IN_TX_FIFO
;-----------------------------------------------------------
; CHECK FOR DATA IN PIC'S UART-TO-NRF BUFFER
IS_UTON_BUF_DATA_WAITING
		FARCALL	IS_U_TO_N_BUF_EMPTY		;
		SKIP_IF_NOT
		RETLW	0X00
	RETLW	0X01
	
	
;-----------------------------------------------------------
NOT_TOO_BUSY
;-----------------------------------------------------------
;	In this section, we can do other stuff...
	FARCALL		READ_NEXT_NRF_REG
	GOTO		BigZ_LOOP

;---------------------------------	
TX_ACTIVATED
;---------------------------------
	;NRF IS BUSY TRANSMITTING, SO NOTHING TO DO.
	RETURN

;---------------------------------
RX_FUNCTIONS
;---------------------------------

IS_DATA_IN_N_TO_U_BUF
    BANKSEL NTOU_BUF_DATA_CNT					; 
    MOVFW   NTOU_BUF_DATA_CNT					; 
    BTFSC   STATUS,Z               			 	; Check if data is qued in Rx buf
	RETLW	0X00			;GOTO	BigZ_LOOP						;NOPE. BUFFERS EMPTY
    BANKSEL	PIE1							; BUT IF IT'S NOT, TXIE SHOULD BE SET
	BTFSS	PIE1,	TXIE
	CALL	BUFFER_HANDLER_ASLEEP
	RETLW	0X01	;GOTO	BigZ_LOOP						;NOPE. BUFFERS EMPTY
	
;---------------------------------
;---------------------------------
	
IS_U_TO_N_BUF_EMPTY					; CHECK THE PICS UART-NRF FIFO
    BANKSEL NUUNBUFSTAT				; 
    BTFSS   NUUNBUFSTAT,U2NBE      	; Check if data is qued in Rx buf	CALL	TRANSMIT_UTON_BUFFER				; YES. TRANSFER IT TO THE NRF CHIP
    RETLW	0						; 
    BANKSEL	UTON_BUF_DATA_CNT		; DOUBLE-CHECKING...
    MOVFW	UTON_BUF_DATA_CNT
    IORLW	0						; AFFECT THE STATUS Z BIT
    BTFSS	STATUS,	Z  
    RETLW	0						; NOT EMPTY
    RETLW	1						; EMPTY

;---------------------------------
IS_DATA_WAITING_IN_TX_FIFO
	m_RD_NRF_REG	CONFIG,	NRF_CONFIG
	BANKSEL			NRF_CONFIG
	BTFSC			NRF_CONFIG,	PRIM_RX
	RETLW	0						; IF IN RECEIVE MODE, IGNORE THE FIFO
	m_RD_NRF_REG	FIFO_STATUS,NRF_FIFO_STATUS		; IS THE FIFO EMPTY?
	BANKSEL		NRF_FIFO_STATUS
	BTFSS		NRF_FIFO_STATUS,	TX_EMPTY	;(BIT 4)
	RETLW	1
    RETLW	0
	
CRAP_LABEL
	BANKSEL	PORTA
	MOVLW	0X1
	XORWF	PORTA, F
	GOTO		BigZ_LOOP

;***********************************************;
;	Normal end of tthe main loop when nothing's going on
;***********************************************;

	BTFSC	NRF_STATE_REG, TX_DS_ISR_BIT
	GOTO	TX_WAS_GOOD
	GOTO	BigZ_LOOP

	BANKSEL	NRF_STATE_REG
	BTFSC	NRF_STATE_REG, MAX_RT_ISR_BIT
	GOTO	MAX_RT_FAILURE
	
	BTFSC	NRF_STATE_REG, UNKNOWN_FAILURE
	GOTO	UNKNOWN_FAILURE_FOUND	
	BTFSC	NRF_STATE_REG, UNKNOWN_INTERRUPT
	GOTO	UNKNOWN_FAILURE_FOUND	
	
TX_WAS_GOOD	
#IFDEF REPORT_TX_SUCCESS
	FARCALL	PRINTF_TX_SUCCESS
#ENDIF
	BANKSEL	NRF_STATE_REG
	BCF		NRF_STATE_REG, TX_DS_ISR_BIT
	RETURN
	
MAX_RT_FAILURE
#IFDEF REPORT_MAX_RT	
	FARCALL	PRINTF_MAX_RT
#ENDIF
	BCF		NRF_STATE_REG, MAX_RT_ISR_BIT
	RETURN

UNKNOWN_FAILURE_FOUND
	FARCALL	PRINTF_UNKNOWN_FAILURE
	BCF		NRF_STATE_REG, UNKNOWN_FAILURE
	RESET
	RETURN
	
;----------------------------------------------------------------------
;-----------END OF BigZ_ROUTINE-------------------------------------------
;----------------------------------------------------------------------

;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
;		LCR_METER ROUTINE
;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
LCR_METER_ROUTINE
LCR_METER_LOOP
	GOTO	LCR_METER_LOOP
;--------------------------------------------------------------------------
;-----------END OF LCR_METER ROUTINE----------------------------------------
;--------------------------------------------------------------------------


;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
;		UTILITY ROUTINES
;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; W_X_1uS_DELAY	
;		DELAY = 25OnS * (4IC + (WREG-1)(4IC) + 2IC), FOR WREG > 1
;			  = 25OnS * 5IC, FOR WREG = 0
;		DELAY = 25OnS * (4IC + (0XFF)(4IC) + 2IC), FOR WREG = 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
W_X_1uS_DELAY	
		MOVWF	CONFIG_ARG_1			; 1IC
		decfsz	CONFIG_ARG_1			;  +2IC
		goto	W_X_1uS_DELAY_LOOP	
		RETURN						;	5IC MINIMUM, = 1.25uSEC (WHEN WREG = 1 ON ENTRY)
	W_X_1uS_DELAY_LOOP				; (WREG-1)*
		NOP							;     (1IC
		decfsz	CONFIG_ARG_1			;      +2IC
		goto	W_X_1uS_DELAY_LOOP	;               +1IC) = 4IC
	Return							; +2IC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; W_X_10uS_DELAY	
;		DELAY = 4/16MHZ * (8IC + WREG * (3*12IC + 6IC))
;			  = 25OnS + (2uS + WREG( 10uS))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
W_X_10uS_DELAY	
#IFDEF	DEBUG_PULSE_RB3		;				 	
		BANKSEL	PORTB			;	..1IC \
		BCF		PORTB,3			;	..1IC  \	4IC
		BSF		PORTB,3			;	..1IC  /
	#ENDIF
		banksel	DELAYREGA		;	..2 IC			 	
		MOVWF	DELAYREGA		;	..1 IC			   				3IC
	DELAYA_LOOP_10U				;						 
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\			   
		MOVLW	.12				;	..1 IC		    \	EACH DELAYREGA
		MOVWF	DELAYREGB		;	..1 IC		 \		2IC
	DELAYB_LOOP_10U								  ;>  + (12 * 3IC) 
		DECFSZ	DELAYREGB		;	..1(2) IC	 /	  + 2IC 
		GOTO	DELAYB_LOOP_10U		;	..2 IC		/	  =	40IC
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;/	
		DECFSZ	DELAYREGA		;	..1(2)IC 		\
		GOTO	DELAYA_LOOP_10U		;	..2IC			 > 				3IC
#IFDEF	DEBUG_PULSE_RB3		;				 	/
		BANKSEL	PORTB			;	..2IC			\
		BCF		PORTB,3			;	..1IC			 / 3IC
#ENDIF
	RETURN						;	..2IC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; W_X_100uS_DELAY	
;		DELAY = 4/16MHZ * (8IC + WREG * (3*132IC + 6IC))
;			  = 25OnS + (2uS + WREG( 100uS))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
W_X_100uS_DELAY	
	#IFDEF	DEBUG_PULSE_RB3		;				 	
		BANKSEL	PORTB			;	..1IC \
		BCF		PORTB,3			;	..1IC  \	4IC
		BSF		PORTB,3			;	..1IC  /
	#ENDIF
		banksel	DELAYREGA		;	..2 IC			 	
		MOVWF	DELAYREGA		;	..1 IC			   				3IC
	DELAYA_LOOP					;						 
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\			   
		MOVLW	.132			;	..1 IC		    \	EACH DELAYREGA
		MOVWF	DELAYREGB		;	..1 IC		 \		2IC
	DELAYB_LOOP									  ;>  + (132 * 3IC) 
		DECFSZ	DELAYREGB		;	..1(2) IC	 /	  + 2IC 
		GOTO	DELAYB_LOOP		;	..2 IC		/	  =	400IC
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;/	
		DECFSZ	DELAYREGA		;	..1(2)IC 		\
		GOTO	DELAYA_LOOP		;	..2IC			 > 				3IC
	#IFDEF	DEBUG_PULSE_RB3		;				 	/
		BANKSEL	PORTB			;	..2IC			\
		BCF		PORTB,3			;	..1IC			 / 3IC
	#ENDIF
	RETURN						;	..2IC


W_x_10mS_DELAY
		banksel	COUNT_REG
		movwF	COUNT_REG
Ten_More_MiliSec
		movlw	.100
		FARCALL	W_X_100uS_DELAY
		decf	COUNT_REG
		SKIP_IF_ZERO
		GOTO	Ten_More_MiliSec
	Return


;;****************************************************************
;;	NAME: EEPROM_READ_W
;;	FUNCTION: READS THE DATA AT THE ADDRESS POINTED TO BY WREG
;; 	ARGUMENT: W_REG (READS DATA AT THIS ADDRESS)
;;	RETURNS:  EEPROM DATA IN THE WREG
;; 	AFFECTS:  W_REG, BANKSEL 0
;;****************************************************************
;EEPROM_READ_W
;		BANKSEL 	EEADRL ;
;		MOVWF 		EEADRL ;Data Memory Address to read
;;****************************************************************
;;	NAME: EEPROM_READ
;;	FUNCTION: READS THE DATA AT EEADR
;; 	ARGUMENT: NONE
;;	RETURNS:  EEPROM DATA IN THE WREG
;; 	AFFECTS:  W_REG, BANKSEL 0
;;****************************************************************
;EEPROM_READ
;		MOVLW		0XF0
;		MOVWF		EEADRH
;		BANKSEL 	EECON1 ;
;		BCF 		EECON1, CFGS ;Point to DATA memory
;		BCF 		EECON1, EEPGD ;Point to DATA memory
;		BSF 		EECON1, RD ;EE Read
;		BANKSEL		EEDATL
;		MOVF 		EEDATL, W ;W = EEDATA
;		BANKSEL		0 ;Bank 0
;	RETURN	
;	
;;****************************************************************
;;	NAME: EEPROM_WRITE
;;	FUNCTION: WRITES THE DATA IN DATA_EE_DATA TO THE ADDRESS POINTED TO BY DATA_EE_ADDR
;; 	ARGUMENT: NONE
;;	REQUIRES: EEADRL AND EEDATL MUST BE PRE-LOADED BY m_EEPROM_WRITE MACRO
;;	RETURNS:  VOID
;; 	AFFECTS: DATA IN DATA_EE_ADDR, SETS BANK TO 0
;;****************************************************************
;EEPROM_WRITE		
;    BCF EECON1, CFGS ;Deselect Configuration space
;    BCF EECON1, EEPGD ;Point to DATA memory
;    BSF EECON1, WREN ;Enable writes
;    BCF INTCON, GIE ;Disable INTs.
;    MOVLW 55h ;
;    MOVWF EECON2 ;Write 55h
;    MOVLW 0AAh ;
;    MOVWF EECON2 ;Write AAh
;    BSF EECON1, WR ;Set WR bit to begin write
;    BSF INTCON, GIE ;Enable Interrupts
;    BCF EECON1, WREN ;Disable writes
;    BTFSC EECON1, WR ;Wait for write to complete
;    GOTO $-2 ;Done
;  RETURN
;    

PRINTF_ALL_NRF_REGISTERS
	CLRF	TABLE_POSITION_PTR	;	
	CLRF	TBL_START_PTR	;	ADDRESS LOCATION
	CLRF	BL_REG_PCLATH		;	PAGE LOCATION 
	CLRF	BKLS_REG1			;	THIS IS THE BYTE INDEX
PANR_L0OP
	;--------------------------------------------
	; FIRST, GET THE PAGE LOCATION FOR THE TABLES
	;--------------------------------------------
	MOVLW	High(tbl_of_Tbl_Pg_Ptrs_To_REG_NAMES)	;\	
	MOVWF	BL_REG_PCLATH								;  >PAGE LOCATION
	MOVLW	LOW(tbl_of_Tbl_Pg_Ptrs_To_REG_NAMES)	;\
	MOVWF	TBL_START_PTR							;  > ADDRESS
	MOVFW	BKLS_REG1									;	
	FARCALL	GET_BYTE_W_FROM_TABLE						; GET THE PAGE POINTER
	MOVWF	CONFIG_ARG_1								;	TEMPORARILY STORE THE PAGE HERE

	;--------------------------------------------
	; SECOND, GET THE ADDRESS LOCATION FOR THE TABLES
	;--------------------------------------------
	MOVLW	High(tbl_of_Tbl_Pointers_To_REG_NAMES)	;\	
	MOVWF	BL_REG_PCLATH							;  >PAGE LOCATION
	MOVLW	LOW(tbl_of_Tbl_Pointers_To_REG_NAMES)	;\
	MOVWF	TBL_START_PTR							;  > ADDRESS
	MOVFW	BKLS_REG1								;\
	FARCALL	GET_BYTE_W_FROM_TABLE					; GET ONE OF THE TABLE POINTERS FROM THIS TABLE OF TABLE POINTERS	!!!ADJUSTS PCLATH IF NEEDED
	MOVWF	TBL_START_PTR

	MOVFW	CONFIG_ARG_1							;  INITIALIZE THE PAGE LOCATION
	MOVWF	BL_REG_PCLATH							;  >PAGE LOCATION
	
	;--------------------------------------------
	;	WE SHOULD NOW BE ABLE TO PRINT THE TABLE
	FARCALL	IMMEDIATE_PRINTF
	;--------------------------------------------
	;--------------------------------------------
	MOVLW	High(tbl_GHOST_REG_VALS)
	MOVWF	BL_REG_PCLATH
	MOVLW	LOW(tbl_GHOST_REG_VALS)
	MOVWF	TBL_START_PTR
	MOVFW	BKLS_REG1
	MOVWF	TABLE_POSITION_PTR							;  
	
	FARCALL	GET_BYTE_W_FROM_TABLE	; GET ONE OF THE NRF_REG POINTERS
	CLRF	FSR1H			;
	MOVWF	FSR1L
	MOVFW	INDF1
	FARCALL	HEXTOUART
	INCF	BKLS_REG1
	MOVLW	REGISTER_COUNT
	XORWF	BKLS_REG1,	W
	BTFSS	STATUS,	Z
	GOTO	PANR_L0OP
 RETURN
		
;--------------------------------------------
;CALLED FROM THE SETUP ROUTINE
	CLEAR_ALL_GPRS
;--------------------------------------------
	BANKSEL	0
	MOVLW	20
	MOVWF	FSR0H
	MOVLW	0X20
	MOVWF	FSR0L
CLR_GPR_LOOP
	CLRF	INDF0
	INCFSZ	FSR0L
	GOTO	CLR_GPR_LOOP
	INCF	FSR0H
	MOVLW	0X2A
	SUBWF	FSR0H,	W
	BTFSC	STATUS,	Z
	RETURN
	GOTO	CLR_GPR_LOOP


#IFNDEF		Tables
#INCLUDE	Tables.inc
#ENDIF

#IFNDEF		UARTInt_DEF
#INCLUDE	UARTInt_DEF.INC
#ENDIF

#IFNDEF 	nrf241
#INCLUDE	nrf241.inc
#ENDIF

#IFNDEF 	uart_lib
#INCLUDE	uart_lib.inc
#ENDIF

#IFNDEF 	SPI
#INCLUDE	SPI.inc
#ENDIF
	
#IFNDEF		DRAGRACE_TIMER
#INCLUDE	DRAGRACE_TIMER.inc
#ENDIF

#IFNDEF		EEPROM_CONFIG
#INCLUDE	EEPROM_CONFIG.INC
#ENDIF

#IFNDEF		ANDROID_LED_CONTROLLER
#INCLUDE	ANDROID_LED_CONTROLLER.INC
#ENDIF

#IFNDEF		COMPARATOR_LIB
#INCLUDE	COMPARATOR_LIB.inc
#ENDIF

                 
  END		;18, 19,1A, and 1B. Thes
