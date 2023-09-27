;;;                      +-------------------------------------------+
;;; $0100|SP'' = $0102+X |               LSB of argument             | A0FUNCT
;;;                      +---------------------+---------------------+
;;;              $0101+X | old value of X e.g. SP'' from a prior time|
;;;                      +-------------------------------------------+
;;; $0100|X    = $0100+X |  high byte of return address from log2()  |
;;;                      +-------------------------------------------+
;;;              $00ff+X |  low byte of return address from log2()   |
;;;                      +-------------------------------------------+
;;;              $00fe+X |                   rtval                   | V0LOCAL
;;;                      +-------------------------------------------+
;;;              $00fd+X |                     z                     | V1LOCAL

log2:	ADRHA	ZPLOG		;register int8_t log2(register int8_t y, vararg)
	ADRLA	ZPLOG,0		;{
log2ptr:tya			; uint8_t* zplog = &vararg;
	pha			; int8_t rtval = y; // floor(log2(vararg)) or -1
	pha			; uint8_t z; // bits left to process within byte
	lsr			;
	lsr			;
	lsr			; // y specifies bits in argument
	tay			;
nexty:	dey			; for (y /= 8; y; y--) // each byte in argument
	lda	#8		;
	sta @w	V1LOCAL	;//z	;  for (z = 8; z; z--) { // each bit in byte
	lda	(ZPLOG),y	;   register uint8_t a = zplog[y-1], c;
	
-	dec @w	V0LOCAL	;//rtval;   rtval--; // starts at e.g. 32, until first 1
	asl			;   c = a & 0x80, a <<= 1;
	bcs	+		;   if (c)
	dec @w	V1LOCAL	;//z	;    return y = rtval;
	bne	-		;  }
	tya			;
	bne	nexty		;
	dec @w	V0LOCAL	;//rtval; return y = -1; // all zeros
+	ldy @w	V0LOCAL	;//rtval;
	POPVARS			;
	rts			;} // log2()

;;;                      +-------------------------------------------+
;;; $0100|SP'' = $0102+X |                LSB of result              | A0FUNCT
;;;                      +---------------------+---------------------+
;;;              $0101+X | old value of X e.g. SP'' from a prior time|
;;;                      +-------------------------------------------+
;;; $0100|X    = $0100+X | high byte of return address from two2the()|
;;;                      +-------------------------------------------+
;;;              $00ff+X | low byte of return address from two2the() |
;;;                      +-------------------------------------------+
;;;              $00fe+X |                   bitof                   | V0LOCAL
;;;                      +-------------------------------------------+

two2the	ADRHA	ZPLOG		;void two2the(register uint8_t y, vararg) {
	ADRLA	ZPLOG,0		; uint8_t* zplog = &vararg;
	tya			;
	and	#$07		;
	clc			;
	adc	#1		;
	pha			; uint8_t bitof = (y & 7) + 1; // V0LOCAL
	tya			;
	lsr			;
	lsr			;
	lsr			;
	tay			; y /= 8; // byte to set nonzero
	lda	#0		;
	sec			;
-	rol			;
	dec @w	V0LOCAL	;//bitof;
	bne	-		;
	sta	(ZPLOG),y	; zplog[y-1] = 1 << (bitof-1);
	POPVARS			;
	rts			;} // two2the()
