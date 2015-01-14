all:    lex.cc  main.cc 
	g++   --std=c++0x lex.cc main.cc

lex.cc: lex.l 
	./cond_remove_scannerh
	flexc++ lex.l; 
	sed -i '/int lex()/r enums' Scanner.h



