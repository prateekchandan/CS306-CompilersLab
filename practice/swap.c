/*swap.c*/
extern int buf []; /*declaration buf*/
#define one 1
int *bufp0 = &buf[0]; /* initialized global */
int *bufp1;
/* uninitialized global */
void swap ()
/* definition swap */
{
int temp;
/* local */
//f();
bufp1 = &buf[one];
temp = *bufp0;
*bufp0 = *bufp1;
*bufp1 = temp;
}
