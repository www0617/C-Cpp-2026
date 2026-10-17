#include <stdio.h>

#define DISKS 4

void hanoi(int n, char from, char to, char aux) {
    if (n <= 0) return;

    hanoi(n - 1, from, aux, to);
    printf("%c -> %c\n", from, to);
    hanoi(n - 1, aux, to, from);
}

int main(void) {
    hanoi(DISKS, 'A', 'C', 'B');
    return 0;
}
