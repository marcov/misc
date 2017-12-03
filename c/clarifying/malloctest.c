#include <stdio.h>
#include <string.h>
#include <stdint.h>

static char stringtest[] = "CRACRACRICRACKER!";


void allocate1(char * buffer)
{
	buffer = stringtest;
}

void allocate2 (char ** buffer)
{	
	*buffer = stringtest;
}


int 
main(void)
{
	char * b = NULL;
	
	// This fails because allocate1 changes the _value_ of the passed parameter,
	// not the value pointed by the passed parameter.
	allocate1(b);
	printf("Allocate 1: %s\n", b);
	
	b = NULL;
	
	// This is OK, because the allocate2 changes the value _pointed_ by the passed parameter.
	allocate2(&b);
	printf("Allocate 2: %s\n", b);
	
	b = NULL;
}

