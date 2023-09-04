;;;// Fundamental assumption is that the X register always contains the SP
;;;// value that must be restored after a function return (RTS) to
;;;// reclaim stack space used for local stack variables and function arguments
;;;// 
;;;// The value above it (at $101,X) contains the X value that must be restored
;;;// immediately afterward in order so that the same holds true again for the
;;;// calling function to do the same when it returns
;;;//
;;;// $0100|SP'' = $0102+x | arg #1 to func(a0funct, a1funct,..)
;;;//                      +-------------------------------------------
;;;//              $0102+X | arg #0 to func(a0funct, a1funct,..) 
;;;//                      +-------------------------------------------
;;;//              $0101+X | old value of X e.g. SP'' from a prior time
;;;//                      +-------------------------------------------
;;;// $0100|X    = $0100+X | high byte of return address from func()
;;;//                      +-------------------------------------------
;;;//              $00ff+X | low byte of return address from func()
;;;//                      +-------------------------------------------
;;;// $0100|SP'  = $00fe+X | func()'s variables and arguments for calls
;;;//                      +-------------------------------------------
;;;//              $00fd+X |
;;;//                      +-------------------------------------------
;;;//              ...     | last of stack usage so far with current SP
;;;//                      +-------------------------------------------
;;;// $0100|SP             |
;;;// 
;;;// Arguments to functions (and pointers for possible return values) proceed
;;;// upward in memory: A0FUNCT, A2FUNCT can be such pointers etc
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
	
;;;// Only use traditional jsr and rts alone when no args passed nor local vars
;;;// 
;;;// These clobber both a and x, so recommend y as a quick stackless arg/retval
;;;// or otherwise A0FUNCT is a good pointer to a multi-byte result.
;;;// Also do FUNCALL before the body of main() so that it can use stack vars

FUNCALL	= .byte $8a,$48,$ba	;// then jump to subroutine with x,sp saved
	;;// $8a=txa
	;;// $48=pha
	;;// $ba=tsx
	;;// jsr func	
FUNRETN	= .byte $68,$aa		;// done immediately after rts from that jsr
	;;// $68=pla
	;;// $aa=tax
	
;;;// Useful both before a FUNCALL (to push AnFUNCT) or after (for VnLOCAL)
UINT8_T	= .byte $48		;// $48=pha
INT8_T	= .byte $48		;// $48=pha
UINT16_T = .byte $48,$48	;// $48=pha
INT16_T	= .byte $48,$48		;// $48=pha
POINTER	= .byte $48,$48		;// $48=pha
ADRHV	= .byte $a9,$01		;// $a9=lda#
ADRHA	= .byte $a9,$01		;// $a9=lda#
ADRLV	= .byte $8a,$18,$69,$fe-;// $8a=txa : $18=clc : $69=adc#
ADRLA	= .byte $8a,$18,$69,$02+;// $8a=txa : $18=clc : $69=adc#
DEREF	= .byte $8a,$18,$69,$fe-;// $8a=txa : $18=clc : $69=adc# : $=sta :$b1=lda (z),y

;;;// Lowercase versions of the above
uint8_t	= UINT8_T
int8_t	= INT8_T
uint16_t = UINT16_T
int16_t	= INT16_T
pointer	= POINTER
adrhv	= ADRHV
adrha	= ADRHA
adrlv	= ADRLV
adrla	= ADRLA
deref	= DEREF

;;;// local stack variables proceed downward in memory: V1LOCAL can be pointer etc
V0LOCAL	= @w $00fe,x
V1LOCAL	= @w $00fd,x
V2LOCAL	= @w $00fc,x	
V3LOCAL	= @w $00fb,x	
V4LOCAL	= @w $00fa,x	
V5LOCAL	= @w $00f9,x	
V6LOCAL	= @w $00f8,x	
V7LOCAL	= @w $00f7,x	
V8LOCAL	= @w $00f6,x	
V9LOCAL	= @w $00f5,x	
VaLOCAL	= @w $00f4,x
VbLOCAL	= @w $00f3,x
VcLOCAL	= @w $00f2,x
VdLOCAL	= @w $00f1,x
VeLOCAL	= @w $00f0,x
VfLOCAL	= @w $00ef,x
	
;;;//undoes stack variable allocations and arguments on stack before rts
;;;// (sp=x=0x00fe+x) or just whenever cleanup is necessary
POPVARS	= .byte $ca,$ca,$9a		;// $ca=dex : $9a=txs
DONTRTS	= .byte	$e8,$e8	;// $e8=inx (if just doing it for stack cleanup not rts
