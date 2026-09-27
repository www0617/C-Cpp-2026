#include <stdio.h>
#include<stdlib.h>
#include<windows.h>
#include <conio.h>
static int get_width(void) {
    CONSOLE_SCREEN_BUFFER_INFO i;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &i);
    return i.srWindow.Right - i.srWindow.Left + 1;
}
void hideCursor() {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &cursorInfo);
}
int main () {
    hideCursor();
    printf("type 1 to start,type any key to end");
    if (getchar()=='1') {
        int i,j=0,k;int width = get_width();
        while (1) {
            if (j==0) {i=0;
                for (k=0;k<width;k++) { if (kbhit()) return 0;
                    for (i=0;i<k;i++) {putchar(' ');}
                    putchar('a');
                    Sleep(30);
                    system("cls");
                }
                j=1;

            }
            else  {
                for (k=width-2;k>0;k--) { if (kbhit()) return 0;
                    for (i=0;i<k;i++) {putchar(' ');}
                    putchar('a');
                    Sleep(30);
                    system("cls");
                }j=0;
            }
        }
    }
}