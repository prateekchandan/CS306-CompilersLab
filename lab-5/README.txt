Compilers Lab-5 Assignment Submission
(copyright owned by our team)

Team Members:
Shyam JVS       (120050052)
Prateek Chandan (120050042)

Instructions to run:
1) Write the code you want to compile in test.c (in the project root directory)
2) Run the following command: ./gen.sh [-s] 
(-s is an optional flag for creating an executable which displays statistics of the code along with output)
[
After doing the above note that:
Inside the project root directory:
- The assembly code would be produced in a file named code.asm
- The executable for code.asm would be produced with the name a.out
]
3) If you want to just modify code.asm and then create an a.out, then modify it and run: ./gen.sh exec [-s]
4) If you want to clean up just code.asm and a.out, run: ./gen.sh clean
5) If you want to clean up all the project executables (i.e. compiler exe, bison and lex produced code) 
and reset to default, then run: ./gen.sh reset

