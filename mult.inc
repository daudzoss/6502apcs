;;; 	                    7    6    5    4       3    2    1    0      
;;;                      +-------------------------------------------+
;;;              ...     |variables in context of caller of multwrp()|
;;;                      +-------------------------------------------+
;;;A1FUNCT+asize+bsize-1 |      MSB of b  ---------> MSB of product  |
;;;                      +-------------------------------------------+
;;;              ...     |      ...                                  |
;;;                      +-------------------------------------------+
;;;  A1FUNCT + asize     |      LSB of b                             |
;;;                      +-------------------------------------------+
;;;  A1FUNCT + asize - 1 |      MSB of a                             |
;;;                      +-------------------------------------------+
;;;              ...     |      ...                                  |
;;;                      +-------------------------------------------+
;;; $0100|SP'' = $0103+x |      LSB of a  ---------> LSB of product  | A1FUNCT
;;;                      +---------------------+---------------------+
;;;              $0102+X |      bsize - 1      |    asize - 1        | A0FUNCT
;;;                      +---------------------+---------------------+
;;;              $0101+X | old value of X e.g. SP'' from a prior time|
;;;                      +-------------------------------------------+
;;; $0100|X    = $0100+X | high byte of return address from multwrp()|
;;;                      +-------------------------------------------+
;;;              $00ff+X | low byte of return address from multwrp() |
;;;                      +-------------------------------------------+
;;; $0100|SP'  = $00fe+X |                   asize                   | V0LOCAL
;;;                      +-------------------------------------------+
;;;              $00fd+X |                   bsize                   | V1LOCAL
;;;                      +-------------------------------------------+
;;;              $00fc+X |                   sizes                   | V2LOCAL
;;;                      +-------------------------------------------+
;;;              $00fb+X |                   savey                   | V3LOCAL
;;;                      +-------------------------------------------+
;;;              $00fa+X |                   sgnba                   | V4LOCAL
;;;                      +-------------------------------------------+
;;;              $00f9+X |                   bnon0                   | V5LOCAL
;;;                      +-------------------------------------------+
;;;              $00f8+X |                   rtval                   | V6LOCAL

USIGNED	= $00
SIGNED	= $80

USIGN8	= $00			; 1 byte - 1 = 0
SIGN8	= SIGNED | USIGN8	; bit 7 set

USIGN16	= $01			; 2 bytes - 1 = 1
SIGN16	= SIGNED | USIGN16	; bit 7 set

USIGN24	= $02			; 3 bytes - 1 = 2
SIGN24	= SIGNED | USIGN24	; bit 7 set
	
USIGN32	= $03			; 4 bytes - 1 = 3
SIGN32	= SIGNED | USIGN32	; bit 7 set

USIGN40	= $04
SIGN40	= SIGNED | USIGN40

USIGN48	= $05
SIGN48	= SIGNED | USIGN48

USIGN56	= $06
SIGN56	= SIGNED | USIGN56

USIGN64	= $07
SIGN64	= SIGNED | USIGN64

USIGN72	= $08
SIGN72	= SIGNED | USIGN72

USIGN_a	= $09
SIGN_a	= SIGNED | USIGN_a

USIGN_b	= $0a
SIGN_b	= SIGNED | USIGN_b

USIGN_c	= $0b
SIGN_c	= SIGNED | USIGN_c

USIGN_d	= $0c
SIGN_d	= SIGNED | USIGN_d

USIGN_e	= $0d
SIGN_e	= SIGNED | USIGN_e

USIGN_f	= $0e
SIGN_f	= SIGNED | USIGN_f

USIGN80	= $0f
SIGN80	= SIGNED | USIGN80
SIGN128	= SIGN80	

mult	.macro
	tya			;inline register int8_t mult(int8_t i,int8_t j){
	and	#~SIGNED	;
	ora	#(\2|\1)&SIGNED	; register int8_t y;
	tay			; y = (j | i) & 0x80; // signed if either signed
	lda #((\1<<4)|(\2&$0f))&$ff;
	pha			; uint8_t sizes = ((j & 0x0f)<<4) | (i & 0x0f);
	FUNCALL			;
	jsr	multwrp		; // stack bytes used:  1  i+1 j+1
	FUNRETN			; return y = multwrp(y, sizes, a, b);// push b,a
	pla			;} // mult()
	.endm
	;; TO-DO: allow y to specify something useful in bits 0-6

multwrp:lda 	A0FUNCT	;//sizes;register int8_t multwrp(register int8_t y,
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
	adc @w	V0LOCAL	;//asiz	; // <= 32 so c==0
	pha			; uint8_t sizes = asize + bsize; // V2LOCAL

	;; save y and prepare to store a and b signs in V and N bit respectively
	tya			;
	pha			; int8_t savey = y; // V3LOCAL
	ldy	#0		; y = 0; // return value is zero until checks OK
	tya			;
	pha			; uint8_t sgnba = 0; // V4LOCAL
	pha			; uint8_t bnon0 = 0; // V5LOCAL
	pha			; uint8_t rtval = 0; // V6LOCAL

	;; check for stack underrun: end of args must still be on page 1
	txa			;
	;clc			;
	adc @w	V2LOCAL	;//sizes;
	bcc	+		;
-	jmp	merror		;
+	cmp	#$fe		; if (x + sizes + 2 > 0x00ff)
	bcs	-		;  return y;

	;; check for stack overrun: need sizes+10 stack bytes left for VnLOCAL's
	txa			;
	sec			;
	sbc @w	V2LOCAL	;//sizes;
	cmp	#$a		; if (x - sizes < 10)
	bcs	+		;
	jmp	merror		;  return y;

	;; find msb of a, store sign bit, allocate asize+bsize of stack space
+	ADRHA	ZPMULT		;
	ADRLA	ZPMULT,1	;
	ldy @w	V0LOCAL	;//asize;
	dey			;
	lda	(ZPMULT),y	;
	rol			;
	ror @w	V4LOCAL	;//sgnba; sgnba |= (a & (1<<(asize*8 - 1))) ? 0x40 : 0;

	ldy @w	V1LOCAL	;//bsize;
	lda	#0		;
-	pha			;
	dey			;
	bne	-		;
	ldy @w	V0LOCAL	;//asize;
-	dey			;
	lda 	(ZPMULT),y	;
	pha			;
	tya			;
	bne	-		; uint8_t awork[sizes];
	
	txa			;
	pha			; // vvv push
	tsx			;
	inx			;
	inx			;
	stx	ZPMULA	  	;
	lda	#1		;
	sta	1+ZPMULA	;
	pla			; // ^^^ pull
	tax			; uint?_t* zpmula = (void*) awork; *zpmula = a;

	;; find MSB of b, store sign bit, allocate bsize of stack space for it
	ldy @w	V2LOCAL	;//sizes;
	dey			;
	lda	(ZPMULT),y	;
	rol			;
	ror @w	V4LOCAL	;//sgnba; sgnba |= (b & (1<<(bsize*8 - 1))) ? 0x80 : 0;

	lda @w	V0LOCAL	;//asize;
	clc			;
	adc	ZPMULT		;
	sta	ZPMULB		;
	lda	#1		;
	sta	ZPMULB+1	;

	ldy @w	V1LOCAL	;//bsize;
-	dey			;
	lda	(ZPMULB),y	;
	pha			;
	tya			;
	bne	-		; uint8_t bwork[bsize];
	txa			;
	pha			; // vvv push
	tsx			;
	inx			;
	inx			;
	stx	ZPMULB		;
	pla			; // ^^^ pull
	tax			; uint?_t* zpmulb = (void*) bwork; *zpmulb = b;
; lda #"a"
; jsr $ffd2
; lda #"="
; jsr $ffd2
; ldy @w V2LOCAL
;- dey
; tya
; pha
; lda (ZPMULA),y	
; tay
; FUNCALL
; jsr putbin
; FUNRETN
; pla
; tay
; bne - 
; lda #$d
; jsr $ffd2
	
; lda #"b"
; jsr $ffd2
; lda #"="
; jsr $ffd2
; ldy @w V1LOCAL
;- dey
; tya
; pha
; lda (ZPMULB),y	
; tay
; FUNCALL
; jsr putbin
; FUNRETN
; pla
; tay
; bne -
; lda #$d
; jsr $ffd2

	;; check for signed or unsigned mode
	lda @w	V3LOCAL	;//savey; if ((savey & SIGNED)  // signed mode requested
	bpl	usmult		;     &&                        // and
smult:	lda @w	V4LOCAL	;//sgnba;     ((a&(1<<asize*8 - 1)) ||  // a negative or
	beq	usmult		;      (b&(1<<bsize*8 - 1)))) { // b negative

	and	#$40		;
	beq	smult2		;  if (a < 0) { //a = -a, sign-extended to sizes
	sec			;   register uint8_t c = 1, y;
	ldy	#0		;   for (y = 0; y < asize; y++) {
	php			;    // vvv push
-	plp			;    // ^^^ pull
	lda	(ZPMULA),y	;
	eor	#$ff		;    register uint9_t sum = ~(a[y]) + c;
	adc	#0		;    c = sum >> 8;
	php			;    // vvv push
	sta	(ZPMULA),y	;    awork[y] = sum & 0xff;
	iny			;
	tya			;
	cmp @w	V0LOCAL	;//asize;
	bcc	-		;
	pla			;   } // ^^^ pull

-	lda	#$00		;   for (; y < sizes; y++)
	sta	(ZPMULA),y	;    awork[y] = 0x00;
	iny			;
	tya			;
	cmp @w	V2LOCAL	;//sizes;
	bcc	-		;
	lda @w	V4LOCAL	;//sgnba;  }
	bpl	usmult		;  if (b < 0) { //b = -b, not sign-extended

smult2:	lda	ZPMULT		;
	clc			;
	adc @w	V0LOCAL	;//asize;
	sta	ZPMULT		;

	sec			;   register uint8_t c = 1, y;
	ldy	#0		;   for (y = 0; y < bsize; y++) {
	php			;    // vvv push
-	plp			;    // ^^^ pull
	lda	(ZPMULT),y	;
	eor	#$ff		;    register uint9_t sum = ~(b[y]) + c;
	adc	#0		;    c = (sum & 0x100) >> 8;
	php			;    // vvv push
	sta	(ZPMULB),y	;    bwork[y] = sum & 0xff;
	;lda	#0		;
	;sta	(ZPMULT),y	;    b[y] = 0; // will hold final sum
	iny			;
	tya			;
	cmp @w	V1LOCAL	;//bsize;
	bcc	-		;    // ^^^ pull
	pla			;   }

	lda	ZPMULT		;
	sec			;
	sbc @w	V0LOCAL	;//asize;  }
	sta	ZPMULT		; }
	
usmult:	ldy @w	V2LOCAL	;//sizes;
-	lda	#0		;
	dey			;
	sta	(ZPMULT),y	; a = b = 0; // will hold final sum
	tya			;
	bne	-		; 

usmult2:ldy @w	V1LOCAL	;//bsize; while (1) {
	lda	#0		;
	sta @w	V5LOCAL	;//bnon0;
	clc			;
-	dey			;  uint?_t* b = (uint?_t*) bwork;
	lda	(ZPMULB),y	;
	ror			;  uint1_t c = (*b) & 1;
	sta	(ZPMULB),y	;
	ora @w	V5LOCAL	;//bnon0;
	sta @w	V5LOCAL	;//bnon0;
	tya			;
	bne	-		;  *b >>= 1;

	bcs	+		;  if (c == 0) {
	lda @w	V5LOCAL	;//bnon0;   if (*b == 0)
	beq	muldone		;    break;
	bcc	lshifta		;  } else {
+	ldy	#0		;
	clc			;
	php			;  // vvv push
-	plp			;  // ^^^ pull
	lda	(ZPMULA),y	;
	adc	(ZPMULT),y	;
	sta	(ZPMULT),y	;  *((uint?_t*) &a) += *((uint?_t*) awork);
	php			;  // vvv push
	iny			;
	tya			;
	cmp @w	V2LOCAL	;//sizes;
	bcc	-		;
	pla			;  // ^^^ pull

lshifta:ldy	#0		;
	clc			;
	php			;  // vvv push
-	plp			;  // ^^^ pull
	lda	(ZPMULA),y	;
	rol			;
	php			;  // vvv push
	sta	(ZPMULA),y	;
	iny			;
	tya			;
	cmp @w	V2LOCAL	;//sizes;
	bcc	-		;  awork <<= 1;
	pla			;  // ^^^ pull
	inc @w	V6LOCAL	;//rtval;  rtval++; // number of bits in result
; ldy @w V2LOCAL
;- dey
; tya
; pha
; lda (ZPMULT),y	
; tay
; FUNCALL
; jsr putbin
; FUNRETN
; pla
; tay
; bne -
; lda #$d
; jsr $ffd2
; sec
	bcs	usmult2		; }

muldone:lda @w	V4LOCAL	;//sgnba;
	beq	posprod		;
	eor	#$c0		;
	beq	posprod		; if (((sgnba&0x80) >> 7) != ((sgnba&0x40) >> 6))

	sec			;
	ldy	#0		;
	php			;  // vvv push
-	plp			;  // ^^^ pull
	lda	(ZPMULT),y	;
	eor	#$ff		;
	adc	#0		;
	php			;  // vvv push
	sta	(ZPMULT),y	;
	iny			;
	tya			;
	cmp @w	V2LOCAL	;//sizes;
	bcc	-		;  // ^^^ pull
	pla			;  a = -a;

posprod:ldy @w	V6LOCAL	;//rtval;

; ldy @w V2LOCAL
;- dey
; tya
; pha
; lda (ZPMULT),y	
; tay
; FUNCALL
; jsr puthex
; FUNRETN
; pla
; tay
; bne -
; lda #$d
; jsr $ffd2

merror:	POPVARS			; return y = rtval;
	rts			;}

