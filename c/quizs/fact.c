#include <stdio.h>
#include <stdint.h>


static uint32_t
my_fact(uint32_t n)
{
	uint32_t res = n;
	
	while (n > 1)
	{
		res *= (--n);
	}
	
	return res;
}


int main(int argc, char * argv[])
{
	uint32_t n;
	
	if (argc > 1)
	{
		n = atoi(argv[1]);
	}
	
	printf("Calculating factorial of %ul...: %ul \n", n, my_fact(n));	
}  