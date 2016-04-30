/* 
this program finds the chromatic number of 
an undirected graph given an adjacency matrix

gcc -msse4.1 -O1 -o ugvec.exe Ugamma_vec.c -lrt -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <xmmintrin.h>
#include <smmintrin.h>

#define ITERS 100
#define DELTA 0
#define BASE  100

#define PRINT 0
#define OPTIONS 2

#define CPG 2.533
#define GIG 1000000000

///////////////////////////////////////////////////////////////////////////////
//functions def
int Gamma( char*, int*, int );
int Vec_Gamma( char*, int*, int );
int clock_gettime(clockid_t clk_id, struct timespec *tp);
void RandUGph( int*, int );
void PrintMat( int*, int );
void Combine( int*, int, int , int );
void Vec_Combine( int*, int, int , int );
struct timespec diff( struct timespec,  struct timespec );
int clock_gettime(clockid_t clk_id, struct timespec *tp);
///////////////////////////////////////////////////////////////////////////////
//main
int main( int argc, char** argv )
{
	srand( time( NULL ) );
	int i, j, g, opt;
	int cn[ITERS];
	int V = BASE + DELTA;
	int* mat;
	int* ckmat;
	char* index;
	struct timespec times[OPTIONS][ITERS];
	struct timespec time1, time2;
	
//timing
//serial version first
	opt = 0;
	int z,y;
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		index = (char*) calloc( V, sizeof(char) );
		mat = (int* ) calloc( V*V, sizeof( int ) );
		ckmat = (int* ) calloc( V*V, sizeof( int ) );
	  RandUGph( mat, V );	
	  for( z = 0; z < V; z++ )
	  {
	  	for( y = 0; y< V; y++ )
			{
				*(ckmat + z*V + y ) = *(mat + z*V + y );
			}
		}
		//PrintMat(mat, V );
		
	//perform timing
		clock_gettime(CLOCK_MONOTONIC, &time1);
		cn[i] = Gamma ( index, mat, V );
		clock_gettime(CLOCK_MONOTONIC, &time2);
	//record
		//printf("Serial result: %i\n", cn[i] );
		free( index );
	  times[opt][i] = diff( time1, time2 );
	  V +=DELTA;
	}
//threaded version second
	opt++;
	int ck;
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		index = (char*) calloc( V, sizeof(char) );
		mat = (int* ) calloc( V*V, sizeof( int ) );
	  RandUGph( mat, V );			
		//PrintMat( ckmat, V );
	//perform timing
		clock_gettime(CLOCK_MONOTONIC, &time1);
		ck = Vec_Gamma ( index, ckmat, V );
		clock_gettime(CLOCK_MONOTONIC, &time2);
		//printf("Vectorized result: %i\n", ck );
	//record
		free( index );
	  times[opt][i] = diff( time1, time2 );
	  V +=DELTA;
	}	
	
//print output
	V = BASE + DELTA;
	printf("TIME(nsec)\nSerial|Vectorized\n");
	for( j = 0; j < ITERS; j++ )
	{
		for( i = 0; i < OPTIONS; i++ )
		{
			printf("%6d ", (long int)((double)(CPG)*(double)
			 (GIG * times[i][j].tv_sec + times[i][j].tv_nsec)));
			V +=DELTA;
		}	
		printf("\n");
	}		
	
	//check for correctness
	
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
//printing a matrix
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
//serial version of gamma
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
///combine function
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
//Vector version of Gamma
int Vec_Gamma ( char* Index, int* A, int V )
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
					Vec_Combine( A, V, i, j );
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

//vector combine
void Vec_Combine( int* A, int V, int i , int j )
{
	//cast the arrays as vectors
	__m128i* src = (__m128i*)( A + i*V );
	__m128i* dest = (__m128i*)(A + j*V );
	//or row i with row j and store in row j & reflect acroos the diagonal
	int x, step = V/8;
	for( x = 0; x < step; x++ )
	{
		*src = _mm_or_si128( *src, *dest );
		src++;
		dest++;
	}
	
	//reflect
	for( x = 0; x < V; x++ )
	{
		*(A + x*V + i ) = *( A + i*V + x );
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
