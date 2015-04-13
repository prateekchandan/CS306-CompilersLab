all: assembler

assembler: machine.cc code.asm
	g++ -std=c++11 -g3 -m32 -O0 machine.cc

.PHONY: clean

clean:
	rm assembler
