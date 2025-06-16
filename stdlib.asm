stckstr	.macro	frstchr,lastchr	;#define stckstr(frstchr,lastchr) \
;;; any way to check also if user rememberd to make lastchr-1 a '\0'?
.if \lastchr < \frstchr
.error "negative-length string specified for stacking"
.elsif \lastchr-\frstchr <= $50
	lda	 #0		;#stckstr(frstchr,lastchr)                     \
	pha			; uint8_t terminator = 0;                      \
	ldy	#\lastchr-\frstchr;                                            \
-	lda	\frstchr-1,y	;                                              \
	pha			;                                              \
	dey			;                                              \
	bne	-		; uint8_t from_end = (char[]) frstchr;
.else
.error "let's not burn that much of the system stack on printing one string"
.endif
	.endm			;// stckstr // suggest caller do POPVARS:DONTRTN

putstck	pha		;//start;void putstck(register uint8_t a, // usually $00
	txa			;             register uint8_t y, // usually $ff
	clc			;             const uint8_t stacked[]) {
	adc @w	V0LOCAL	;//start;
	sta @w	V0LOCAL	;//start; start = x + a;
	tya			;
	clc			;
	adc @w	V0LOCAL	;//start; stop = start + y;
	bcc	+		;
	lda	#$ff		; if (stop > 0x1ff)
+	pha		;//stop	;  stop = 0x01ff;
-	ldy @w	V0LOCAL	;//start; for (y = start; y <= stop; y++) {
	lda	$0102,y		;  a = stacked[y];
	beq	+		;  if (a)
	jsr	putchar		;   putchar(a);
	inc @w	V0LOCAL	;//start;  else
	lda @w	V0LOCAL	;//start;
	cmp @w	V1LOCAL	;//stop	;   return; // '\0' hit before requested # chars
	bcc	-		; }
+	POPVARS			;
	rts			;} // putstck()

putchar	tay			;inline void putchar(register uint8_t a) {
	txa			; // a stashed in y
	pha			; // x stashed on stack, by way of a
	tya			; // a restored from y
	jsr	$ffd2		; (* ((*)(uint8_t)) 0xffd2)(a);
	pla			;
	tax			; // x restored from stack, by way of a
	rts			;} // putchar()

puthex_	.byte	'0','1','2','3'	;static uint8_t puthex_[] = {'0','1','2','3'
	.byte	'4','5','6','7'	;                           '4','5','6','7'
	.byte	'8','9','a','b'	;                           '8','9','a','b'
	.byte	'c','d','e','f'	;                           'c','d','e','f'};
puthexd	pha	;V0LOCAL;//oldy	;register uint8_t puthexd(register uint8_t a) {
	lsr			; uint8_t oldy = a;
	lsr			;
	lsr			;
	lsr			;
	tay			;
	lda	puthex_,y	;
	jsr	putchar		; putchar(puthex_[a >> 4]);
	lda @w	V0LOCAL	;//oldy	;
	and	#%0000 .. %1111	;
	tay			;
	lda	puthex_,y	;
	jsr	putchar		; putchar(puthex_[a & 0x0f]);
	ldy @w	V0LOCAL	;//oldy	;
	POPVARS			; return y = oldy;
	rts			;} // puthexd()

getchar	txa			;inline uint8_t getchar(void) {
	pha			; // x stashed on stack, by way of a
-	jsr	$ffe4		; do {
	beq	-		;  y = (* ((*)(void)) 0xffe4)();
	tay			; } while (!y);
	pla			; return y;
	tax			; // x restored from stack, by way of a
	rts			;} // getchar()

