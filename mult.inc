;;;//                      +-------------------------------------------
;;;//              ...     | last of stack usage so far with current SP
;;;//                      +-------------------------------------------
;;;//                      +-------------------------------------------
;;;//              ...     | last of stack usage so far with current SP
;;;//                      +-------------------------------------------
;;;// $0100|SP'' = $0102+x |                                           |
;;;//                      +-------------------------------------------+
;;;//              $0102+X |                     |                     | A0FUNCT
;;;//                      +-------------------------------------------+
;;;//              $0101+X | old value of X e.g. SP'' from a prior time
;;;//                      +-------------------------------------------
;;;// $0100|X    = $0100+X | high byte of return address from dmult()
;;;//                      +-------------------------------------------
;;;//              $00ff+X | low byte of return address from dmult()
;;;//                      +-------------------------------------------
;;;// $0100|SP'  = $00fe+X | dmult()'s variables and arguments for calls
;;;//                      +-------------------------------------------
;;;//              $00fd+X |
;;;//                      +-------------------------------------------
;;;//              ...     | last of stack usage so far with current SP
;;;//                      +-------------------------------------------
;;;// $0100|SP             |





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
	adc @w	V0LOCAL	;//asiz	; //a <= 32 so c==0
	pha			; uint8_t sizes = asize + bsize; // V2LOCAL

	;; save y and prepare to store a and b signs in V and N bit respectively
	tya			;
	pha			; int8_t savey = y; // V3LOCAL
	ldy	#0		; y = 0; // return value is zero until checks OK
	tya			;
	pha			; uint8_t sgnba = 0; // V4LOCAL
	
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
	
	;; find MSB of b, store sign bit, allocate bsize of stack space for it
	ldy @w	V1LOCAL	;//bsize;
	clc			;
	adc @w	V0LOCAL	;//asize;
	tay			;
	dey			;
	lda	(ZPMULT),y	;
	rol			;
	ror	sgnba		; sgnba |= (a & (1<<(asize*8 - 1))) ? 0x40 : 0;
	iny			;
-	pha			;
	dey			;
	bne	-		; uint8_t bwork[bsize];
	
	
	;; check for signed or unsigned mode
	lda @w	V3LOCAL	;//savey;
	and	#SIGNED		; if ((savey & SIGNED)  // signed mode requested
	bpl	usmult		;     &&                        // and
smult	lda @w	sgnba		;     ((a&(1<<asize*8 - 1)) ||  // a negative or
	beq	usmult		;      (b&(1<<bsize*8 - 1)))) { // b negative
	bvc	smult2		;  if (a < 0) {
	lda @w	sgnba		;
smult2	bpl	usmult		;  } else if (b < 0) {

usmult
	

	;; TO-DO: 
