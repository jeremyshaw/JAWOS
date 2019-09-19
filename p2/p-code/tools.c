// tools.c, 159

...
...
...

void Number2Str(int x, char *str) {
	
	int len = 0;
	while(x>10){
		int temp;
		temp = x % 10;
		temp += '0'; //convert to ascii digits
		*str = temp;
		str++;		
		len++;
	}
	*str = x;
	
	//now, need to reverse the string to actually make it in the correct order. Don't forget to add a NULL.
   ...
   ...
   ...
}

