all:    clean lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++   --std=c++0x lex.cc parse.cc main.cc;
	./a.out < test-cpp > junk.scm;
        
graph:
	dot -Tps graph1.gv -o graph1.ps
	gnome-open graph1.ps
	
lex.cc: lex.l Scanner.ih 
	@rm -rf Scanner*
	flexc++ lex.l; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  parse.y;
	./sedscript
     

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean:
	rm -rf Parser* Scanner* a.out junk* lex.cc parse.cc graph1*
