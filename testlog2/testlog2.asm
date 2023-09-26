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
	
	lda	#128		;
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

+	lda	#128		;
	pha			;
	ldy	#8		;
	FUNCALL			;
	jsr	log2		;
	FUNRETN			;
	pla			;
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
	jsr	puthex		;
	tya			;
	bmi	+		;
	brk			;

+	POPVARS			;
	rts			;
