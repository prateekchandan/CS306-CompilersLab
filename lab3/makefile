all:    clean lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++   --std=c++0x lex.cc parse.cc main.cc;
	./a.out < test-cpp > junk;
	egrep 'nextToken|reduce' junk
        
graph:
	dot -Tps graph1.gv -o graph1.ps
	gnome-open graph1.ps
	
lex.cc: lex.l Scanner.ih 
	@rm -rf Scanner*
	flexc++ lex.l; 
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  parse.y; 
	sed -i '/#include "Parser.h"/r extra-headers.cpp' Parser.ih
#	bisonc++   --construction -V parse.y; 
#	sed -i '/ifndef/a #include "tree_util.hh"' Parserbase.h;
#	sed -i '/ifndef/a #include "tree.hh"' Parserbase.h;
#	./sedscript
     

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean:
	rm -rf Parser* Scanner* a.out junk lex.cc parse.cc junk graph1*
