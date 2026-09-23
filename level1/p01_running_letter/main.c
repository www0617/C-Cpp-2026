#include <stdio.h>
#include<stdlib.h>
#include<windows.h>
static int get_width(void) {
    CONSOLE_SCREEN_BUFFER_INFO i;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i);
    return i.srWindow.Right - i.srWindow.Left + 1;
}
int main (void) {
    int i,j=0,k;int width = get_width();
    while (1) {
        if (j==0) {i=0;
            for (k=0;k<width;k++) {
                for (i=0;i<k;i++) {putchar(' ');}
                putchar('a');
                Sleep(10);
                system("cls");
            }
            j=1;

        }
        else  {
            for (k=width-2;k>0;k--) {
                for (i=0;i<k;i++) {putchar(' ');}
                putchar('a');
                Sleep(10);
                system("cls");
            }j=0;
        }
    }
}