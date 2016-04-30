#include <stdio.h>

int main()
{
	int V = 10;
	int i, j;
	
	printf("V, i, j\n");
	for( i = 0; i < V; i++ )
	{
		for( j = 0; j < V; j++ )
		{
			printf( "%i, %i, %i\n", V, i, j );
		}
		V--;
	}
	
	return 0;
}
