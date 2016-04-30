/*
This program generates a random graph with n verticies and random 
number of edges. 
Inputs:
N - user specified number of vertices

Ouputs:
A text file specifing the names of the edges and the adjacency list.
Information in the file is separated by the character '-'.

The names of the egdes start at 'A' then to 'a' then to 'AA' then to 'aa'
and so on and so forth using both lower case and upper case as one bit of
the name.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char** argv )
{
	printf("This program generates a random graph with n verticies and random number of edges.\n Inputs:\nN - user specified number of vertices\n\nOuputs:\nA text file specifing the adjacency list.\n\n\n");
	
	if( argc != 2 )
	{
		printf("Error: expected one agrument: <int>\n");
		return 0;
	}
	
	int V = atoi( argv[1] );
	int i,j,r;
	int* mat = (int* ) calloc( V*V, sizeof( int ) );
	char* fileName = "graph.txt\0";
	
	
	FILE *f;
	f = fopen( fileName, "w" );
	
//genereate the random connections and output to file
	srand( time( NULL ) );	
	printf("output to file:\n");
	for( i = 0; i < V; i++ )
	{
		for( j = 0; j < V; j++ )
		{
			r = 0;
			if( i < j )
			{
				r = rand()&1;
				*( mat + i*V + j ) = r;
				fprintf( f, "%i ", r );
				printf("%i ", r );
			}
			else if ( i == j )
			{
				fprintf( f, "%i", r );
				printf("%i ", r );
			}
			else if ( i > j )
			{
				r = *( mat + j*V + i);
				fprintf( f, "%i ", r );
				printf("%i ", r );
			}
		}
		fprintf( f, "\n" );
		printf("\n");
	}	
	free( mat );
	fclose( f );
	return 0;
}
