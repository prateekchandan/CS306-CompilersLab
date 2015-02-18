#include <stdio.h> /* included system
header files */
/*main.c*/
extern void swap (); /* declaration */
int buf [2] = {34,56}; /* initialised global */
int main () /* definition main */
{
swap ();
printf("buf[0]= %d buf[1]= %d\n", buf[0], buf[1]);
return 0;
}
