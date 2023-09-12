;;; 	                    7    6    5    4       3    2    1    0      
;;;                      +-------------------------------------------+
;;;              ...     | variables in context of caller of dmult() |
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
;;;                      +-------------------------------------------+
;;;              $0102+X |      bsize - 1      |    asize - 1        | A0FUNCT
;;;                      +-------------------------------------------+
;;;              $0101+X | old value of X e.g. SP'' from a prior time|
;;;                      +-------------------------------------------+
;;; $0100|X    = $0100+X | high byte of return address from dmult()  |
;;;                      +-------------------------------------------+
;;;              $00ff+X | low byte of return address from dmult()   |
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
;;;              $00f9+X |                   acopy?                  | V5LOCAL
;;;                      +-------------------------------------------+
;;;              $00f8+X |                   bcopy?                  | V6LOCAL




ZPMULT = $TBD
ZPMULB = $TBD
ZPMULA = $TBD
	
USIGNED	= 0x00
SIGNED	= SIGNED

USIGN8	= 0x00			; 1 byte - 1 = 0
SIGN8	= SIGNED | USIGN8		; bit 7 set

USIGN16	= 0x01			; 2 bytes - 1 = 1
SIGN16	= SIGNED | USIGN16	; bit 7 set

USIGN32	= 0x03			; 4 bytes - 1 = 3
SIGN32	= SIGNED | USIGN32	; bit 7 set
	
mult	.macro
	tya			;inline register int8_t mult(int8_t x,int8_t y){
	ora	#(\2|\1)&SIGNED	; register int8_t y;
	tay			; y = (y | x) & 0x80; // signed if either signed
	lda	#(\2<<4)|(\1&15); uint8_t sizes;
	pha			; sizes = ((y & 0x0f)<<4) | (x & 0x0f);//varargs
	FUNCALL			; // stack bytes used:  1  x+1 y+1
	jsr	dmult		; return y = dmult(y, sizes, a, b);// push b 1st
	FUNRETN			;} // mult()
	.endm

dmult	lda 	A0FUNCT	;//sizes;register int8_t dmult(register int8_t y,uint8_t
	and	#0x0f		;                 sizes, uint?_t a, uint?_t b) {
	clc			;
	adc	#1		;
	pha			; uint8_t asize = (size& 0x0f)+1; // V0LOCAL
	lda 	A0FUNCT	;//sizes;
	lsr			;
	lsr			;
	lsr			;
	lsr			;
	clc			;
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
	
	;; starting address of full-width copy of a and copy of b will be cached
	;pha			; uint8_t* acopy; // V5LOCAL
	;pha			; uint8_t* bcopy; // V6LOCAL

	;; check for stack underrun: end of args must still be on page 1
	txa			;
	;clc			;
	adc @w	V2LOCAL	;//sizes;
	bcs	merror		;
	cmp	#$fc		; if (x + sizes + 4 > 0x00ff)
	bcs	merror		;  return y;

	;; check for stack overrun: need TBD bytes of stack left for VnLOCAL's
	txa			;
	sec			;
	sbc	V2LOCAL	;//sizes;
	cmp	#TBD		; if (x - sizes < TBD)
	bcc	merror		;  return y;

	;; find msb of a, store sign bit, allocate asize+bsize of stack space
	ADRHA	ZPMULT		;
	ADRHL	ZPMULT,1	;
	ldy @w	V0LOCAL	;//asize;
	dey			;
	lda	(ZPMULT),y	;
	rol			;
	ror	sgnba		; sgnba |= (a & (1<<(asize*8 - 1))) ? 0x40 : 0;
	iny			;
-	pha			;
	dey			;
	bne	-		; uint8_t awork[asize + bsize];
	
	txa			;
	pha			;
	tsx			;
	txa			;
	;tay			;
	sta	ZPMULA	  	;
	inc	ZPMULA		;
	lda	#1		;
	sta	1+ZPMULA	;
	pla			;
	tax			;
	;sty @w	V5LOCAL	;//acopy; acopy = &awork[0];
	
	;; find MSB of b, store sign bit, allocate bsize of stack space for it
	ldy @w	V1LOCAL	;//bsize;
	clc			;
	adc @w	V0LOCAL	;//asize;
	tay			;
	dey			;
	lda	(ZPMULT),y	;
	rol			;
	ror	sgnba		; sgnba |= (a & (1<<(asize*8 - 1))) ? 0x80 : 0;
	iny			;
-	pha			;
	dey			;
	bne	-		; uint8_t bwork[bsize];
	
	txa			;
	pha			;
	tsx			;
	txa			;
	;tay			;
	sta	ZPMULB		;
	inc	ZPMULB		;
	lda	#1		;
	sta	1+ZPMULB	;
	pla			;
	tax			;
	;sty @w	V6LOCAL	;//bcopy; bcopy = &bwork[0];
	
	;; check for signed or unsigned mode
	lda @w	V3LOCAL	;//savey;
	and	#SIGNED		; if ((savey & SIGNED)  // signed mode requested
	bpl	usmult		;     &&                        // and
smult	lda @w	sgnba		;     ((a&(1<<asize*8 - 1)) ||  // a negative or
	beq	usmult		;      (b&(1<<bsize*8 - 1)))) { // b negative
	bvc	smult2		;  if (a < 0) { //a = -a, sign-extended to sizes
	
	sec			;   register uint8_t c = 1, y;
	ldy	#0		;   for (y = 0; y < asize; y++) {
-	lda	(ZPMULT),y	;
	eor	#$ff		;    register uint9_t sum = ~(a[y]) + c;
	adc	#0		;    c = sum >> 8;
	sta	(ZPMULA),y	;    awork[y] = sum & 0xff;
	;lda	#0		;
	;sta	(ZPMULT),y	;    a[y] = 0; // will hold final sum
	iny			;
	tya			;
	cmp @w	V0LOCAL	;//asize;
	bcc	-		;   }
	
-	lda	#$00		;   for (; y < sizes; y++)
	sta	(ZPMULA),y	;    awork[y] = 0x00;
	iny			;
	tya			;
	cmp @w	V2LOCAL	;//sizes;
	
	lda @w	sgnba		;  }
smult2	bpl	usmult		;  if (b < 0) { //b = -b, not sign-extended

	lda	ZPMULT		;
	clc			;
	adc @w	V0LOCAL	;//asize;
	sta	ZPMULT		;

	sec			;   register uint8_t c = 1, y;
	ldy	#0		;   for (y = 0; y < bsize; y++) {
-	lda	(ZPMULT),y	;
	eor	#$ff		;    register uint9_t sum = ~(b[y]) + c;
	adc	#0		;    c = (sum & 0x100) >> 8;
	sta	(ZPMULB),y	;    bwork[y] = sum & 0xff;
	;lda	#0		;
	;sta	(ZPMULT),y	;    b[y] = 0; // will hold final sum
	iny			;
	tya			;
	cmp @w	V1LOCAL	;//bsize;
	bcc	-		;   }

	lda	ZPMULT		;
	sec			;
	sbc @w	V0LOCAL	;//asize;  }
	sta	ZPMULT		; }

usmult	ldy	#0		;
-	lda	#0		;
	sta	(ZPMULT),y	;
	iny			;
	tya			;
	cmp @w	V2LOCAL	;//sizes; a = b = 0;
	bne	-		;  // will hold final sum
	
usmult2	ldy @w	V1LOCAL	;//bsize; while (1) {
	lda	#0		;
	pha			;  uint8_t bnon0; // vvv push
	clc			;
-	dey			;  uint?_t* b = (uint?_t*) bwork;
	lda	(ZPMULB),y	;
	ror			;  uint1_t c = (*b) & 1;
	sta	(ZPMULB),y	;
	ora @w	V5LOCAL	;//bnon0;
	sta @w	V5LOCAL	;//bnon0;
	tya			;
	bne	-		;  *b >>= 1;
	pla			;  // ^^^ pull
	
	bcs	+		;  if (c == 0) {
	beq	muldone		;   if (*b == 0) break;
	bcc	lshifta		;  } else {
+	ldy	#0		;
	clc			;
	php			; // vvv push
-	plp			; // ^^^ pull
	lda	(ZPMULA),y	;
	adc	(ZPMULT),y	;
	sta	(ZPMULT),y	;  *((uint?_t*) &a) += *((uint?_t*) awork);
	iny			;
	php			; // vvv push
	cmp @w	V2LOCAL	;//sizes;
	bcs	-		;
	pla			; // ^^^ pull
	
lshifta	ldy @w	V2LOCAL	;//sizes;
	clc			;
-	dey			;
	lda	(ZPMULA),y	;
	rol			;
	sta	(ZPMULA),y	;
	tya			;
	bne	-		;  a <<= 1;
	beq	usmult2		; }

postmul
	;; TO-DO: allow y to specify starting value (if known not all bits used)
