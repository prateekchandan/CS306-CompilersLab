all: test lex.cc  main.cc 
	g++   --std=c++0x lex.cc main.cc

test: lex.l 
	rm -rf a.out Scanner*
	flexc++ lex.l; 
	sed -i '/int lex()/r enums' Scanner.h

clean:
	rm -rf a.out Scanner* lex.cc




