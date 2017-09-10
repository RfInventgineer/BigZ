LIST
;*********************************************************************
;*
;*  This implements a generic library functionality to support UART.
;*  It adds additional functionality of Rx/Tx user defined circular 
;*  FIFO buffer
;*
;*********************************************************************
;* FileName:            UARTInt.asm
;*                      P16xxx.inc
;*                      UARTInt.inc
;* Processor:           PIC16xxxx 
; errorlevel      -207    ;Hide found label after column 1 warning

#IFNDEF UART_LIB	
#defINE uart_lib

#INCLUDE	UARTInt_DEF.INC

;****************************************************************************
; Macro:        mUARTRxIntDisable
;
; Overview:     Disables Receive interrupt.
;
; Input:        None
;
; Output:       None
;
; Side Effects: Databank changed
;****************************************************************************
mUARTRxIntDisable    macro
        banksel PIE1
        bcf     PIE1, RCIE                      ; Disable USART rx interrupt
        endm

;****************************************************************************
; Macro:        mUARTRxIntEnable
;
; Overview:     Enables Receive interrupt.
;
; Input:        None
;
; Output:       None
;
; Side Effects: Databank changed
;****************************************************************************
mUARTRxIntEnable     macro
        banksel PIE1
        bsf     PIE1, RCIE                      ; Enable USART rx interrupt
        endm



;****************************************************************************
; Macro:        mDisableUARTTxInt
;
; Overview:     Disables Transmit interrupt.
;
; Input:        None
;
; Output:       None
;
; Side Effects: Databank changed
;****************************************************************************
mDisableUARTTxInt    macro
        banksel PIE1
        bcf     PIE1, TXIE                      ; Disable USART Tx interrupt
        endm


;****************************************************************************
; Macro:        mEnableUARTTxInt
;
; Overview:     Enables Transmit interrupt.
;
; Input:        None
;
; Output:       None
;
; Side Effects: Databank changed
;****************************************************************************
mEnableUARTTxInt     macro
        banksel PIE1
        bsf     PIE1, TXIE                      ; Enable USART Tx interrupt
        endm



;****************************************************************************
; Macro:        mSetUART_BRGHHigh
;
; Overview:     Sets BRGH value to high
;
; Input:        None
;
; Output:       None
;
; Side Effects: Databank changed
;****************************************************************************
mSetUART_BRGHHigh         macro
        banksel TXSTA
        bsf     TXSTA,BRGH              ;Enable BRGH
        endm


;****************************************************************************
; Macro:        mSetUART_BRGHLow
;
; Overview:     Sets BRGH value to low.
;
; Input:        None
;
; Output:       None
;
; Side Effects: Databank changed
;****************************************************************************
mSetUART_BRGHLow          macro
        banksel TXSTA
        bcf     TXSTA,BRGH              ;Disable BRGH
        endm




;****************************************************************************
; Macro:        mSetUART_SPBRG _SPBRGVal
;
; Overview:     Loads SPBRG register with content of W register
;
; Input:        SPBRG value as argument
;
; Output:       None
;
; Side Effects: Databank, W changed
;****************************************************************************
mSetUART_SPBRG         macro   _SPBRGVal
        banksel RCSTA
        bcf     RCSTA,SPEN              ;Disable Serial Port
        movlw   _SPBRGVal
        bsf     STATUS,RP0              ;Bank-1
        movwf   SPBRG                   ;Set SPBRG value
        bcf     STATUS,RP0              ;Bank-0
        bsf     RCSTA,SPEN              ;Enable Serial Port
        endm



; errorlevel      -207    ;Hide found label after column 1 warning


;*********************************************************************
;*
;*  This implements a generic library functionality to support UART
;*  for PIC16 family
;*  It adds additional functionality of Rx/Tx user defined FIFO buffer
;*
;*********************************************************************
;* FileName:            UARTInt.asm
;* Dependencies:        P16xxx.inc
;*                      UARTInt.inc
;* Processor:           PIC16xxxx
;* Assembler:           MPASMWIN 02.70.02 or higher
;* Linker:              MPLINK 2.33.00 or higher
;* Company:             Microchip Technology, Inc.
;*
;* Software License Agreement
;*
;* The software supplied herewith by Microchip Technology Incorporated
;* (the "Company") for its PICmicro® Microcontroller is intended and
;* supplied to you, the Company's customer, for use solely and
;* exclusively on Microchip PICmicro Microcontroller products. The
;* software is owned by the Company and/or its supplier, and is
;* protected under applicable copyright laws. All rights are reserved.
;* Any use in violation of the foregoing restrictions may subject the
;* user to criminal sanctions under applicable laws, as well as to
;* civil liability for the breach of the terms and conditions of this
;* license.
;*
;* THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
;* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
;* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
;* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
;* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

        errorlevel      -302            ;Ignore Banksel warning

_UARTIntcode   code ;0X1A8

;****************************************************************************
; Function: UARTIntInit
;
; PreCondition: None
;
; Overview:
;       This routine is used for Serial Port Initialization
;       It initializes Port according to compile time selection and
;       flushes the Rx and Tx buffer. It clears all USART errors
;
; Input: None
;
;
; Output: None
;
; Side Effects: Databank, W changed
;
; Stack requirement: 1 level deep
;
;****************************************************************************


;	Formula: (((10*XTAL_FREQ/(64*BAUD))+5)/10)-1
; NOTE Rather use the calculations further down below instead !
BAUD_9600		EQU	(((.10*XTAL_FREQ/(.4*.9600))+.5)/.10)-.1	; 103, AND SET BRG16 LOW
BAUD_19200		EQU	(((.10*XTAL_FREQ/(.4*.19200))+.5)/.10)-.1	; 207
BAUD_38400		EQU	(((.10*XTAL_FREQ/(.4*.38400))+.5)/.10)-.1	; 103
BAUD_57600		EQU	(((.10*XTAL_FREQ/(.4*.57600))+.5)/.10)-.1	; 68
BAUD_115200		EQU	(((.10*XTAL_FREQ/(.4*.115200))+.5)/.10)-.1	; 34



UARTIntInit:
;---------------------------------------------------------------------------------------------
; FOR A 16MHz XTAL, BAUD RATE POSSIBILITIES ARE AS FOLLOWS:
;						
;		BAUDRATE:	SPBRG(H:L)			TXTSA:BRGH	BAUDCON:BRG16	TXTSA:SYNC
;		4800		D'832 (Ox0340)			1			1				0
;		9600: 		D'415 (Ox01A0)			1			1				0
;		19200:		D'207 (Ox00CF)			1			1				0
;		38400:		D'103 (Ox0067)			1			1				0
;		57600:		D'68  (Ox0044)			1			1				0
;		115200:		D'34  (Ox0022)			1			1				0
;		230400:		d'17  (0x0011)			1			1				0
;	In all cases, the actual baud rate will equal (16MHz/4)/(SPBRG+1). 
;
;		For example, (16MHz/4)/(103+1) = 4Mhz/104 = 38.46K ~= 38.4kbaud
;---------------------------------------------------------------------------------------------
    banksel TXSTA
    BCF     TXSTA,TXEN      ;Tx. DISabled

    banksel TXSTA
    clrf    TXSTA           ;Clear Transmit Status Register
    bcf     TXSTA,SYNC      ;CLEAR SYNC
    bsf     TXSTA,BRGH      ;Set BRGH value HIGH

	BANKSEL	BAUDCON
	BSF		BAUDCON, BRG16	;Set BRG16

;	m_EEPROM_READ	SPBRGH_EE
;	BANKSEL	SPBRGH			; default is 0. Would only be 1 for baud rates below 4800
;	MOVWF	SPBRGH			; 
	
	m_EEPROM_READ	SPBRGH_EE
	BANKSEL	SPBRGH			; default is 0. Would only be 1 for baud rates below 4800
	MOVWF	SPBRGH			; 
	
	m_EEPROM_READ	SPBRGL_EE
	BANKSEL	SPBRGL			; Default is 0x45
	MOVWF	SPBRGL
	
    banksel TXSTA
    clrf    TXSTA           ;Clear Transmit Status Register
    bcf     TXSTA,SYNC              ;CLEAR SYNC
    bsf     TXSTA,BRGH      ;Set BRGH value HIGH

    banksel TXSTA
    bsf     TXSTA,TXEN      ;Tx. Enabled.

    banksel PIE1
    bcf     PIE1,TXIE		;DIsable TXIE interrupt until we want to use it
    bsf     PIE1,RCIE       ;Enable receive interrupt

    banksel RCSTA
    clrf    RCSTA           ;Clear Receive status register
    bsf     RCSTA,SPEN      ;Enable Serial Port
    bsf     RCSTA,CREN      ;Enable Continuous Receive
 RETURN
 
 INIT_BUFFERS

	    banksel PIE1
	    bcf     PIE1,TXIE		;Desable TXIE interrupt until we want to use it
	    bcf     PIE1,RCIE       ;Enable receive interrupt

        banksel NTOU_BUF_DATA_CNT
        clrf    NTOU_BUF_DATA_CNT
        movlw	NTOU_BUFFER
       	banksel NTOU_BUF_RD_PTR
       	movwf   NTOU_BUF_RD_PTR
        movwf   NTOU_BUF_WT_PTR

        banksel	UTON_BUF_DATA_CNT
        clrf    UTON_BUF_DATA_CNT
        clrf    UTON_BUF_RD_PTR
        clrf    UTON_BUF_WT_PTR
        banksel	NUUNBUFSTAT
        clrf    NUUNBUFSTAT      		;Clear all the Errors
		BSF		NUUNBUFSTAT,	U2NBE
		BSF		NUUNBUFSTAT,	N2UBE
		
	    banksel PIE1
	    bsf     PIE1,RCIE       ;Enable receive interrupt
 RETURN


;---------------------------------------------------------------------------------------------
;---------------------------------------------------------------------------------------------
; Function: UARTIntGetCh
;
; PreCondition: None
;
; Overview:
;       It reads data POINTED AT BY UTON_BUF_RD_PTR from the UTON_BUFFER. If UTON_BUFFER is empty
;       then it will set U2NBE bit in NUUNBUFSTAT.
;       Otherwise returns received data via Wreg and decrements UTON_BUF_DATA_CNT.
;
; Input:    None
;
; Output:   If success U2NBE=0 else U2NBE=1,
;       	U2NBE is defined in NUUNBUFSTAT
;       	If success Received byte in W register
;
;
; Side Effects: Databank, FSR1, W , STATUS value changed
;
; Stack requirement: 1 level deep
;
;---------------------------------------------------------------------------------------------
UARTIntGetCh:

        GLOBAL  UARTIntGetCh        ;
        banksel UTON_BUF_DATA_CNT					;
        movf    UTON_BUF_DATA_CNT,W
        btfss   STATUS,Z                			;Check if data is qued in Rx buf...is data count at 0?
        goto    TransferRecdData					; No, go move a byte from the UTON_BUFFER to the NRF chip
        bsf     NUUNBUFSTAT,U2NBE   				;Set U2NBE flag
        retlw   0									; NOTHING TO DO

TransferRecdData    

		CLRF		FSR1H			;
        banksel		UTON_BUFFER       			
        movfw    	UTON_BUF_RD_PTR    				; get the location index
        addlw    	low(UTON_BUFFER)   				; add start address of buffer
        	
        movwf   	FSR1L                    		; Point FSR1 to read location
        incf    	UTON_BUF_RD_PTR,F    			; Increment Read pointer for next time
        movlw   	UTON_BUFFER_SIZE  				; If read pointer has reached the maximum
        xorwf   	UTON_BUF_RD_PTR,W    			; value then reset it for roll-over
        btfsc   	STATUS,Z
        clrf    	UTON_BUF_RD_PTR      			; reset to 0

		BANKISEL	UTON_BUFFER						;
        movf   		INDF1,W                 			;Read a byte from the FIFO buffer data

		banksel		NUUNBUFSTAT					
        bcf     	NUUNBUFSTAT,U2NBF 	 			; ...at least room for one byte...
        bcf     	NUUNBUFSTAT,U2NBOF  			; and not overflowing either.
		BANKSEL		UTON_BUF_DATA_CNT
        decfSZ   	UTON_BUF_DATA_CNT,F  			; Decrement UTON_BUFFER data count since
		RETURN										; there is one more byte available 
		BSF			NUUNBUFSTAT,U2NBE				; DATA_CNT = 0, SO BUFER IS EMPTY. (SEEMS REDUNDANT)
	return





;---------------------------------------------------------------------------------------------
;---------------------------------------------------------------------------------------------
; Function: UARTIntPutCh
;
; PreCondition: None
;
; Overview:
;       It writes Content of W reg. in NTOU_BUFFER. 
;		If NTOU_BUFFER is full then NUUNBUFSTAT, N2UBF WILL BE SET. 
;		In that case, it waits for the interrupt routine TransmitData to execute, which will
;		clear the N2UBF flag.
;		
;		After writing to the buffer, it sets TXIE bit so that the TXIF interrupt will cause 
;		TransmitData to execute as soon as TXREG is ready.
;
; Input:    Data to be transmitted in W reg.
;
; Output:   If NTOU_BUFFER becomes full it sets N2UBF flag bit in
;       USARTErros
;
; Side Effects: Databank , FSR1, W , STATUS value changed
;
; Stack requirement: 1 level deep
;
;---------------------------------------------------------------------------------------------
UARTIntPutCh:

    GLOBAL  	UARTIntPutCh
    banksel 	temp3
    movwf   	temp3           				;Store Value in Temp. location
	BANKSEL		NUUNBUFSTAT		
    BTFSS   	NUUNBUFSTAT, N2UBF  	;Check for N2UBF bit
	GOTO		N2UBUFNOTFUL			
	
;	BANKSEL		PIE1
;	BTFSS		PIE1,	TXIE			; IF BUFFER'S FULL, TXIE BETTER BE SET...
;	BTFSS		INTCON,	GIE
;	RETURN
;	GOTO		$-9						;	WAIT TIL THE NTOU BUFFER HAS SPACE
;
TXREG_READY

N2UBUFNOTFUL
Append_N2U_Buffer
    	banksel		PIE1							; Turn off interrupt
    	BCF			PIE1,	TXIE					;
    	BTFSC		PIE1,	TXIE					;
    	GOTO		$-2          					; Don't allow interrupt, which could alter pointer
        banksel 	NTOU_BUFFER       					; 
        movfw    	NTOU_BUF_WT_PTR		   				; Pick a number...
        movwf   	FSR1L              		 			; Point FSR1 to Wrtie location
        incf    	NTOU_BUF_WT_PTR,F  					; Increment Write pointer for next write
        movlw   	(NTOU_BUFFER+NTOU_BUFFER_SIZE)		; If Write pointer has reached the maximum
        xorwf   	NTOU_BUF_WT_PTR,W    				; value then reset it for roll-over
        btfss   	STATUS,Z							; 
        goto		Chk_Buffer_Full						
        movlw		NTOU_BUFFER							;\  (ran out of numbers. Put in a new roll...)
        movwf    	NTOU_BUF_WT_PTR            			;  \reset the buffer write pointer

Chk_Buffer_Full
        incf    	NTOU_BUF_DATA_CNT,F 				;Increment NTOU_BUFFER data size
        movlw   	NTOU_BUFFER_SIZE  					; SEE IF BUFFER IS FULL
        xorwf   	NTOU_BUF_DATA_CNT,W 				;
        btfss   	STATUS,Z							;
        goto    	Copy_Byte_To_Buffer					; 
        bsf     	NUUNBUFSTAT,N2UBF    				; Buffer Is full. Set the buffer full flag

Copy_Byte_To_Buffer
		BANKSEL		temp3
        movf    	temp3,W
        movwf   	INDF1                    ;Copy the data into FIFO buffer
        banksel 	PIE1					;Enable TX interrupt, so when TXREG is ready
        bsf     	PIE1,TXIE       		;for more data .

UARTIntPutChrETURN
	return

;---------------------------------------------------------------------------------------------
;---------------------------------------------------------------------------------------------
; Function: UARTIntISR
;
; PreCondition: None
;
; Overview:
;       This is a Interrupt service routine for Serial Interrupt.
;       It handles Reception and Transmission of data on interrupt.
;       Call it from Interrupt service routine at proper Interrupt
;       Vector (High or Low priority Vector)
;
; Input:
;
;
; Output:   If data is received it puts it in UTON_BUFFER and accordingly
;       adjusts the RxBufferRdPtr and clears U2NBE flag.
;       If Receive Buffer becomes full then it will set U2NBF
;       bit. If data is received when Receive buffer was full it will
;       set U2NBOF flag to indicate that transmitted data has
;       been missed because of full UTON_BUFFER. If any error is
;       generated in reception it will set U2NERR flag bit.
;
;       If last data is transmitted then it will transmit next pending
;       data if any. It will accordingly adjust the NTOU_BUF_DATA_CNT. It
;       will clear the N2UBF bit to indicate space for data in
;       NTOU_BUFFER.
;
; Side Effects: Databank, W, STATUS changed FSR1
;
; Stack requirement: 1 level deep
;---------------------------------------------------------------------------------------------
UARTIntISR:
        GLOBAL  UARTIntISR
        banksel PIE1							; Maybe,
        btfsS   PIE1,TXIE       				; Check if TX int. enabled (Which is set if there is data in the N2U buffer)
		GOTO	ChkReceiver						; Not set, so this isn't the cause

        banksel PIR1
        btfss   PIR1,TXIF       				; Was it a Tx Interrupt?
		GOTO	ChkReceiver						; NO.
		
UART_READY_FOR_BUFFER_BYTE
 		banksel NTOU_BUF_DATA_CNT		;
		MOVF    NTOU_BUF_DATA_CNT, F	; AFFECT THE STATUS REGISTER
		SKIP_IF_ZERO
        goto    TransmitData			; IF THERE IS NO DATA IN THE BUFFER, JUST SENT IT TO TXREG

N_U_BUFFER_IS_EMPTY						;UART TXREG IS CAUGHT UP
		BSF		NUUNBUFSTAT, N2UBE 		;Place for possible Tx Buffer Empty flag
        banksel PIE1					;Disable TX interrupt until we have
        bcf     PIE1,TXIE       		;more data to transmit
    return							;



; LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
;	FUNCTION: TransmitData		
;	ACTIONS: (PRIMARY)	NTOU_BUFFER(NTOU_BUF_RD_PTR)-->TXREG
;			  (SECONDARY)	INCREMENT NTOU_BUF_RD_PTR
; LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
TransmitData		
		CLRF		FSR1H			;
        banksel		NTOU_BUF_RD_PTR    				;
        movfW   	NTOU_BUF_RD_PTR   			; get read ponter for FSR1
        movwf   	FSR1L                			; Point FSR1 to Write location
;------ NTOU_BUF_RD_PTR MAINTENANCE... ------------------
        incf    	NTOU_BUF_RD_PTR,F   			; Increment Read pointer for next read
        movlw   	NTOU_BUFFER						; If read pointer has reached the maximum
        addlw		NTOU_BUFFER_SIZE				; 
        xorwf   	NTOU_BUF_RD_PTR,W   			;value then reset it for roll-over
        btfss   	STATUS,Z						;
        goto 		skip_reset						;
        movlw		NTOU_BUFFER						; Starting address
        banksel		NTOU_BUF_RD_PTR   				; ....
        movwf		NTOU_BUF_RD_PTR   				; ....

skip_reset
        movf    	INDF1,W			; Get the byte from the register pointed at by the RD pointer
        banksel 	TXREG           ;Transmit Next Data
        movwf   	TXREG

		banksel 	NUUNBUFSTAT     			; TXIE is enabled. Clear N2UBF status bit as Data
        bcf     	NUUNBUFSTAT,N2UBF  			; is transmitted so Buffer has
                                				; space for new data
#ifdef	STATISTICS                                				      
        banksel 	BYTE_COUNT_TO_UART
        INCF		BYTE_COUNT_TO_UART+1
        BTFSC		STATUS,	Z
        INCF		BYTE_COUNT_TO_UART
#endif        
		BANKSEL		NTOU_BUF_DATA_CNT
        decf	  	NTOU_BUF_DATA_CNT,F    	; 	ON THE NEXT INTERRUPT, TH
	  return           				
    

ChkReceiver
        banksel PIR1
        btfsS   PIR1,RCIF       	;Check for Receive Interrupt Flag
        RETURN						; NOTHING RECEIVED
              
		BANKSEL	CONFIG_STATUS_REG
		BSF		CONFIG_STATUS_REG,	UART_RCIF_ASSERTED

        banksel RCREG				; 
        movf    RCREG,W         	; Read RCREG data to clear the interrupt
        banksel TEMP1   			; Get the data
        movwf   TEMP1				; ...
        
        
; !!!!!!!!!!!!!CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!        
;lllllllllllll VECTOR GENERATOR BELOW. DONT ALTER! llllllllllllllllllllllllllllllllllllllllllllllllllll
	PAGESEL	NO_REC_ERROR
        banksel 	RCSTA
        movlw   	b'00000110'     ;Mask out unwanted bits, KEEP OVERRUN ERROR AND FRAMING ERROR
        andwf   	RCSTA,W         ;Check for errors
        ADDWF		PCL
		GOTO		NO_REC_ERROR			; 0...NO ERRORS, WREG S/B 0
		NOP									; 1...PLACEHOLDE
		FARCALL		RECORD_OVER_RUN_ERROR	; 2...OVERRUN, WREG S/B b'10'
		GOTO		REC_ERROR_RECORDED		; ...PASS ON THE INFORMATION ANYWAY
		FARCALL		RecordFrameError		; 4...FRAMING ERROR, WREG S/B b'100'
		GOTO		REC_ERROR_RECORDED		; ...PASS ON THE INFORMATION ANYWAY
;lllllllllllll VECTOR GENERATOR ABOVE. DONT ALTER! llllllllllllllllllllllllllllllllllllllllll
;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
		GOTO	$-1
		
		FARCALL		RecordFrameError		; 4
REC_ERROR_RECORDED
SAFE_UTON_BUFF_APPEND
NO_REC_ERROR	; OR
        banksel NUUNBUFSTAT     ;
        btfsc   NUUNBUFSTAT,U2NBOF  	;Check for UTONBuffer overflow
	  RETLW	0X03           				

        btfss   NUUNBUFSTAT,U2NBF  ;Check for U2NBF bit
        goto    AppendUTONBuffer   				;if Buffer Full then set Rx Buffer
        bsf     NUUNBUFSTAT,U2NBOF  	;Over Flow flag to indicate that
										;data is missed.
	  RETLW	0X03
AppendUTONBuffer
        banksel UTON_BUF_WT_PTR			;The  segments ensures that whole
        movf    UTON_BUF_WT_PTR,W    	;buffer will be in one bank. Therefore
        addlw   low(UTON_BUFFER)   		;setting IRP bit based on buffer
                                        ;start address is OK     
        movwf   FSR1                     ;Point FSR1 to Wrtie location
        incf    UTON_BUF_WT_PTR,F    	;Increment Write pointer
        movlw   UTON_BUFFER_SIZE  		;If Write pointer has reached the maximum
        xorwf   UTON_BUF_WT_PTR,W    	;value then reset it for roll-over
        btfsc   STATUS,Z				
        clrf    UTON_BUF_WT_PTR      

        bcf     NUUNBUFSTAT, U2NBE	; 
        incf    UTON_BUF_DATA_CNT,F  	;Increment UTON_BUFFER data size
        movlw   UTON_BUFFER_SIZE  		;If Buffer has reached the maximum
        xorwf   UTON_BUF_DATA_CNT,W  	;value then set the flag for full
        btfsc   STATUS,Z				;
        bsf     NUUNBUFSTAT,U2NBF    	;in NUUNBUFSTAT

		BANKSEL	TEMP1
        movf    TEMP1,W					;
		CLRF	FSR1H			;
        movwf   INDF1                    ;Copy the data into FIFO buffer

#ifdef	STATISTICS        
        BANKSEL	BYTE_COUNT_FROM_UART
        INCF	BYTE_COUNT_FROM_UART
        BTFSC	STATUS,	Z
        INCF	BYTE_COUNT_FROM_UART+1
#endif
        RETURN					;

RcvError
        banksel RCSTA          
		btfss	RCSTA, OERR
		goto	RecordFrameError
RECORD_OVER_RUN_ERROR
        banksel NUUNBUFSTAT	;
        bsf     NUUNBUFSTAT,U2NERR ;Set Data Error flag
        BANKSEL	UART_OFLO_ERR_COUNT
        incfsz	UART_OFLO_ERR_COUNT+1
        goto 	$+2
        incf	UART_OFLO_ERR_COUNT
        banksel RCSTA          
        bcf     RCSTA,CREN      ;Clear reciever status
        bsf     RCSTA,CREN		; Overrun error (can be cleared by clearing bit CREN)       
 		RETURN

RecordFrameError
        banksel RCSTA          
		btfss	RCSTA, FERR
        banksel NUUNBUFSTAT	;
        bsf     NUUNBUFSTAT,U2NERR ;Set Data Error flag
        banksel	UART_FRM_ERR_COUNT
        incfsz	UART_FRM_ERR_COUNT+1
        goto 	$+2
		INCF	UART_FRM_ERR_COUNT
	RETURN

EndISR
        return

BUFFER_HANDLER_ASLEEP
	BANKSEL	PIE1
	BSF		PIE1,	TXIE
	RETURN

RESET_N2U_U2N_BUF
		BANKSEL	NUUNBUFSTAT
		CLRF	NTOU_BUF_DATA_CNT   
		movlw	low(NTOU_BUFFER)
		movwf	NTOU_BUF_WT_PTR     
		movwf	NTOU_BUF_RD_PTR     
		movlw	0x22				; set the buffer empty bits, clear everything else
		movwf	NUUNBUFSTAT

		FARCALL	PRINTF_BUFFER_RESET
	RETURN	

;---------------------------------------------------------------------------------------------

;SAFE_UARTIntPutCh
;	BANKSEL	NUUNBUFSTAT
;	BTFSC	NUUNBUFSTAT,	N2UBF
;	GOTO	$-1
;	FARCALL	UARTIntPutCh
;RETURN
;
;---------------------------------------------------------------------------------------------
;	FUNCTION: 	 IMMEDIATE_PRINTF
;	DESCRIPTION: PRINTS A STORED MESSAGE TO THE SCREEN
;	PREREQUISITE:TBL_START_PTR MUST BE INITIALIZED WITH THE LOCATION 
;					OF THE MESSAGE ARRAY CREATED BY THE 'dt' ASSEMBLER DIRECTIVE
;	AFFECTS: PCLATH, WREG, BK_INDEX_REG, STATUS
;	CALLS:	GET_TABLE_BYTE, SEND_CHAR_TO_SERIAL_PORT
;---------------------------------------------------------------------------------------------
IMMEDIATE_PRINTF 
		CLRF		BK_INDEX_REG			;INDEXER
MSG_LOOP
	MOVFW	BK_INDEX_REG
	FARCALL	GET_BYTE_W_FROM_TABLE		; NEED TO BE A CALL TO GIVE RETLW SOMEPLACE TO GO
	;FARCALL	SEND_CHAR_TO_SERIAL_PORT
	FARCALL	SAFE_UART_PUT_CHAR	
	INCF	BK_INDEX_REG
	XORLW	0X03		
	SKIP_IF_ZERO				; END OF TRANSMISSION?
	GOTO	MSG_LOOP			; GOTO MSG_LOOP. ADJUST AFTER EDITING THIS CODE
	BSF		INTCON,		GIE
	RETURN						;YES. DONE









;---------------------------------------------------------------------------------------------
;	FUNCTION: 	 SEND_FILE_TO_CHAR_PORT
;	DESC:	READS THE CONTENTS OF THE FILE POINTED TO BY THE FSR1 AND (TYPICALLY) PRINTS IT TO THE SCREEN
;	PRE-REQUISITE:	INITIALIZE THE FSR1
;	AFFECTS:	PCL. LEAVES THE W REG INTACT
;---------------------------------------------------------------------------------------------
SEND_CHAR_TO_SERIAL_PORT
        banksel		PIR1
        btfss		PIR1,	TXIF	; IS THE TXREG READY TO RECEIVE ANOTHER BYTE?
        GOTO	$-1					; WAIT TIL IT'S READY
        banksel 	TXREG							
        movwf   	TXREG           ;Immediately Transmit Data
 return      	            	 	;Our work here is done

;---------------------------------------------------------------------------------------------
;	FUNCTION: 	 SEND_FILE_TO_SERIAL_PORT
;	DESC:	READS THE CONTENTS OF THE FILE WHOSE ADDRESS MATCHES WREG AND (TYPICALLY) PRINTS IT TO THE SCREEN
;	PRE-REQUISITE:	INITIALIZE WREG WITH DESIRED FILE'S ADDRESS
;	AFFECTS:	PCL, WREG 
;---------------------------------------------------------------------------------------------
SEND_FILE_TO_SERIAL_PORT
		MOVWF		FSR1L
		movfw		INDF1
		CALL		SAFE_UART_PUT_CHAR
 return      	            	 	;Our work here is done
 
 ;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
;	FUNCTION: HEX2UART
;	DESCRIPTION: OUTPUTS THE VALUE OF WREG TO THE SERIAL PORT, FORMATTED AS A HEXIDECIMAL VALUE
;	AFFECTS: W, STATUS, HEX2UARTREG
;LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
HEXTOUART:
		GLOBAL	HEXTOUART
		BANKSEL	HEX2UARTREG
		MOVWF	HEX2UARTREG
		MOVLW	"0"
		CALL	SAFE_UART_PUT_CHAR
		MOVLW	"x"
	
		CALL	SAFE_UART_PUT_CHAR
	
		BANKSEL	HEX2UARTREG
		SWAPF	HEX2UARTREG, 0		;HI NIBBLE
		CALL	HEXTOASC
		CALL	SAFE_UART_PUT_CHAR
	
		BANKSEL	HEX2UARTREG
		MOVFW	HEX2UARTREG
		CALL	HEXTOASC
		CALL	SAFE_UART_PUT_CHAR
	
		MOVLW	0X0A				; NEW LINE
		CALL	SAFE_UART_PUT_CHAR
		
		MOVLW	0X0D				; CARRIAGE RETURN
		CALL	SAFE_UART_PUT_CHAR
	RETURN

;	HEXTOASC: CONVERTS THE HEX VALUE IN THE WREG TO ITS ASCII EQUIVALANT
;	REGISTERS AFFECTED: WREG, STATUS
HEXTOASC
		ANDLW	0X0F			;Isolate the low 4 bits									[14]
		ADDLW	-.10			;Value greater than 10?									[4]
		SKPNC					;Skip if not
		ADDLW	.7				;Else adjust for 'A'..'F' (sHOULD BE 65-48=17, OR 0X11)	[4+65-48=21]
		ADDLW	"0" +.10		;Convert to ASCII	(48 + 10 = 58, OR 0X3A 				[21+48+10=79]
	RETURN

SAFE_UART_PUT_CHAR
		call	SEND_CHAR_TO_SERIAL_PORT
	return
	
		BANKSEL		NUUNBUFSTAT		
	    BTFSC   	NUUNBUFSTAT, N2UBF  	;Check for N2UBF bit
	    goto		check_ints
	    BCF			INTCON,	GIE
		FARCALL		UARTIntPutCh
	    BSF			INTCON,	GIE
		return
check_ints
		BTFSS		PIR1,	TXIF
		GOTO		SAFE_UART_PUT_CHAR
		BSF			PIR1,	TXIE
		BSF			INTCON,	GIE
		GOTO		SAFE_UART_PUT_CHAR
	RETURN
#ENDIF