#include <stdio.h>
#include <time.h>

#define N 1000

int main(void) {
    int is_composite[N + 1] = {0};
    int primes[N];
    int cnt = 0;

    clock_t start = clock();

    for (int i = 2; i <= N; i++) {
        if (!is_composite[i]) {
            primes[cnt++] = i;
        }
        for (int j = 0; j < cnt; j++) {
            if (i * primes[j] > N) break;
            is_composite[i * primes[j]] = 1;
            if (i % primes[j] == 0) break;
        }
    }

    clock_t end = clock();

    for (int i = 0; i < cnt; i++) {
        printf("%d ", primes[i]);
    }
    printf("\n");

    printf("total: %d\n", cnt);
    printf("time: %.6f s\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}