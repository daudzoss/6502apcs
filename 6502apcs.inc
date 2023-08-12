//#define FUNARG(n) $102+n,x	; upward in memory: FUNARG(0) can be a pointer

;;; only use traditional jsr and rts alone when no args passed nor local vars
;;; these clobber both a and x, so recommend y as a quick stackless arg/retval
;;; otherwise FUNARG(0) is a good pointer to a multi-byte result
;;; also do FUNCALL at the beginning of main() so that it can use stack vars
#define FUNCALL txa:pha:tsx:txa:pha ; then jump to subroutine with x,sp saved
	;; jsr function
//#define FUNRETN pla:tax:pla:txs:tax ; immediately after rts from that jsr

;;; these are useful both before a FUNCALL (to push args) or after (for vars)
#define UINT8_T pha
//#define INT8_T pha
//#define UINT16_T pha:pha
//#define INT16_T pha:pha
//#define POINTER pha:pha
	
//#define LOCVAR(n) $fd-n,x	; downward in memory: FUNARG(1) can be a pointer
#define POPVARS lda $100,x:tax:dex:dex:txs ; undoes stack variables before rts
