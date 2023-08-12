;;;// Fundamental assumption is that both the X register and $100,X contain
;;;// the SP value that must be restored after a function return (RTS) to
;;;// reclaim stack space used for local stack variables and function arguments
;;;// 
;;;// The value above it (at $101,X) contains the X value that must be restored
;;;// immediately afterward in order so that the same holds true again for the
;;;// calling function to do the same when it returns
;;;//
;;;// $0100|SP'' = $0102+x | first of args to func(a0funct, a1funct,..)
;;;//                      +-------------------------------------------
;;;//              $0101+X | old value of X i.e. SP''' from prior call
;;;//                      +-------------------------------------------
;;;//              $0100+X | SP'' (which is also stored in X)
;;;//                      +-------------------------------------------
;;;//              $00ff+X | low byte of return address from func()
;;;//                      +-------------------------------------------
;;;//              $00fe+X | high byte of return address from func()
;;;//                      +-------------------------------------------
;;;// $0100|SP'  = $00fd+X | func()'s variables and arguments for calls
;;;//                      +-------------------------------------------
;;;//              $00fc+X |
;;;//                      +-------------------------------------------
;;;//              ...     |
;;;//                      +-------------------------------------------
;;;// $0100|SP             |
;;;// 
;;;// Arguments to functions (and pointers for possible return values) proceed
;;;// upward in memory: A0FUNCT, A2FUNCT can be such pointers etc
#define A0FUNCT $102,x
#define A1FUNCT $103,x
#define A2FUNCT $104,x
#define A3FUNCT $105,x
#define A4FUNCT $106,x
#define A5FUNCT $107,x
#define A6FUNCT $108,x
#define A7FUNCT $109,x
#define A8FUNCT $10a,x
#define A9FUNCT $10b,x

;;;// Only use traditional jsr and rts alone when no args passed nor local vars
;;;// 
;;;// These clobber both a and x, so recommend y as a quick stackless arg/retval
;;;// or otherwise A0FUNCT is a good pointer to a multi-byte result.
;;;// Also do FUNCALL before the body of main() so that it can use stack vars
	
#define FUNCALL txa:pha:tsx:txa:pha ;// then jump to subroutine with x,sp saved
	;;// jsr func
#define FUNRETN pla:tax:pla:txs:tax ;// done immediately after rts from that jsr

;;;// These are useful both before a FUNCALL (to push args) or after (for vars)
#define UINT8_T pha
#define INT8_T pha
#define UINT16_T pha:pha
#define INT16_T pha:pha
#define POINTER pha:pha
	
;;;// Lowercase versions of the above
#define	uint8_t UINT8_T
#define int8_t INT8_T
#define uint16_t UINT16_T
#define int16_t INT16_T
#define pointer POINTER
	
;;;// local stack variables proceed downward in memory: V1LOCAL can be pointer etc
#define V0LOCAL	!$00fd,x
#define V1LOCAL !$00fc,x
#define V2LOCAL !$00fb,x	
#define V3LOCAL !$00fa,x	
#define V4LOCAL !$00f9,x	
#define V5LOCAL !$00f8,x	
#define V6LOCAL !$00f7,x	
#define V7LOCAL !$00f6,x	
#define V8LOCAL !$00f5,x	
#define V9LOCAL !$00f4,x	

#define POPVARS lda $100,x:tax:dex:dex:txs ;// undoes stack variables before rts
