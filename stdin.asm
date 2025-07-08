getchar	txa			;inline uint8_t getchar(void) {
	pha			; // x stashed on stack, by way of a
-	jsr	$ffe4		; do {
	beq	-		;  y = (* ((*)(void)) 0xffe4)();
	tay			; } while (!y);
	pla			; return y;
	tax			; // x restored from stack, by way of a
	rts			;} // getchar()

