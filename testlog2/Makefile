testlog2.prg : testlog2.asm header.inc 6502apcs.inc log2.inc
	64tass -a testlog2.asm -L testlog2.lst -o testlog2.prg

clean :
	rm testlog2.lst testlog2.prg

pdf :
	enscript -2r log2.inc testlog2.asm -o testlog2.ps && ps2pdf testlog2.ps
