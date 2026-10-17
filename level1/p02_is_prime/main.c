#include <stdio.h>

int main(void) {
    long long n;

    while (1) {
        printf("please type an int number (Ctrl+Z then Enter to quit)\n");

        int ret = scanf("%lld", &n);
        if (ret == EOF) break;
        if (ret == 0) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("invalid input\n");
            continue;
        }

        if (n <= 1) {
            printf("%lld: neither prime nor composite\n", n);
            continue;
        }

        int is_prime = 1;
        for (long long d = 2; d <= n / d; d++) {
            if (n % d == 0) {
                is_prime = 0;
                break;
            }
        }

        if (is_prime)
            printf("%lld: it is a prime number\n", n);
        else
            printf("%lld: it is a composite number\n", n);
    }

    printf("end\n");
    return 0;
}
