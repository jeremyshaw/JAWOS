/* sample.c */
#include<spede/stdio.h>
#include<spede/flames.h>

int main(void){
	long	i;
	
	i = 128;
	printf("%d Hello world %d \nECS", i, 2 * i);
	cons_printf("--> Hello world <--\nCPE/CSC"); /*target printf*/
	return 0;
}
// random note, Py \ means continue intpreted code on the next line.
//py reverse indexing arr[-1] will return last letter