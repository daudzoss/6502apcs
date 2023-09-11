USIGN8	= 0x00			; 1 byte - 1 = 0
SIGN8	= 0x80 | USIGN8		; bit 7 set

USIGN16	= 0x01			; 2 bytes - 1 = 1
SIGN16	= 0x80 | USIGN16	; bit 7 set

USIGN32	= 0x03			; 4 bytes - 1 = 3
SIGN32	= 0x80 | USIGN32	; bit 7 set
	
mult	.macro
	tya			;inline register int8_t mult(int8_t x,int8_t y){
	ora	#(\2|\1)&0x80	; register int8_t y;
	tay			; y = (y | x) & 0x80; // signed if either signed
	lda	#(\2<<4)|(\1&15); uint8_t siz;
	pha			; siz =  ((y & 0x0f) << 4) | (x & 0x0f);  // var
	FUNCALL			;
	jsr	domult		; return y = domult(siz, a, b); // push y then x
	FUNRETN			;} // mult()
	.endm

domult	
