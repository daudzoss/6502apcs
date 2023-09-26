;;;                      +-------------------------------------------+
;;; $0100|SP'' = $0102+X |               LSB of argument             | A0FUNCT
;;;                      +---------------------+---------------------+
;;;              $0101+X | old value of X e.g. SP'' from a prior time|
;;;                      +-------------------------------------------+
;;; $0100|X    = $0100+X | high byte of return address from multwrp()|
;;;                      +-------------------------------------------+
;;;              $00ff+X | low byte of return address from multwrp() |
;;;                      +-------------------------------------------+
;;;              $00fe+X |                   rtval                   | V0LOCAL
;;;                      +-------------------------------------------+
;;;              $00fd+X |                     z                     | V1LOCAL

log2:	ADRHA	ZPLOG		;register int8_t log2(register int8_t y, vararg)
	ADRLA	ZPLOG,0		;{
	tya			; uint8_t* zplog = &vararg;
	pha			; int8_t rtval = y; // floor(log2(vararg)) or -1
	pha			; uint8_t z; // bits left to process within byte
	lsr			;
	lsr			;
	lsr			;
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
	dec @w	V0LOCAL	;//rtval; return -1; // all zeros
+	ldy @w	V0LOCAL	;//rtval;
	POPVARS			;
	rts			;}
