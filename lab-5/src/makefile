all: clean compiler 

compiler: lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	@g++  --std=c++0x lex.cc parse.cc main.cc -o compiler;
	@echo "Compiler got compiled :)";
	
exe: clean3 machine.cc
	@./compiler < ../test.c > code.asm;
	@g++ -std=c++11 -g3 -m32 -Wno-write-strings -O0 machine.cc;
	@echo "Compiler compiled the code :D"
	
lex.cc: lex.l Scanner.ih 
	@rm -rf Scanner*
	@flexc++ lex.l;

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	@bisonc++  parse.y > /dev/null;
	@./sedscript
     
Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean3:
	@rm -rf code.asm a.out
	
clean2:
	@rm -rf Parser* Scanner* code.asm junk* lex.cc parse.cc graph1* a.out
	
clean:
	@rm -rf Parser* Scanner* compiler code.asm junk* lex.cc parse.cc graph1* a.out
