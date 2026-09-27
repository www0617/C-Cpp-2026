#include <stdio.h>

int main() {
    while (1){
   long long  i;
    printf("please type an int number\n");
        int c=scanf("%lld", &i);
        if (c == EOF) {
            break;
        }
            if (c==0) {
                while (getchar() != '\n'); printf("invalid input \n");continue;
            }

    if (i <= 1) {
        printf("neither prime nor composite\n");
        continue;
    }
    if (i > 1) {
        int is_prime = 1;
        for (long long j = 2; j  <= i/j; j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }

        if (is_prime)
            printf("it is a prime number\n");
        else
            printf("it is a composite number\n");
    }
    } printf("end");return 0;
}