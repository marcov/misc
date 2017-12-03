#include <stdio.h>


int main()
{
	const int * cintgr; 		// pointer to a constant integer value.
	int * const cptr;			// constant pointer to an integer value.
	
	(*cptr)++;		// doesnt raise error: increments value _pointed_ by a constant pointer.
	
	cptr++;			// raises error:  increments the pointer _value_.
					// Error message: "increment of read-only variable 'cptr'"
	
	cintgr++;		// doesnt raise error: increments the pointer _value_.
	
	(*cintgr)++;	// raises error:  increments constant value _pointed_ by a pointer.
					// Error message: "increment of read-only location"
	return 0;
}