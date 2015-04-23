#!/bin/bash

if [[ $1 = "reset" ]]; then
	rm -rf a.out code.asm
	cd src
	make clean
	cd ..
elif [[ $1 = "clean" ]]; then
	rm -rf a.out code.asm
elif [[ $1 = "exec" ]]; then
	code=./code.asm
	rm -rf a.out
	if [[ ! -f $code ]]; then
		echo "code.asm not present in the directory."
	else
		mv code.asm src/code.asm
		cd src
		g++ -std=c++11 -g3 -m32 -Wno-write-strings -O0 machine.cc;
		g++ -std=c++11 -g3 -m32 -Wno-write-strings -O0 machine_stats.cc -o b.out;
		if [[ $2 = "-s" ]]; then
			rm -rf a.out
			mv b.out ../a.out
		else
			rm -rf b.out
			mv a.out ..
		fi
		mv code.asm ..
		cd ..
		echo "executable file produced."
	fi
else
	file=./compiler
	rm -rf code.asm a.out
	cd src
	if [[ ! -f $file ]]; then
		make compiler
	fi
	make exe
	mv code.asm ..
	if [[ $1 = "-s" ]]; then
		rm -rf a.out
		mv b.out ../a.out
	else
		rm -rf b.out
		mv a.out ..
	fi
	cd ..
fi
