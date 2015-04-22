#!/bin/bash

if [[ $1 = "reset" ]]; then
	rm -rf a.out code.asm
	cd src
	make clean
	cd ..
elif [[ $1 = "clean" ]]; then
	rm -rf a.out code.asm
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
