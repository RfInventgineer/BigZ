
#ifndef	UARTInt_DEF
#define UARTInt_DEF

; 
#ifndef CLOCK_FREQ
#define CLOCK_FREQ      D'16000000'
#endif
; 
#define UARTINT_TXON
#define UARTINT_RXON
;#define NtoU_BUFFER_SIZE .32
;#define UtoN_BUFFER_SIZE .32
; 

; Baud rate constants from (((10*XTAL_FREQ/(16*BAUD))+5)/10)-1
; note these calulations uses BRGH = 1 ( high speed mode )
; for BRGH = 0 use the formula: (((10*XTAL_FREQ/(64*BAUD))+5)/10)-1
; NOTE Rather use the calculations further down below instead !
;BAUD_9600	EQU	(((.10*XTAL_FREQ/(.16*.9600))+.5)/.10)-.1
;BAUD_19200	EQU	(((.10*XTAL_FREQ/(.16*.19200))+.5)/.10)-.1
;BAUD_38400	EQU	(((.10*XTAL_FREQ/(.16*.38400))+.5)/.10)-.1
;BAUD_57600	EQU	(((.10*XTAL_FREQ/(.16*.57600))+.5)/.10)-.1
;BAUD_115200	EQU	(((.10*XTAL_FREQ/(.16*.115200))+.5)/.10)-.1

#DEFINE BRGH_VAL_HI          

;	GLOBAL	NUUNBUFSTAT     
;	GLOBAL	NtoU_BUFFER      
;	GLOBAL	NtoU_BUF_DATA_CNT  
;	GLOBAL	NtoU_BUF_WT_PTR    
;	GLOBAL	NtoU_BUF_RD_PTR                         
;	GLOBAL	UtoN_BUFFER      
;	GLOBAL	UtoN_BUF_DATA_CNT  
;	GLOBAL	UtoN_BUF_WT_PTR    
;	GLOBAL	UtoN_BUF_RD_PTR    
;	GLOBAL	temp1
;	GLOBAL	temp3
;
;N2UBuffers	    UDATA	0xA1
;NUUNBUFSTAT	    	RES      1					;Used to store status flags
;NTOU_BUFFER	    	RES     NtoU_BUFFER_SIZE    ;Transmit Data FIFO buffer
;NTOU_BUF_DATA_CNT   RES     01					;Number of Bytes in Buffer
;NTOU_BUF_WT_PTR     RES     01          	;Transmit Buffer Write Position
;NTOU_BUF_RD_PTR     RES     01          	;Transmit Buffer Read Position
;
;UTON_BUFFER        	RES     UtoN_BUFFER_SIZE ;Receive Data FIFO buffer
;UTON_BUF_DATA_CNT   RES     01				;Number of Bytes in Buffer          	
;UTON_BUF_WT_PTR     RES     01				;Transmit Buffer Write Position
;UTON_BUF_RD_PTR     RES     01				;Transmit Buffer Read Position
;


;----- NUUNBUFSTAT Bits ---------------------------------------
U2NBF            EQU  H'0000'
U2NBE            EQU  H'0001'
U2NERR           EQU  H'0002'
U2NBOF           EQU  H'0003'
N2UBF            EQU  H'0004'
N2UBE            EQU  H'0005'
N2UERR           EQU  H'0006'
N2UBOF           EQU  H'0007'



;#define U2NBE   	0x1     	;For Receive Buffer Empty Flag in
;#define UtoNError 		0x2     	;For Data Receive Error Flag in
;#define U2NBOF  		0x3     	;For Data Buffer over flow indiaction
				                   	;bit. It indicates that Data received
                				   	;when Rxbuffer was full. So it shows
                    				;that in between data is missing.

;#define NtoUBufFul    		0x4     ;For N2U Buffer Full Flag in
;#define NtoUBufEmpty	   	0x5     ;For N2U Buffer Empty Flag in
;#define NRF2UartError 		0x6     ;For N2U Data Receive Error Flag in
;#define NRF2UartBufOF  		0x7     ;For N2U Buffer over flow indiaction
                   	 				;bit. It indicates that Data received
                    				;when Rxbuffer was full. So it shows
                    				;that in between data is missing.       
              
;For Receive Buffer Full Flag in             

UDATA_OVR

;This data is temporarily only. It can be kept in over writtable RAM.
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

#endif