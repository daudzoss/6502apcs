divwrp: lda 	A0FUNCT	;//sizes;register int8_t divwrp(register int8_t y,
	and	#$0f		;         uint8_t sizes, uint?_t a, uint?_t b) {
	clc			;
	adc	#1		; // <= 16 so c==0
	pha			; uint8_t asize = (size& 0x0f)+1; // V0LOCAL
	lda 	A0FUNCT	;//sizes;
	lsr			;
	lsr			;
	lsr			;
	lsr			;
	;clc			;
	adc	#1    		; // <= 16 so c==0
	pha			; uint8_t bsize = ((size& 0xf0)>>4)+1;// V1LOCAL
	;clc			;

	tay			;
	adc @w	V0LOCAL	;//asiz	; // <= 32 so c==0
	pha			; uint8_t sizes = asize + bsize; // V2LOCAL

	;; save y and prepare to store a and b signs in V and N bit respectively
	tya			;
	pha			; int8_t savey = y; // V3LOCAL
	ldy	#0		; y = 0; // return value is zero until checks OK
	tya			;
	pha			; uint8_t sgnba = 0; // V4LOCAL
	pha			; uint8_t non0 = 0; // V5LOCAL
	pha			; uint8_t rtval = 0; // V6LOCAL

	;; check for stack underrun: end of args must still be on page 1
	txa			;
	;clc			;
	adc @w	V2LOCAL	;//sizes;
	bcc	+		;
-	jmp	merror		;
+	cmp	#$fe		; if (x + sizes + 2 > 0x00ff)
	bcs	-		;  return y;

	;; check for stack overrun: need sizes+TBD stack bytes left for VnLOCAL's
	txa			;
	sec			;
	sbc @w	V2LOCAL	;//sizes;
	cmp	#$a		; if (x - sizes < TBD)
	bcs	+		;
	jmp	merror		;  return y;

	;; find MSB of b, store sign bit, allocate bsize of stack space for it
	ldy @w	V2LOCAL	;//sizes;
	dey			;
	lda	(ZPDIV),y	;
	rol			;
	ror @w	V4LOCAL	;//sgnba; sgnba |= (b & (1<<(bsize*8 - 1))) ? 0x80 : 0;

	lda @w	V0LOCAL	;//asize;
	clc			;
	adc	ZPDIV		;
	sta	ZPDIVB		;
	lda	#1		;
	sta	ZPDIVB+1	;

	lda	#0		;
	sta @w	V5LOCAL	;//non0 ;
	ldy @w	V1LOCAL	;//bsize;
-	dey			;
	lda	(ZPDIVB),y	;
	pha			;
	ora @w	V5LOCAL	;//non0 ;
	sta @w	V5LOCAL	;//non0 ;
	tya			;
	bne	-		; uint8_t bwork[bsize];
	lda @w	V5LOCAL	;//non0 ;
	beq	zerodiv		;
	
	txa			;
	pha			; // vvv push
	tsx			;
	inx			;
	inx			;
	stx	ZPDIVB		;
	pla			; // ^^^ pull
	tax			; uint?_t* zpdivb = (void*) bwork; *zpdivb = b;
	lda @w	V4LOCAL	;//sgnba;
	bpl	+		; if (b < 0) {
	;ldy 	#0		;
	sec			;
-	lda	(ZPDIVB),y	;
	eor	#$ff		;
	adc	#0		;
	sta	(ZPDIVB),y	;
	tya			;
	cmp @w	V1LOCAL	;//bsize;  *zpdivb = -b;
	bcc	-		; }
	
	;; find floor(log2(b)) to aid in initial guess
+	lda @w	V1LOCAL	;//bsize;
	tay			;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	sty @w	V6LOCAL	;//rtval; rtval = log2(*zpdivb);
	
	;; find msb of a, store sign bit, allocate asize of stack space
+	ADRHA	ZPDIV		;
	ADRLA	ZPDIV,1	;
	ldy @w	V0LOCAL	;//asize;
	dey			;
	lda	(ZPDIV),y	;
	rol			;
	ror @w	V4LOCAL	;//sgnba; sgnba |= (a & (1<<(asize*8 - 1))) ? 0x40 : 0;

	lda	#0		;
	sta @w	V5LOCAL	;//non0 ;
	ldy @w	V0LOCAL	;//asize;
-	dey			;
	lda 	(ZPDIV),y	;
	pha			;
	ora @w	V5LOCAL	;//non0 ;
	sta @w	V5LOCAL	;//non0 ;
	tya			;
	bne	-		; uint8_t awork[sizes];
	lda @w	V5LOCAL	;//non0	;
	beq	divby0		;
	
	txa			;
	pha			; // vvv push
	tsx			;
	inx			;
	inx			;
	stx	ZPDIVA	  	;
	lda	#1		;
	sta	1+ZPDIVA	;
	pla			; // ^^^ pull
	tax			; uint?_t* zpdiva = (void*) awork; *zpdiva = a;


	lda @w	V4LOCAL	;//sgnba;
	bpl	+		; if (a < 0) {
	;ldy 	#0		;
	sec			;
-	lda	(ZPDIVA),y	;
	eor	#$ff		;
	adc	#0		;
	sta	(ZPDIVA),y	;
	tya			;
	cmp @w	V1LOCAL	;//bsize;  *zpdiva = -a;
	bcc	-		; }

	;; begin code NOT copied from mult.inc
	
	;; find floor(log2(a)) to aid in initial guess
+	lda @w	V0LOCAL	;//asize;
	tay			;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	sty @w	V5LOCAL	;//non0	; non0 = log2(*zpdiva);

	lda @w	V6LOCAL	;//rtval;
	sec			;
	sbc @w	V5LOCAL	;//non0	;
	sta @w	V6LOCAL	;//rtval;
	bmi	zerodiv		; if (log2(*zpdiva) <= log2(*zpdivb)) { // non-0

usdiv:	ldy @w	V2LOCAL	;//sizes;
-	lda	#0		;
	dey			;
	sta	(ZPDIV),y	;  a = b = 0; // to hold final quotient/remainder
	tya			;
	bne	-		; 
	ADRHA	ZPLOG		;
	ADRLA	ZPLOG,1		;
	ldy @w	V6LOCAL	;//rtval;
	FUNCALL			;  // initial guess based on difference of logs
	jsr	two2the		;  two2the(log2(*zpdivb) - log2(*zpdiva), a);
	FUNRETN			;
