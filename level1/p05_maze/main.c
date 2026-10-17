#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#define H 11
#define W 19

/* 迷宫地图：'#' = 墙，' ' = 通路，'E' = 出口 */
char maze[H][W + 1] = {
    "###################",
    "#                 #",
    "# ##### ######### #",
    "# #   #       #   #",
    "# # # # ##### # # #",
    "#   # #     # #   #",
    "##### ##### # #####",
    "#     #   # #     #",
    "# ####### # ##### #",
    "#         #      E#",
    "###################",
};

/* 玩家坐标（全局，函数里可直接修改） */
int px = 1;
int py = 1;

/* 画迷宫：逐格输出，玩家所在格显示为 'P' */
void printmaze(void)
{
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (x == px && y == py) {
                putchar('P');
            } else {
                putchar(maze[y][x]);
            }
        }
        putchar('\n');
    }
}

/* 读一个方向，统一翻译成 'w'/'a'/'s'/'d'；其余字符原样返回 */
int readkey(void)
{
    int k = _getch();

    /* Windows 原生控制台：方向键 = 前缀(0 或 224) + 方向码 */
    if (k == 0 || k == 224) {
        switch (_getch()) {
            case 72: return 'w';   /* up */
            case 80: return 's';   /* down */
            case 75: return 'a';   /* left */
            case 77: return 'd';   /* right */
            default: return 0;
        }
    }


    if (k == 27) {
        if (_getch() == '[') {
            switch (_getch()) {
                case 'A': return 'w';
                case 'B': return 's';
                case 'C': return 'd';
                case 'D': return 'a';
                default:  return 0;
            }
        }
        return 0;
    }

    return k;
}


int controlp(void)
{
    int key = readkey();

    if (key == 'q' || key == 'Q') {
        return -1;
    }

    int nx = px;
    int ny = py;

    if (key == 'w') {
        ny = py - 1;
    } else if (key == 's') {
        ny = py + 1;
    } else if (key == 'a') {
        nx = px - 1;
    } else if (key == 'd') {
        nx = px + 1;
    }

    if (ny >= 0 && ny < H && nx >= 0 && nx < W && maze[ny][nx] != '#') {
        px = nx;
        py = ny;

        if (maze[py][px] == 'E') {
            return 1;
        }
    }

    return 0;
}

int main(void)
{
    while (1) {
        system("cls");
        printmaze();
        printf("\nArrow keys / WASD to move, Q to quit\n");

        int result = controlp();

        if (result == 1) {
            printf("\nYou win!\n");
            printf("Press any key to exit...");
            _getch();
            break;
        }

        if (result == -1) {
            printf("\nGame over.\n");
            break;
        }
    }
    return 0;
}
