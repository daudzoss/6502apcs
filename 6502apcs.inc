;;; Fundamental assumption is that the X register always contains the SP
;;; value that must be restored after a function return (RTS) to
;;; reclaim stack space used for local stack variables and function arguments
;;; 
;;; The value above it (at $101,X) contains the X value that must be restored
;;; immediately afterward in order so that the same holds true again for the
;;; calling function to do the same when it returns
;;;
;;; $0100|SP'' = $0103+x | arg #1 to func(a0funct, a1funct,..)
;;;                      +-------------------------------------------
;;;              $0102+X | arg #0 to func(a0funct, a1funct,..) 
;;;                      +-------------------------------------------
;;;              $0101+X | old value of X e.g. SP'' from a prior time
;;;                      +-------------------------------------------
;;; $0100|X    = $0100+X | high byte of return address from func()
;;;                      +-------------------------------------------
;;;              $00ff+X | low byte of return address from func()
;;;                      +-------------------------------------------
;;; $0100|SP'  = $00fe+X | func()'s variables and arguments for calls
;;;                      +-------------------------------------------
;;;              $00fd+X |
;;;                      +-------------------------------------------
;;;              ...     | last of stack usage so far with current SP
;;;                      +-------------------------------------------
;;; $0100|SP             |
;;; 
;;; Arguments to functions (and pointers for possible return values) proceed
;;; upward in memory: A0FUNCT, A2FUNCT can be such pointers etc
A0FUNCT	= $0102,x
A1FUNCT	= $0103,x
A2FUNCT	= $0104,x
A3FUNCT	= $0105,x
A4FUNCT	= $0106,x
A5FUNCT	= $0107,x
A6FUNCT	= $0108,x
A7FUNCT	= $0109,x
A8FUNCT	= $010a,x
A9FUNCT	= $010b,x
AaFUNCT	= $010c,x
AbFUNCT	= $010d,x
AcFUNCT	= $010e,x
AdFUNCT	= $010f,x
	
;;; Only use traditional jsr and rts alone when no args passed nor local vars
;;; 
;;; These clobber both a and x, so recommend y as a quick stackless arg/retval
;;; or otherwise A0FUNCT is a good pointer to a multi-byte result.
;;; Do FUNCALL or tsx before jumping into main() so that it can use stack vars

FUNCALL	.macro			; // then jump to subroutine with x,sp saved
	txa
	pha
	tsx
	.endm
	;; lda #additional_arg
	;; jsr func	
FUNRETN	.macro			; // done immediately after rts from that jsr
	pla
	tax
	.endm
	
jsrAPCS	.macro	addr,opcode=tya,arg1=,arg2=
	FUNCALL
	\opcode	\arg1 \arg2
	jsr	\addr
	FUNRETN
	.endm

ADRHV	.macro
	lda	#$01
	sta	1+\1
	.endm

ADRHA	.macro
	lda	#$01
	sta	1+\1
	.endm

ADRLV	.macro
	txa
	clc
	adc	#$fe-\2
	sta	\1
	.endm

ADRLA	.macro
	txa
	clc
	adc	#$02+\2
	sta	\1
	.endm

;;; local stack variables proceed downward in memory eg V1LOCAL can be pointer
;;; these must all use 16-bit indexed addressing; they are not on zero-page!!!
V0LOCAL	= $00fe,x
V1LOCAL	= $00fd,x
V2LOCAL	= $00fc,x	
V3LOCAL	= $00fb,x	
V4LOCAL	= $00fa,x	
V5LOCAL	= $00f9,x	
V6LOCAL	= $00f8,x	
V7LOCAL	= $00f7,x	
V8LOCAL	= $00f6,x	
V9LOCAL	= $00f5,x	
VaLOCAL	= $00f4,x
VbLOCAL	= $00f3,x
VcLOCAL	= $00f2,x
VdLOCAL	= $00f1,x
VeLOCAL	= $00f0,x
VfLOCAL	= $00ef,x
	
;;; pha either before a FUNCALL (to push AnFUNCT) or after jsr (for VnLOCAL)

;;;undoes stack variable allocations and arguments on stack before rts
;;;(sp=x=0x00fe+x) or just whenever cleanup is necessary (by then restoring x)
POPVARS	.macro
	dex
	dex
	txs
	.endm
DONTRTS	.macro
	inx
	inx
	.endm
