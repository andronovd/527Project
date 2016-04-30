/* 
this program finds the chromatic number of 
an undirected graph given an adjacency matrix
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define ITERS 20
#define DELTA 50
#define BASE  10

#define PRINT 0

#define CPG 2.533
#define GIG 1000000000

///////////////////////////////////////////////////////////////////////////////
//functions def
int Gamma( char*, int*, int );
int clock_gettime(clockid_t clk_id, struct timespec *tp);
void RandUGph( int*, int );
void Combine( int*, int, int , int );
void PrintMat( int*, int );
struct timespec diff( struct timespec,  struct timespec );
int clock_gettime(clockid_t clk_id, struct timespec *tp);
///////////////////////////////////////////////////////////////////////////////
//main
int main( int argc, char** argv )
{
	srand( time( NULL ) );
	int i, g;
	int cn[ITERS];
	int V = BASE + DELTA;
	int* mat;
	char* index;
	struct timespec times[ITERS];
	struct timespec time1, time2;
	
//timing
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		index = (char*) calloc( V, sizeof(char) );
		mat = (int* ) calloc( V*V, sizeof( int ) );
	  RandUGph( mat, V );		
		//PrintMat( mat, V );
	//perform timing
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
		cn[i] = Gamma ( index, mat, V );
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
		//printf("Gamma: %i\n", cn[i] );
	//record
		free( index );
	  times[i] = diff( time1, time2 );
	  V +=DELTA;
	}
	
	
//print output
	V = BASE + DELTA;
	printf("ITERS |  g   |  V   |TIME(nsec)\n");
	for( i = 0; i < ITERS; i++ )
	{
		printf("%6d|%6d|%6d| %li\n", i, cn[i], V, (long int)((double)(CPG)*(double)
		 (GIG * times[i].tv_sec + times[i].tv_nsec)));
		V +=DELTA;
	}		
	return 0;	
}
//end of main
///////////////////////////////////////////////////////////////////////////////
//function intialization
void RandUGph( int* mat, int V )
{
	int i,j,r;

	for( i = 0; i < V; i++ )
	{
		for( j = 0; j < V; j++ )
		{
			r = 0;
			if( i < j )
			{
				r = rand()&1;
				*( mat + i*V + j ) = r;
			}
			else if ( i == j )
			{
				//do nothing
			}
			else if ( i > j )
			{
				r = *( mat + j*V + i);
				*( mat +i*V + j ) = *( mat + j*V + i );
			}
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
int Gamma ( char* Index, int* A, int V )
{
	int i, j, a, ncombine = V, mask = 0;
	int* B; 
	for( i = 0; i < V; i++ )
	{
		if( Index[i] )
		{}
		else
		{
			for( j = 0; j < V; j++ )
			{
				if( Index[j] )
				{}
				else if( !*(A + i*V + j ) & i != j)
				{
					//found a disconnect
					ncombine--;
					Index[j] = 1;
					//cobmine
					Combine( A, V, i, j );
					i = 0;
					j = 0;
					//we found two non-adjacent vectors 	
					break;	
				}
			}
		}
	}
	return ncombine;		
}
///////////////////////////////////////////////////////////////////////////////
void Combine( int* A, int V, int i , int j )
{
	//or row i with row j and store in row j & reflect acroos the diagonal
	int x, a, b;
	for( x = 0; x < V; x++ )
	{
		a = *( A + j*V + x );
		b = *( A + i*V + x );
		b |= a;
		*(A + i*V + x ) = b;
	}
	
	//reflect
	for( x = 0; x < V; x++ )
	{
		*(A + x*V + i ) = *( A + i*V + x );
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
void PrintMat( int* A, int V )
{
	int i,j;
	for( i = 0; i < V; i++ )
	{
		for( j = 0; j < V; j++ )
		{
			printf("%i ", *( A + i*V + j ) );
		}
		printf( "\n" );
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
//timing mechanism
struct timespec diff(struct timespec start, struct timespec end)
{
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}
