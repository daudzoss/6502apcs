	.include "6502apcs.inc"

	.include "header.inc"
*	= BASIC+1
	.word	(+), 2055
	.null	$9e, format("%4d", start)
+	.word	0
start	FUNCALL			;// give frame pointer x a valid initial value
	jsr	main		;
	FUNRETN			;
	rts			;

	.include "log2.inc"

putbin:	tya			;void putbin(register uint8_t y) {
	pha			; uint8_t savey = y;
	ldy	#8		;
-	rol @w	V0LOCAL		; for (y = 8; y; y--) {
	lda	#"0"		;  uint8_t c = savey & 0x80;
	adc	#0		;  savey <<= 1;
	jsr	$ffd2		;  putchar(c ? '1' : '0');
	dey			;
	bne	-		; }
	POPVARS			;
	rts			;} // putbin()

putnyb:	and	#$0f		;void putnyb(register uint8_t a, register uint8_t c) {
	bne	+		; a &= 0x0f;
	bcc	+		; if ((a == 0) && c) { // omit leading 0
	lda	#" "		;   putchar(' ');
	bcs	putnyb2		; } else {
+	ora	#"0"		;  a |= '0';
	cmp	#1+"9"		;
	bcc	putnyb2		;  
	clc			;
	adc	#"a"-"9"-1	;  putchar ((a <= '9') ? a : a + '9' - 1);
putnyb2	jsr	$ffd2		; }
	rts			;} // putnyb()

puthex:	tya			;void puthex(register uint8_t y, register uint8_t c) {
	pha			;
	tya			;
	php			;
	lsr			;
	lsr			;
	lsr			;
	lsr			;
	plp			;
	jsr	putnyb		; putnyb(a >> 4, c);
	pla			;
	clc			; putnyb(a, 0);
	jmp	putnyb		;} // puthex()

main:	nop	
	
	;; 32-bit inverse test
+	ldy	#30		;
	lda	#0		;
	pha			;
	pha			;
	pha			;
	pha			;
	FUNCALL			;
	jsr	two2the		;
	FUNRETN			;
	ldy	#32		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	cpy	#30		;
	beq	+		;
	brk			;
	
+	lda	#128		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	cpy	#15		;
	beq	+		;
	brk			;

+	lda	#64		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	cpy	#14		;
	beq	+		;
	brk			;

+	lda	#32		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	cpy	#13		;
	beq	+		;
	brk			;

+	lda	#16		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	cpy	#12		;
	beq	+		;
	brk			;

+	lda	#8		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	cpy	#11		;
	beq	+		;
	brk			;

+	lda	#4		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	cpy	#10		;
	beq	+		;
	brk			;

+	lda	#2		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#9		;
	beq	+		;
	brk			;

+	lda	#1		;
	pha			;
	pha			;
	ldy	#16		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#8		;
	beq	+		;
	brk			;

+	lda	#128		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#7		;
	beq	+		;
	brk			;

+	lda	#64		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#6		;
	beq	+		;
	brk			;

+	lda	#32		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#5		;
	beq	+		;
	brk			;

+	lda	#16		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#4		;
	beq	+		;
	brk			;

+	lda	#8		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#3		;
	beq	+		;
	brk			;

+	lda	#4		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#2		;
	beq	+		;
	brk			;

+	lda	#2		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#1		;
	beq	+		;
	brk			;

+	lda	#1		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	sec			;
	jsr	puthex		;
	cpy	#0		;
	beq	+		;
	brk			;

+	lda	#0		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
	tya			;
	bmi	+		;
	brk			;

+	POPVARS			;
	rts			;
