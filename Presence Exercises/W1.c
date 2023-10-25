#include <stdio.h>

int fib(int n)
{
    int a = 0;
    int b = 1;
    int temp = 0;

    for (int i = 0; i < n; ++i)
    {
        temp = a + b;
        a = b;
        b = temp;
    }

    return temp;
}

int fibr(int n)
{
    if (n == 0 || n == 1)
    {
        return 1;
    }   else
    {
        return fib(n-2) + fib(n-1);
    }
}

// The iterative approach is way faster than the recursive approach

int main()
{
    for (int i = 0; i < 10; ++i)
    {
        printf("%d: %d %d\n", i, fib(i), fibr(i));
    }
        
}