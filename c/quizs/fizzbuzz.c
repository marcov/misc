#include <stdio.h>

int main(void)
{
    int i;
    
    for (i = 1; i< (100+1); i++)
    {
        if ((i % 3) == 0)
        {
            if ((i%5) == 0)
            {
                printf("FizzBuzz\n");
            }
            else 
            {
                printf("Fizz\n");
            }
        }
        else if ((i%5) == 0)
        {
                printf("Buzz\n");
        }
        else
        {
            printf("%d\n", i);
        }

        /*************/

        if ((i%5) && (i%3))
        {
            printf("%d\n", i);
        }
        else if (i%5)
        {
            printf("Fizz\n"); 
        }
        else if (i%3)
        {
            printf("Buzz\n");
        }
        else
        {
            printf("FizzBuzz\n");
        }
    }
   
    return 0;
}
