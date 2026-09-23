#include <stdio.h>

int main(void)
{
    int i;
    printf("please type an int number\n");
    scanf("%d", &i);

    if (i <= 1) {
        printf("it is not a prime number\n");
        return 0;
    }

    int is_prime = 1;
    for (int j = 2; j * j <= i; j++) {
        if (i % j == 0) {
            is_prime = 0;
            break;
        }
    }

    if (is_prime)
        printf("it is a prime number\n");
    else
        printf("it is not a prime number\n");

    return 0;
}