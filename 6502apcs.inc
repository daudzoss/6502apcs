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
#define A0FUNCT $0102,x
#define A1FUNCT $0103,x
#define A2FUNCT $0104,x
#define A3FUNCT $0105,x
#define A4FUNCT $0106,x
#define A5FUNCT $0107,x
#define A6FUNCT $0108,x
#define A7FUNCT $0109,x
#define A8FUNCT $010a,x
#define A9FUNCT $010b,x
#define AaFUNCT $010c,x
#define AbFUNCT $010d,x
#define AcFUNCT $010e,x
#define AdFUNCT $010f,x
	
;;;// Only use traditional jsr and rts alone when no args passed nor local vars
;;;// 
;;;// These clobber both a and x, so recommend y as a quick stackless arg/retval
;;;// or otherwise A0FUNCT is a good pointer to a multi-byte result.
;;;// Also do FUNCALL before the body of main() so that it can use stack vars

#define FUNCALL txa:pha:tsx	;// then jump to subroutine with x,sp saved
	;;// jsr func
#define FUNRETN pla:tax		;// done immediately after rts from that jsr

;;;// Useful both before a FUNCALL (to push AnFUNCT) or after (for VnLOCAL)
#define UINT8_T pha
#define INT8_T pha
#define UINT16_T pha:pha
#define INT16_T pha:pha
#define POINTER pha:pha
#define ADRHV lda #1
#define ADRHA lda #1
#define ADRLV(n) txa : clc : adc #$fe-n
#define ADRLA(n) txa : clc : adc #$02+n
#define DERV(n,z) /*lda #1:sta z+1:*/ADRLV(n):sta z:lda (z),y
#define DERA(n,z) /*lda #1:sta z+1:*/ADRLA(n):sta z:lda (z),y
#define ARRAY8(n) txa:tay:tsx:txa:sec:sbc #n:tax:txs:tya:tax
#define ARRAY16(n) txa:tay:tsx:txa:sec:sbc #n:sbc #n:tax:txs:tya:tax

;;;// Lowercase versions of the above
#define	uint8_t UINT8_T
#define int8_t INT8_T
#define uint16_t UINT16_T
#define int16_t INT16_T
#define pointer POINTER
#define adrhv ADRHV
#define adrha ADRHA
#define adrlv(n) ADRLV(n)
#define derefv(n,z) DEREFV(n,z)
#define derefa(n,z) DEREFA(n,z)
#define array8(n) ARRAY8(n)
#define array16(n) ARRAY16(n)

;;;// local stack variables proceed downward in memory: V1LOCAL can be pointer etc
#define V0LOCAL	!$00fe,x
#define V1LOCAL !$00fd,x
#define V2LOCAL !$00fc,x	
#define V3LOCAL !$00fb,x	
#define V4LOCAL !$00fa,x	
#define V5LOCAL !$00f9,x	
#define V6LOCAL !$00f8,x	
#define V7LOCAL !$00f7,x	
#define V8LOCAL !$00f6,x	
#define V9LOCAL !$00f5,x	
#define VaLOCAL !$00f4,x
#define VbLOCAL !$00f3,x
#define VcLOCAL !$00f2,x
#define VdLOCAL !$00f1,x
#define VeLOCAL !$00f0,x
	
;;;//undoes stack variable allocations and arguments on stack before rts
;;;// (sp=x=0x00fe+x) or just whenever cleanup is necessary
#define POPVARS dex:dex:txs
#define DONTRTS inx:inx		;// if just doing it for stack cleanup not rts
