#IFNDEF	    nrf24L01_DEF
#define	    nrf24L01_DEF


ARDELAYTIME			SET	.1500		; uS, HOW LONG, IN uS, TO WAIT BEFORE RE-TRANSMITTING
AUTO_RETRAN_COUNT	SET	 D'2'	; HOW MANY RETRYS...0 TO 15
POWER_LEVEL			SET	.20			; dBm, INCLUDING APP. 20dB GAIN FROM PA
BIT_RATE			SET	.250			; .250 			; Xb/s, MUST BE .100, .2000, OR .250
ADDRESS_WIDTH		SET	5		; ADDRESS WIDTH, IN BYTES,  FOR NRF TX AND RX MUST BE 3, 4, OR 5
MAX_PAYLOAD_BYTES	SET .32

;*************************************************************************
;/* Memory Map */
; BITS 7-0:	RESERVED, MASK_RX_DR,MASK_TX_DS,MASK_MAX_RT,EN_CRC,CRCO,PWR_UP,PRIM_RX
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define RF_STATUS   0x07
#define OBSERVE_TX  0x08
#define RPD         0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10			;R/W TRANSMIT ADDRESS. USED FOR A PTX DEVICE ONLY.
						;(LSBYTE IS WRITTEN FIRST) SET RX_ADDR_P0 EQUAL TO 
						; THIS ADDRESS TO HANDLE AUTOMATIC ACKNOWLEDGE IF THIS 
						;IS A PTX DEVICE WITH ENHANCED SHOCKBURST™ ENABLED

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;NUMBER OF BYTES IN RX PAYLOAD IN DATA PIPES. 1 TO 32 BYTES.
;	0 = PIPE NOT USED
;	1 = 1 BYTE...
;	...32 = 32 BYTES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17

#define DYNPD 		0X1C	;ENABLE DYNAMIC PAYLOAD LENGTH
#define FEATURE 	0X1D	;FEATURE REGISTER


;**********/* Bit Mnemonics */*****************;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	CONFIG BITS
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRC2BYTE	2
#define PWR_UP      1
#define PRIM_RX     0

;	FOR EN_AA
#define ENAA_P5     B'100000'
#define ENAA_P4     B'010000'
#define ENAA_P3     B'001000'
#define ENAA_P2     B'000100'
#define ENAA_P1     B'000010'
#define ENAA_P0     B'000001'

;	FOR EN_RXADDR
#define ERX_P5      B'100000'	
#define ERX_P4      B'010000'
#define ERX_P3      B'001000'
#define ERX_P2      B'000100'
#define ERX_P1      B'000010'
#define ERX_P0      B'000001'

;	FOR SETUP_RETR
#define ARD         4		;Auto Retransmit Delay
#define ARC         0		;Auto Retransmit Count

;	FOR RF_SETUP
;#define PLL_LOCK    4
;#define RF_DR_HI    3
;#define RF_PWR_MSB	2
;#define RF_PWR_LSB	1
;
;#define LNA_HCURR   0        

#define RX_DR       6
#define TX_DS       5
#define MX_RT		4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0

;*************************************************************************
;**************	FACTORY DEFAULT REGISTER VALUES   *****************************
;~~~~~~~~~~PRE-CALCS~~~~~~~~~~~~~~~~
	;	FOR SETUP_RETR
	ARDVAL			SET	((ARDELAYTIME/D'250') + 1) * .16
	ARCVAL			SET	AUTO_RETRAN_COUNT

	; FOR RF_SETUP Register
	IF BIT_RATE == .250
		BIT_RATE_VAL SET B'00100000'
	ELSE
		IF BIT_RATE == .2000
			BIT_RATE_VAL SET B'00001000'
		ELSE
			IF	BIT_RATE == .1000
				BIT_RATE_VAL SET B'00000000'
			ENDIF
		ENDIF		
	ENDIF
	RF_PWR_VAL 	SET (POWER_LEVEL -.2)/.3	; ex.: If Power Level = 20dBm, then power level = (20-2)/3 = 6h, or 110b

;*************************************************************************
;*************************************************************************

CONFIG_VAL		SET	b'00001111'			; EN_CRC + CRC2BYTE + PWR_UP + PRIM_RX
EN_AA_VAL		SET	0x3F				; ENABLES AUTO-ACK FOR ALL PIPES
EN_RXADDR_VAL	SET	0X3F				; ENABLES RX ADDRESSES FOR ALL PIPES
SETUP_AW_VAL	SET ADDRESS_WIDTH - 2
SETUP_RETR_VAL	SET ARDVAL + ARCVAL
RF_CH_VAL		SET	0x22			; RF FREQ = 2.4ghZ + TESTCHANNEL*1MHz
RF_SETUP_VAL	SET	RF_PWR_VAL + BIT_RATE_VAL
;
;	STATUS, OBSERV_TX, AND RPD NEED NO INITIALE VALUE....
;
RXAD0_0_VAL		SET	0x96
RXAD0_1_VAL		SET	0x96
RXAD0_2_VAL 	SET	0x96
RXAD0_3_VAL		SET	0x96
RXAD0_4_VAL		SET	0x96

RXAD1_0_VAL		SET	0X71
RXAD1_1_VAL		SET	0X71
RXAD1_2_VAL		SET	0X71
RXAD1_3_VAL		SET	0X71
RXAD1_4_VAL		SET	0X71

RXAD2_0_VAL		SET	0X72
RXAD3_0_VAL		SET	0X73
RXAD4_0_VAL		SET	0X74
RXAD5_0_VAL		SET	0X75

TXAD0_VAL		SET	0x96
TXAD1_VAL		SET	0x96
TXAD2_VAL		SET	0x96
TXAD3_VAL		SET	0x96
TXAD4_VAL		SET	0x96

RX_PW_Px_VAL	SET	0X20	; 32
DYNPD_VAL		SET	0X3f	
FEATURE_VAL		SET	0X07
;*************************************************************************
;*************************************************************************

;***********************************************;
;RF_STATUS_READ BITS
RX_P_NO_3	SET		0x3
RX_P_NO_2	SET		0x2
RX_P_NO_1	SET		0x1


;/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
;#define NOP           0xFF

;/*;;;;;;;NRF24 COMMANDS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/

#define W_TX_PAYLOAD 		B'10100000'		;DATA LENGTH: 1 TO 32 LSBYTE FIRST
#define W_TX_PAYLOAD_NOACK	B'10110000'		;1 TO 32 BYTE DATA LENGTH. LSBYTE FIRST USED IN TX MODE. 
												;DISABLES AUTOACK ON THIS SPECIFIC PACKET.
#define W_ACK_PAYLOAD		B'10101000' 	;1 TO 32BYTES,LSBYTE FIRST. USED IN RX MODE.
													;WRITE PAYLOAD TO BE TRANSMITTED TOGETHER WITH
													;ACK PACKET ON PIPE PPP. (PPP VALID IN THE
													;RANGE FROM 000 TO 101). MAXIMUM THREE ACK
													;PACKET PAYLOADS CAN BE PENDING. PAYLOADS WITH
													;SAME PPP ARE HANDLED USING FIRST IN - FIRST OUT
													;PRINCIPLE. WRITE PAYLOAD: 1– 32 BYTES. A WRITE
													;OPERATION ALWAYS STARTS AT BYTE 0.												;LSBYTE FIRST USED IN TX MODE. DISABLES AUTOACK 

#define WRITE_REG			0X20

; READ PAYLOAD COMMAND: R_RX_PAYLOAD, TO GET THE DATA OUT OF THE RX_PLD REGISTER
#define R_RX_PAYLOAD 		0X61			;DATA LENGTH: 1 TO 32 LSBYTE FIRST
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#define FLUSH_TX			0XE1		
#define FLUSH_RX			0XE2		
#define REUSE_TX_PL			B'11100011'		
#define ACTIVATE 			B'01010000'		;FOLLOWED BY DATA 0X73 ACTIVATES THESE FEATURES:
												; • R_RX_PL_WID
												; • W_ACK_PAYLOAD
												; • W_TX_PAYLOAD_NOACK
												; A NEW ACTIVATE COMMAND WITH THE SAME DATA
												; DEACTIVATES THEM AGAIN. THIS IS EXECUTABLE IN
												; POWER DOWN OR STAND BY MODES ONLY.

#define R_RX_PL_WID 	B'01100000'		; READ RX-PAYLOAD WIDTH FOR THE R_RX_PAYLOAD IN RX FIFO.
#define NRF_NOP 			0xFF 		;0 BYTES, NO OPERATION.


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	R/W RECEIVE ADDRESS DATA PIPES 0 THRU 5, EACH IS UP TO 5 BYTES LONG.
;	LSBYTE IS WRITTEN FIRST. WRITE THE NUMBER OF BYTES DEFINED BY SETUP_AW.

WT_TX_ADDR 		SET	b'00110000'	;0X30 	;R/W TRANSMIT ADDRESS. USED FOR A PTX DEVICE ONLY.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;----------NRF/NRF_STATE_REG BIT DEFINITIONS----------------------
UNKNOWN_FAILURE		EQU	7	; 
RX_DR_ISR_BIT 		EQU	6	; Data was received
TX_DS_ISR_BIT 		EQU	5	; Transmit Data was sent
MAX_RT_ISR_BIT 		EQU	4	; INDICATES FAILURE. MAXIMUM RETRIES
TX_IN_PROGRESS 		EQU	3	; Transmitting a packet
RX_IN_PROGRESS		EQU 2	; Receiveing a packet
UNKNOWN_INTERRUPT	EQU	1	; Indicates a bug in software, probably
NRF_RX_FIFO_OF		EQU 0	; PIC isnt keeping up with the data transmission
;-----------------------------------------------------------------------

#define RX_EMPTY_MASK				B'00000001'
#define RX_P_NO_MASK				B'00001110'
#define NRF_INT_FLAG_MASK			B'01110000'
#define SWAPPED_NRF_INT_FLAG_MASK	B'00000111'
#define RX_DR_FLAG_MASK				B'01000000'
#define TX_DS_FLAG_MASK				B'00100000'
#define MAX_RT_FLAG_MASK			B'00010000'
	
;------------------- FLAG BITS IN THE FIFO_STATUS REGISTER --------------------------
TX_REUSE  			EQU  6
FIFO_FULL 			EQU  5
TX_EMPTY  			EQU  4
RX_FULL   			EQU  1
RX_EMPTY  			EQU  0

TX_FIFO_FULL 		EQU	5 	; TX FIFO full flag. 1: TX FIFO full. 0: Available locations in TX FIFO.
TX_FIFO_EMPTY 		EQU	4	; R TX FIFO empty flag.	1: TX FIFO empty.	0: Data in TX FIFO.

RESERVED3			SET	3
RESERVED2			SET	2
;--------------------------------------------------------------------------------------
#ENDIF
