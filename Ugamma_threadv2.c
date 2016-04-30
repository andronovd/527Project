/* 
this program finds the chromatic number of 
an undirected graph given an adjacency matrix.

Compilation Instructions:
gcc -pthread -o Ugamma_threadv2.exe Ugamma_threadv2.c -lm -lrt
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define ITERS 10
#define DELTA 0
#define BASE  10000

#define PRINT 0
#define OPTIONS 2

#define CPG 2.533
#define GIG 1000000000

#define NUMTHREADS 4

pthread_mutex_t mutexcombine;
///////////////////////////////////////////////////////////////////////////////
//functions def
int Gamma( char*, int*, int );
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int* RandUGph( int );
void Combine( int*, int, int , int );
void PrintMat( int*, int );
void callThread( char*, int*, int );
void ThreadGamma( void* );
struct timespec diff( struct timespec,  struct timespec );
///////////////////////////////////////////////////////////////////////////////
//combine thread argument struct
typedef struct THREAD_ARGS
{
	char* index;
	int* adj;
	int V;
	int op;
	int start;
	int end;
	int ans;
}	args;

///////////////////////////////////////////////////////////////////////////////
//main
int main( int argc, char** argv )
{
	srand( time( NULL ) );
	int i, j, g, opt, ck;
	int cn[ITERS];
	int V = BASE + DELTA;
	int* mat;
	char* index;
	struct timespec times[OPTIONS][ITERS];
	struct timespec time1, time2;
	
//timing
//serial version first
	opt = 0;
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		index = (char*) calloc( V, sizeof(char) );
		mat = RandUGph( V );		
		//PrintMat( mat, V );
	//perform timing
		clock_gettime(CLOCK_MONOTONIC, &time1);
		ck = Gamma ( index, mat, V );
		clock_gettime(CLOCK_MONOTONIC, &time2);
	//record
	  //printf("Gamma: %i\n", ck );
		free( index );
	  times[opt][i] = diff( time1, time2 );
	  V +=DELTA;
	}
//threaded version second
	opt++;
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		index = (char*) calloc( V, sizeof(char) );
		mat = RandUGph( V );		
		//PrintMat( mat, V );
	//perform timing
		clock_gettime(CLOCK_MONOTONIC, &time1);
		callThread( index, mat, V );
		clock_gettime(CLOCK_MONOTONIC, &time2);
		//printf("Gamma: %i\n", ck );
	//record
		free( index );
	  times[opt][i] = diff( time1, time2 );
	  V +=DELTA;
	}	
	
//print output
	V = BASE + DELTA;
	printf("TIME(nsec)\nSerial|Threaded\n");
	for( j = 0; j < ITERS; j++ )
	{
		for( i = 0; i < OPTIONS; i++ )
		{
			printf("%6f ", (float)((double)(CPG)*(double)
			 (GIG * times[i][j].tv_sec + times[i][j].tv_nsec))/1000000000);
			V +=DELTA;
		}	
		printf("\n");
	}	

	
	return 0;	
}
//end of main
///////////////////////////////////////////////////////////////////////////////


//function intialization
int* RandUGph( int V )
{
	int i,j,r;
	int* mat = (int* ) calloc( V*V, sizeof( int ) );

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
	return mat;
}
//matrix printing functions - mostly for debugging use
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

//Serial Gamma Function
int Gamma ( char* Index, int* A, int V )
{
	int i, j, a, ncombine = V;
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
					//we found two non-adjacent vectors 	
					break;	
				}
			}
		}
	}
	return ncombine;		
}
//combination function
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


////thread functions
void callThread( char* Index, int* A, int V )
{
	int i, t, j, chunk = V/NUMTHREADS;
	
	pthread_mutex_init( &mutexcombine, NULL );
	pthread_t threads[NUMTHREADS];
	args a[NUMTHREADS];
	
//create threads and send them to work
	for( i = 0; i < NUMTHREADS; i++ )
	{
	//package the thread arguments
		a[i].index = Index;
		a[i].adj = A;
		a[i].V = V;
		a[i].op = i;
		a[i].start = i*chunk;
		a[i].end = (i+1)*chunk;
	//call to thread_create
		t = pthread_create(&threads[i], 
											 NULL, 
											 ThreadGamma,
											 (void*) &a[i]);
		if( t )
		{
			printf("Error creating: thread No.%i failed, error code %d\n", i, t );
			exit( -1 );
		}
	}
	
//join the threads back together
	for( i = 0; i < NUMTHREADS; i++ )
	{
		j = pthread_join( threads[i], NULL );
		if( j )
		{
			printf("Error joining: thread No.%i failed, error code %d\n", i, j );
			exit( -1 );
		}
	}
//done	
	
}
//threaded Gamma function
void ThreadGamma ( void* Targ )
{

	//unpack arguments
	args* argv = (args*) Targ;
	char* Index = argv->index;
	int*  A = argv->adj;
	int 	V = argv->V;
	int start = argv->start;
	int end   = argv->end;
	int op		= argv->op;
	
  printf("threadID<%i>,i ranges from %i to %i\n", pthread_self(), start, end );
	
	int i, j, a, ncombine = V;
	for( i = start; i < end; i++ )
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
					pthread_mutex_lock( &mutexcombine );
					ncombine--;
					Index[j] = 1;
					//cobmine
					
				  Combine( A, V, i, j );
					i = 0;
					j = 0;
					//we found two non-adjacent vectors 
					pthread_mutex_unlock( &mutexcombine );	
					break;	
				}
			}
		}
	}
	argv->ans = ncombine;
	return;
}
//get the correct bounds to perform operations on the correct elements in 
//adjacency matrix matrix A
	
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
