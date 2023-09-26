testmult.prg : testmult.asm header.inc 6502apcs.inc mult.inc
	64tass -a testmult.asm -L testmult.lst -o testmult.prg

clean :
	rm testmult.lst testmult.prg

pdf :
	enscript -2r mult.inc testmult.asm -o testmult.ps && ps2pdf testmult.ps
