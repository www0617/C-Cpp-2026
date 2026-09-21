#include <stdio.h>
#include<stdlib.h>
#include<windows.h>
int main (void) {
    int i,j=0,k;
    while (1) {
        if (j==0) {i=0;
            for (k=0;k<10;k++) {
                for (i=0;i<k;i++) {putchar(' ');}
                putchar('a');
                Sleep(100);
                system("cls");
            }
            j=1;

        }
        if (j==1) {
            for (k=10;k>0;k--) {
                for (i=0;i<k;i++) {putchar(' ');}
                putchar('a');
                Sleep(100);
                system("cls");
                j=0;
            }
        }
    }
}