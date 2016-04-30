/* 
this program finds the chromatic number of 
an undirected graph given an adjacency matrix.

Compilation Instructions:
gcc -pthread -o Ugamma_thread.exe Ugamma_thread.c -lm -lrt
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define ITERS 1
#define DELTA 0
#define BASE  10

#define PRINT 0
#define OPTIONS 2

#define CPG 2.533
#define GIG 1000000000

#define NUMTHREADS 2

int V;
int* A;
char* Index;
int chunk;
int row1;
int row2;
///////////////////////////////////////////////////////////////////////////////
//functions def
int Gamma( char*, int*, int );
int ThreadGamma( char*, int*, int );
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int* RandUGph( int );
void Combine( int*, int, int , int );
void PrintMat( int*, int );
void callThreads();
void* ThreadCombine( void* );
struct timespec diff( struct timespec,  struct timespec );
///////////////////////////////////////////////////////////////////////////////
//thread argument struct
typedef struct thread_args
{
	int id;
	int V;
	int m;
	int n;
	int start;
	int end;
	int* adj;
} thread_arg;

///////////////////////////////////////////////////////////////////////////////
//main
int main( int argc, char** argv )
{
	srand( time( NULL ) );
	int i, j, g, opt, ck;
	int cn[ITERS];
	int V = BASE + DELTA;
	int chunk = V/NUMTHREADS;
	//int* mat;
	//char* index;
	struct timespec times[OPTIONS][ITERS];
	struct timespec time1, time2;
	
//timing
//serial version first
	opt = 0;
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		Index = (char*) calloc( V, sizeof(char) );
		A = RandUGph( V );	
		chunk = V/NUMTHREADS;	
		//PrintMat( mat, V );
	//perform timing
		clock_gettime(CLOCK_MONOTONIC, &time1);
		ck = Gamma ( Index, A, V );
		clock_gettime(CLOCK_MONOTONIC, &time2);
	//record
	  //printf("Gamma: %i\n", ck );
		free( Index );
	  times[opt][i] = diff( time1, time2 );
	  V +=DELTA;
	}
//threaded version second
	opt++;
	for( i = 0; i < ITERS; i++ )
	{
	//intialize arrays
		Index = (char*) calloc( V, sizeof(char) );
		A = RandUGph( V );	
		chunk = V/NUMTHREADS;
		printf("CHUNK: %i\n", chunk );		
		//PrintMat( mat, V );
	//perform timing
		clock_gettime(CLOCK_MONOTONIC, &time1);
		ck = ThreadGamma ( Index, A, V );
		clock_gettime(CLOCK_MONOTONIC, &time2);
		//printf("Gamma: %i\n", ck );
	//record
		free( Index );
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
			printf("%6lf ", (float)((double)(CPG)*(double)
			 (GIG * times[i][j].tv_sec + times[i][j].tv_nsec))/GIG);
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
		/*
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
			*/
			*(mat + i*V + j ) = 0;
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
int Gamma ( char* Index, int* A, int V )
{
	int i, j, ncombine = V;
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
		*( A + i*V + x ) |= *( A + j*V + x );
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
//thread creating function
void callThreads()
{
	pthread_t threads[NUMTHREADS];
	
	//create 2 threads
	int id = 0;
	pthread_create(&threads[id], 
								 NULL, 
								 ThreadCombine,
								 (void*) &id);
	id++;		 
	pthread_create(&threads[id], 
								 NULL, 
								 ThreadCombine,
								 (void*) &id);


	//join
	pthread_join( threads[0], NULL );
	pthread_join( threads[1], NULL );
	
	
	/*(
//create threads and send them to work
	for( i = 0; i < NUMTHREADS; i++ )
	{
	//package the thread arguments
		//args[i].id = i
	//call to thread_create
		t = pthread_create(&threads[i], 
											 NULL, 
											 ThreadCombine,
											 (void*) &i);
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
//done	*/
}
//thread work function
void* ThreadCombine( void* arg )
{	
	int myId = *( (int*) arg );
	int start = myId * chunk;
	int end = start + chunk;
	
	printf("No.<%i>, %i->%i: %i %i\n", myId, start, end, chunk, V );
	
//combination 
	int x;
	for( x = start; x < end; x++ )
	{
		*( A + row1*V + x ) |= *( A + row2*V + x );
	}

//then reflection
	for( x = start; x < end; x++ )
	{
		*(A + x*V + row1 ) = *( A + row1*V + x );
	}
	return;
}

//threaded Gamma function
int ThreadGamma ( char* Index, int* A, int V )
{
	printf(" chunk:%i V:%i\n", chunk, V );
	pthread_t threads[NUMTHREADS];
	
	int a, ncombine = V, id;
	for( row1 = 0; row1 < V; row1++ )
	{
		if( Index[row1] )
		{}
		else
		{
			for( row2 = 0; row2 < V; row2++ )
			{
				if( Index[row2] )
				{}
				else if( !*(A + row1*V + row2 ) & row1 != row2)
				{
					//found a disconnect
					ncombine--;
					Index[row2] = 1;
					//cobmine
	
					//create 2 threads
					id = 0;
					pthread_create(&threads[id], 
												 NULL, 
												 ThreadCombine,
												 (void*) &id);
					id++;		 
					pthread_create(&threads[id], 
												 NULL, 
												 ThreadCombine,
												 (void*) &id);


					//join
					pthread_join( threads[0], NULL );
					pthread_join( threads[1], NULL );
		
					break;	
				}
			}
		}
	}
	return ncombine;		
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
