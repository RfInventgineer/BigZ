
#ifndef SPI
#defINE SPI

#ifndef nrf24L01_DEF
#include nrf24L01_DEF.INC
#defINE nrf24L01_DEF
#ENDIF


SPI_CODE:	CODE	; 0X718
SPI_INIT
	;***********************************************;
	;SPI_SETUP
	;***********************************************;
		BANKSEL	SSPCON
		CLRF	SSPCON
		BCF		SSPCON,SSPM3
		BCF		SSPCON,SSPM2			
		BCF		SSPCON,SSPM1			
	    BCF		SSPCON,SSPM0			;	SSPM<3:0> = 0000 = SPI Master mode, clock = FOSC/4 (4MHz)
	    BCF     SSPCON,CKP              ;   CLOCK LOW WHEN IDLE
	    BSF		SSPCON,SSPEN			;	1 = Enables serial port and configures 
	    								;	SCK, SDO, SDI and SS as the source of the serial port pins
	    BCF		SSPCON,SSPOV			;	CLEAR THE Receive Overflow Indicator bit
	    BCF		SSPCON,	WCOL			;   CLEAR THE Write Collision Detect bit
	    								
	
		BANKSEL	SSPSTAT                 ;
	    BCF     SSPSTAT,SMP             ;SAMPLE DATA IN THE MIDDLE OF THE DATA PERIOD
	    BSF     SSPSTAT,CKE				;0 = Data transmitted on falling edge of SCK
    

		BANKSEL		SSPBUF		
		MOVFW		SSPBUF				; JUST CHECKING IT'S OPERATION HERE...; 
		BANKSEL 	SSPSTAT				; READING THE SSPBUF SHOULD HAVE CLEARED THE BF FLAG
		BTFSC		SSPSTAT,BF			;  > WAIT FOR END OF TRANSMISSION
		GOTO		$-1					; /

		BANKSEL		SSPBUF		
		MOVWF		SSPBUF				; WRITE TO THE SSP AND WAIT FOR BF TO INDICATE IT IS FINISHED
		BANKSEL 	SSPSTAT				; \
		BTFSS		SSPSTAT,BF			;  > BF SHOULD GO HIGH AFTER A WRITE
		GOTO		$-1					; /
		
		BANKSEL		SSPBUF		
		MOVWF		SSPBUF				; JUST CHECKING IT'S OPERATION HERE...; 
		BANKSEL 	SSPSTAT				; \
		BTFSS		SSPSTAT,BF			;  > WAIT FOR END OF TRANSMISSION
		GOTO		$-1					; /

		NOP								; IF WE GET HERE IT'S GOOD TO GO
		
	RETURN


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;FUNCTION: SPI_TRANSFER
;	SENDS THE CONTENTS OF THE W REG TO THE SPI
;	RETURNS: CONTENTS OF SSPBUF IN THE W REG.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SPI_TRANSFER
	BANKSEL 	PIR1
	BTFSC		PIR1,	SSPIF		;	Check/clear ssp interrupt flag
	BCF			PIR1,	SSPIF
	BANKSEL		SSPBUF				; > read to clear the BF flag (Although it seems to clear when BTF'd...
	XORWF		SSPBUF, W			;\ CLEARS THE BF FLAG...
	XORWF		SSPBUF, F			; > SSPBUF ==  W'S ORIGINAL VALUE
BYTE_WRIT
; TODO: see if the next four lines can be removed.
	CLRF	BKLS_REG2				; TIME-OUT counter for debugging. 
	decfsz	BKLS_REG2				; No, wait some ore
	GOTO	$+2						; loop
	NOP								; nop
	BANKSEL PIR1					; 
	BTFSS	PIR1,	SSPIF			; is SSP transfer completed?
	goto	$-4						; No
	
	BANKSEL	SSPBUF					; transfer complete
	MOVFW	SSPBUF					; put NRF response in WREG
	BANKSEL	PIR1					; housekeep
	BCF	PIR1,	SSPIF				; clear the interrupt
  RETURN

CLEARBF:
	BANKSEL		SSPBUF				; > read to clear the BF flag (Although it seems to clear when BTF'd...
	XORWF		SSPBUF, W				;/
	XORWF		SSPBUF, W				;/
  RETURN

;
;BANK_GOT_CHANGED
;	RETURN
;D-BUGD-BUGD-BUGD-BUGD-BUGD-BUGD-BUGD-BUGD-BUGD-BUGD-BUG

SPI_SUCCESS
	BANKSEL	SSPBUF				; Once it is, read the Buffer into the WREG
	MOVF	SSPBUF, W			;CLEARS BUFFER FULL, AND LEAVES A BYTE FROM THE NRF IN THE WREG

  Return											; and return.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	FUNCTION: SPI_OUT
;
Spi_Put
		BANKSEL SSPBUF
		MOVWF	SSPBUF							; STARTS TRANSMISSION
        btfss   SSPCON,WCOL                     ; Check for a write collision
        retlw   000h                            ; No collision
        bcf     SSPCON,WCOL                     ; Clear the collision
        retlw   SPIMErrWriteCollision           ; indicates Write Collision Error


	
Spi_Get
		BANKSEL	SSPBUF				
		MOVF	SSPBUF, W			;CLEARS BUFFER FULL, AND LEAVES A BYTE FROM THE NRF IN THE WREG
	RETURN 

	#endif
