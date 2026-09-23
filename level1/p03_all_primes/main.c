#include <stdio.h>
#include <time.h>

static int test(int i, const int *m, const int *k)
{
    for (const int *g = m; g < k; g++) {
        if ((*g) * (*g) > i) return 1;
        if (i % (*g) == 0)   return 0;
    }
    return 1;
}

int main(void)
{
    clock_t t0 = clock();

    int arr[100000];
    int *k = arr;
    int *m = arr;

    for (int i = 2; i <= 100000; i++) {
        if (test(i, m, k)) {
            *k = i;
            k++;
        }
    }

    for (int *p = arr; p < k; p++)
        printf("%6d", *p);

    clock_t t1 = clock();
    printf("\ntime: %.6f second\n",
           (double)(t1 - t0) / CLOCKS_PER_SEC);
    return 0;
}