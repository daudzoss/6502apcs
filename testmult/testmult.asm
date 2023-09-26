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

	.include "mult.inc"

putbin:	tya			;
	pha			;
	ldy	#8		;
-	rol @w	V0LOCAL		;
	lda	#"0"		;
	adc	#0		;
	jsr	$ffd2		;
	dey			;
	bne	-		;
	POPVARS			;
	rts			;

putnyb:	and	#$0f		;
	ora	#"0"		;
	cmp	#1+"9"		;
	bcc	+		;
	clc			;
	adc	#"a"-"9"-1	;
+	jsr	$ffd2		;
	rts			;

puthex:	tya			;
	pha			;
	tya			;
	lsr			;
	lsr			;
	lsr			;
	lsr			;
	jsr	putnyb		;
	pla			;
	jmp	putnyb		;

main:	nop	
	
	;; 10 * 10
	lda	#10		;
	pha			;
	pha			;
	mult	USIGN8,USIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	+		;
	brk

	;; +10 * 10
+	lda	#10		;
	pha			;
	pha			;
	mult	SIGN8,USIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	+		;
	brk

	;; 10 * +10
+	lda	#10		;
	pha			;
	pha			;
	mult	USIGN8,SIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	+		;
	brk			;

	;; +10 * +10
+	lda	#10		;
	pha			;
	pha			;
	mult	SIGN8,SIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	+
	brk			;

	;; same quantity as above but 32-bit unsigned, squared
+	lda	#0		;
	pha
	pha			;
	pha			;
	lda	#10		;
	pha			;
	lda	#0		;
	pha
	pha			;
	pha			;
	lda	#10		;
	pha			;
	mult	USIGN32,USIGN32	;
	pla			;
	tay			;
	pla			;
	beq	+		;
	brk			;
+	pla			;
	beq	+		;
	brk			;
+	pla			;
	beq	+		;
	brk			;
+	pla			;
	beq	+		;
	brk			;
+	pla			;
	beq	+		;
	brk			;
+	pla			;
	beq	+		;
	brk			;
+	pla			;
	beq	+		;
	brk			;
+	cpy	#100		;
	beq	+		;
	brk			;

	;; 16-bit unsigned, squared (pi*10000)
+	lda	#31416>>8	;
	pha			;
	lda	#31416&$ff	;
	pha			;
	lda	#31416>>8	;
	pha			;
	lda	#31416&$ff	;
	pha			;
	mult	USIGN16,USIGN16	;
	pla			;
	cmp	#(986965056&$000000ff)>>0
	beq	+		;
	brk			;
	cmp	#(986965056&$0000ff00)>>8
	beq	+		;
	brk			;
	cmp	#(986965056&$00ff0000)>>16
	beq	+		;
	brk			;
	cmp	#(986965056&$ff000000)>>24
	beq	+		;
	brk			;

	;; -64 * 2 in 16 bits x 8 bits
+	lda	#-1		;
	pha			;
	lda	#-64		;
	pha			;
	lda	#2		;
	pha			;
	mult	SIGN16,SIGN8	;
	pla			;
	cmp	#$80		;
	beq	+		;
	brk			;
	pla			;
	cmp	#$ff		;
	beq	+		;
	brk			;
	pla			;
	cmp	#$ff		;
	beq	+		;
	brk			;
	
	;; -64 * 2 should be -128 and this works in 16 bits
+	lda	#-64		;
	pha			;
	lda	#2		;
	pha			;
	mult	SIGN8,SIGN8	;
	pla			;
	cmp	#$80		;
	beq	+		;
	brk			;
	pla			;
	beq	+		;
	brk
+	POPVARS			;
	rts			;
