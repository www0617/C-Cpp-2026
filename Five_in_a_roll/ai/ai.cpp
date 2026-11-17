#include <stdio.h>

#define AI 0
#define PLAYER 1
#define BOARD_SIZE 15

typedef struct CHESS {
    int side;     //ai 0,player 1
    int i;        //[i][j]
    int j;
} chess;

chess board [BOARD_SIZE][BOARD_SIZE];
int main() {
    printf("%d",board [2][3].i);
}
