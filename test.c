/*
This is the basic serial code that generates a random
adjacency matrix and finds the chromatic number  of the graph
defined by that matrix.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(  )
{
	srand( time( NULL ) );
	void Undir( int );
	void Dir( int );
	
	
	while( 1 )
	{
		char c[1];
		char d[1];
	
		printf("What kind of graph do you want?\nUndirected( 0 ) or Directed( 1 ): " );
		scanf( "%s", c );
	
		if( *c == 'q' )
		{
			printf("exiting the program\n");
			return 0; //quit the program
		}
		else if( *c == '0' )
		{
			printf("undirected\nhow many verticies do you want?: ");
			scanf( "%s", d );
			
			Undir( atoi( d ) );
		}
		else if ( *c == '1' )
		{
			printf("directed\nhow many verticies do you want?: ");
			scanf( "%s", d );
			
			Dir( atoi( d ) );
		}
		else
		{
			printf("incorrect input, please try again\n");
		}
	}
	
}

void Undir( int n )
{
	int V = n;
	int i,j,r;
	int* mat = (int* ) calloc( V*V, sizeof( int ) );
	
	char* fileName = "graphs.txt\0";	
	FILE *f;
	f = fopen( fileName, "a" );
	
//genereate the random connections and output to file	
	fprintf(f, "---\n" );
	printf("---------\n" );
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
				fprintf( f, "%i ", r );
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
}

void Dir( int n )
{
	int V = n;
	int i,j,r;
	int* mat = (int* ) calloc( V*V, sizeof( int ) );
	
	char* fileName = "graphs.txt\0";	
	FILE *f;
	f = fopen( fileName, "a" );
	
//genereate the random connections and output to file	
	//fprintf(f, "Matrix %i\n", Count );
	//printf("Matrix %i\n", Count );
	fprintf( f, "---------\n" );
	printf("---------\n" );
	for( i = 0; i < V; i++ )
	{
		for( j = 0; j < V; j++ )
		{
			r = 0;
			if ( i == j )
			{
				fprintf( f, "%i ", r );
				printf("%i ", r );
			}
			else
			{
				r = rand()&1;
				fprintf( f, "%i ", r );
				printf("%i ", r );
			}
		}
		fprintf( f, "\n" );
		printf("\n");
	}	
	free( mat );
	fclose( f );
	return;
}
