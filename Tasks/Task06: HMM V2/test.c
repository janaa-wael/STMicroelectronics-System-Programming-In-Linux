#include <stdio.h>
#include <stdlib.h>

int main()
{
	int * ptr = malloc(50);
	for(int i = 0 ; i < 50 ; i++)
	{
		*ptr = i;
		ptr++;
	}
	int * ptr2 = calloc(40,2);
	printf("%d %d",ptr2[10], ptr2[45]);
	free(ptr);
	free(ptr2);

}
