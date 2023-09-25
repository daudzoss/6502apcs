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

main:	nop			;
test0:	lda	#10		;
	pha			;
	pha			;
	mult	USIGN8,USIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	test1		;
	brk

test1:	lda	#10		;
	pha			;
	pha			;
	mult	SIGN8,USIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	test2		;
	brk

test2:	lda	#10		;
	pha			;
	pha			;
	mult	USIGN8,SIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	test3		;
	brk			;

test3:	lda	#10		;
	pha			;
	pha			;
	mult	SIGN8,SIGN8	;
	pla			;
	tay			;
	pla			;
	cpy	#100		;
	beq	test4		;
	brk			;

test4:	nop			;
exit:	rts			;
