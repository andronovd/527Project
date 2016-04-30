#include <stdio.h>

int global_var;

int main()
{
	int r1, r2;
	int f1();
	int f2 ();
	
	global_var = 10;
	r1 = f1();
	r2 = f2();
	
	printf("f1 returned %i\nf2 returned %i\n", r1, r2 );
	return 0;
}

int f1 ()
{
	int r = 0;
	int i;
	for( i = 0; i < global_var; i++ )
	{
		r++;
	}
	return r;
	
}

int f2()
{
	int r = 0;
	int i;
	for( i = 0; i < global_var; i++ )
	{
		r+=2;
	}
	return r;
}

