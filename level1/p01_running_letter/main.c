#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define DELAY_MS 30

static HANDLE out(void) { return GetStdHandle(STD_OUTPUT_HANDLE); }

static int window_width(void) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(out(), &info)) return 80;
    return info.srWindow.Right - info.srWindow.Left + 1;
}

static void set_cursor_visible(int visible) {
    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(out(), &info)) return;
    info.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(out(), &info);
}

static void draw_at(const char *text, int pos, int width) {
    COORD home = {0, 0};
    DWORD written;
    FillConsoleOutputCharacterA(out(), ' ', (DWORD)width, home, &written);
    SetConsoleCursorPosition(out(), home);
    for (int i = 0; i < pos; i++) putchar(' ');
    fputs(text, stdout);
    fflush(stdout);
}

int main(void) {
    const char *text = "a";
    const int text_len = (int)strlen(text);
    int pos = 0;
    int step = 1;

    set_cursor_visible(0);
    printf("type 1 to start (any key to stop): ");

    if (getchar() != '1') {
        set_cursor_visible(1);
        return 0;
    }

    while (!_kbhit()) {
        int width = window_width();
        int max_pos = width - text_len - 1;
        if (max_pos < 1) max_pos = 1;

        draw_at(text, pos, width);

        if (pos >= max_pos) step = -1;
        if (pos <= 0) step = 1;
        pos += step;

        Sleep(DELAY_MS);
    }

    printf("\nstopped.\n");
    set_cursor_visible(1);
    return 0;
}