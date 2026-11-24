#include <limits.h>
#include <stdio.h>
#include<stdbool.h>
#include <string.h>
/*--------------赋值表-------------------------*/
#define AI 1
#define EMPTY 0                             //who
#define PLAYER (-1)
#define BOARD_SIZE 15
#define MAXDEPTH 8
#define WALL 2
#define CAND_MAX 64
#define SEARCH 16
#define WIN_SCORE  100000000
/* ---------- 分值表------------------------- */          //score为分值
#define S_FIVE         10000000
#define S_LIVE_FOUR     1000000
#define S_LIVE_THREE      200000
#define S_RUSH_FOUR       30000
#define S_SLEEP_THREE      8000
#define S_LIVE_TWO         4000
#define S_SLEEP_TWO         400
#define S_ONE                40
/*----------------------棋盘-------------------*/
int  board [BOARD_SIZE][BOARD_SIZE];     //row col

int point_val[BOARD_SIZE][BOARD_SIZE];

typedef struct VAL_STRUCT {
    int row;
    int col;
    int value;
} valstruct;

valstruct cand[CAND_MAX];//候选点
int n = 0;//计数用

/*------------------棋型索引-------------------*/
typedef enum {                                                     //pattern为棋型
    P_NONE = 0, P_ONE, P_SLEEP_TWO, P_LIVE_TWO,
    P_SLEEP_THREE, P_RUSH_FOUR,P_LIVE_THREE, P_LIVE_FOUR, P_FIVE
} Pat;

static const int PAT_SCORE[] = {
    0,              S_ONE,       S_SLEEP_TWO, S_LIVE_TWO,
    S_SLEEP_THREE, S_RUSH_FOUR,S_LIVE_THREE, S_LIVE_FOUR, S_FIVE
};

typedef struct { const char *pat; Pat type; } PatEntry;
/*----------------------------------------------*/
static const PatEntry PAT[] = {
    {"11111", P_FIVE},

    {"011110", P_LIVE_FOUR},

    {"211110", P_RUSH_FOUR}, {"011112", P_RUSH_FOUR},
    {"11011",  P_RUSH_FOUR}, {"10111",  P_RUSH_FOUR},
    {"11101",  P_RUSH_FOUR},

    {"011100", P_LIVE_THREE}, {"001110", P_LIVE_THREE},
    {"010110", P_LIVE_THREE}, {"011010", P_LIVE_THREE},

    {"211100", P_SLEEP_THREE}, {"001112", P_SLEEP_THREE},
    {"211010", P_SLEEP_THREE}, {"010112", P_SLEEP_THREE},
    {"210110", P_SLEEP_THREE}, {"011012", P_SLEEP_THREE},
    {"10011",  P_SLEEP_THREE}, {"11001",  P_SLEEP_THREE},
    {"10101",  P_SLEEP_THREE},

    {"0110", P_LIVE_TWO},

    {"2110", P_SLEEP_TWO}, {"0112", P_SLEEP_TWO}, {"1010", P_SLEEP_TWO},
};
/*----------------------------获取棋盘格子状态--------*/
int getval(int r, int c)
{
    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) return WALL;
    return board[r][c];
}
/*----------------------转化为pattern----------------*/
static void lineToStr(const int *L, int who, char *s)
{
    for (int i = 0; i < 9; ++i)
        s[i] = (L[i] == who) ? '1' : (L[i] == EMPTY ? '0' : '2');
    s[9] = '\0';
}

/* 返回该方向最强的、且“包含中心那子”的棋型 */
static Pat matchBest(const char *s)
{
    int n = (int)(sizeof PAT / sizeof PAT[0]);
    for (int i = 0; i < n; ++i) {
        int len = (int)strlen(PAT[i].pat);
        for (int st = 0; st + len <= 9; ++st) {
            if (st > 4 || st + len - 1 < 4) continue;
            if (strncmp(s + st, PAT[i].pat, len) == 0)
                return PAT[i].type;
        }
    }
    return P_ONE;
}
/*-------------------------邻居剪枝------------------------------------*/
bool has_neighbour(int board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    for (int dr = -2; dr <= 2; dr++) {
        for (int dc = -2; dc <= 2; dc++) {
            if (dr == 0 && dc == 0) continue;

            int nr = row + dr;
            int nc = col + dc;

            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (board[nr][nc] != EMPTY) {
                    return true;
                }
            }
        }
    }
    return false;
}
/*-----------------------------估值正--------------------------------*/
int evaluatepoint(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int who)
{
    static const int dr[4] = { 0, 1, 1,  1 };
    static const int dc[4] = { 1, 0, 1, -1 };
    int  L[9], total = 0, nThree = 0, nFour = 0, placed = 0;
    char s[10];

    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;

    if (board[row][col] == EMPTY) {
        board[row][col] = who; placed = 1;
        if (board[row][col] != who) return 0;
    }

    for (int d = 0; d < 4; ++d) {
        for (int i = -4; i <= 4; ++i)
            L[i + 4] = getval(row + dr[d] * i, col + dc[d] * i);

        lineToStr(L, who, s);
        Pat p = matchBest(s);
        total += PAT_SCORE[p];

        if (p == P_LIVE_THREE || p == P_SLEEP_THREE) ++nThree;
        else if (p == P_RUSH_FOUR)                   ++nFour;
    }

    /* 组合威胁：双三 / 四三 / 双冲四，威力≈活四，再补一份活四的分 */
    if (nThree >= 2 || (nThree >= 1 && nFour >= 1) || nFour >= 2)
        total += S_LIVE_FOUR;

    if (placed) board[row][col] = EMPTY;
    return total;
}

/*-------------------------------------------------------------------------*/
void print_board(int  board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\n");
    char player=' ';
    printf("| 0 ");
    for (int i = 0; i < 9; i++) {
        printf("| %d ", i+1);
    }
    for (int i = 9; i < BOARD_SIZE; i++) {
        printf("|%d ", i+1);
    }
    printf("|");
    printf("\n");
    for (int i = 0; i < BOARD_SIZE+1; i++){
        printf("+---");
    }printf("+\n");
    for(int j=0;j<BOARD_SIZE;j++) {
        if (j<9) {
            printf("| %d ",j+1);
        }
        else
            printf("|%d ",j+1);
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[j][i]==1)
                 player='x';
            if (board[j][i]==-1)
                 player='o';
            if (board[j][i]==0)
                 player=' ';
            printf("| %c ", player);

        }
        printf("|\n");
        for (int i = 0; i < BOARD_SIZE+1; i++){
             printf("+---");
        }printf("+\n");
    }

    printf("\n");
} //打印棋盘

bool is_full(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i=0;i<BOARD_SIZE;i++) {
        for (int j=0;j<BOARD_SIZE;j++) {
            if ((board[i][j])==EMPTY)return false;
        }
    }
    return true;
}

// 修改后的 check_winner
int check_winner(int who, int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return EMPTY;
    int dr[] = {0, 1, 1, 1};
    int dc[] = {1, 0, 1, -1};

    for (int d = 0; d < 4; d++) {
        int count = 1;
        for (int i = 1; i <= 4; i++) {
            int r = row + dr[d] * i, c = col + dc[d] * i;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && board[r][c] == who) count++;
            else break;
        }
        for (int i = 1; i <= 4; i++) {
            int r = row - dr[d] * i, c = col - dc[d] * i;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && board[r][c] == who) count++;
            else break;
        }
        if (count >= 5) return who;
    }
    return EMPTY;
}

   //int evaluate(int board[BOARD_SIZE][BOARD_SIZE],bool is_max){return 1;}
/*---------------------------------------*/
static int makes_five(int r, int c, int side)
{
    board[r][c] = side;
    int win = (check_winner(side, r, c) == side);
    board[r][c] = EMPTY;
    return win;
}
/*------------------如果下了判5连-----------------------------------*/
int side_val(int side) {
    int sidevalue=0;
    for (int r=0;r<BOARD_SIZE;r++) {
        for (int c=0;c<BOARD_SIZE;c++) {
            if (board[r][c]==side) {
                sidevalue+=evaluatepoint(board,r,c,side);
            }
        }
    }
    return sidevalue;
}
/*------------------------------全盘打分----------------------*/
int gen_cand(valstruct *out) {
    int n = 0;
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {

            if (board[r][c] != EMPTY)          continue;   /* ① 必须是空点 */
            if (!has_neighbour(board, r, c))   continue;   /* ② 附近半径 2 内要有子 */

            if (n >= CAND_MAX) break;                      /* ③ 防越界 */

            out[n].row = r;
            out[n].col = c;
            int atk = evaluatepoint(board, r, c, AI);
            int def = evaluatepoint(board, r, c, PLAYER);
            out[n].value = atk + def * 8 / 10;
            ++n;
        }
        if (n >= CAND_MAX) break;                          /* 外层也要跳，否则继续扫 */
    }

    if (n == 0) return 0;//空盘不要扫

    for (int i = 1; i < n; ++i) {
        valstruct t = out[i];
        int j = i - 1;
        while (j >= 0 && out[j].value < t.value) {
            out[j + 1] = out[j];
            --j;
        }
        out[j + 1] = t;
    }                                    //排序便于剪枝
    if (n > SEARCH) n = SEARCH;           //只保留16个点
    return n;
}
/*------------------------ 返回候选个数 在valstruct *out数组------------------------------*/

int maxmin(int depth, int side, int alpha, int beta) {
    if (depth == MAXDEPTH)
        return side_val(AI) - side_val(PLAYER) * 11 / 10;
    valstruct cand[CAND_MAX];
    int n = gen_cand(cand);
    if (n == 0) return 0;
    int best = (side == AI) ? INT_MIN : INT_MAX;
    for (int k = 0; k < n; ++k) {
        int r = cand[k].row;
        int c = cand[k].col;
        board[r][c] = side;
        int val;
        if (check_winner(side, r, c) == side) {
            val = (side == AI) ? (WIN_SCORE - depth) : -(WIN_SCORE - depth);
        } else {
            val = maxmin(depth + 1, -side, alpha, beta);
        }
        board[r][c] = EMPTY;

        if (side == AI) {
            if (val > best)   best  = val;
            if (best > alpha) alpha = best;
        } else {
            if (val < best)   best  = val;
            if (best < beta)  beta  = best;
        }
        if (alpha >= beta) break;
    }
    return best;
}


void best_move(int*col,int*row) {
    *col=-1;
    *row=-1;

    int stones = 0;
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
            if (board[r][c] != EMPTY) ++stones;

    if (stones == 0||board[7][7]==EMPTY) {
        *row = BOARD_SIZE / 2;
        *col = BOARD_SIZE / 2;
        return;
    }                                                     //开局天元

    /* 1) 自己能成五 */
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
            if (board[r][c] == EMPTY && has_neighbour(board, r, c) && makes_five(r, c, AI)) {
                *row = r; *col = c; return;
            }

    /* 2) 对手能成五 */
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
            if (board[r][c] == EMPTY && has_neighbour(board, r, c) && makes_five(r, c, PLAYER)) {
                *row = r; *col = c; return;
            }

    valstruct out[CAND_MAX]={0};


    int n=gen_cand(out) ;

    if (n == 0) {
        for (int r = 0; r < BOARD_SIZE && *row < 0; ++r)
            for (int c = 0; c < BOARD_SIZE; ++c)
                if (board[r][c] == EMPTY) { *row = r; *col = c; break; }
        return;
    }

    int bestmov=INT_MIN;


            for (int cnt=0;cnt<n;cnt++) {
                int i=out[cnt].row;
                int j=out[cnt].col;
                    board[i][j]=AI;
                    int mov_val=maxmin(1,PLAYER, bestmov,INT_MAX);
                    board[i][j]=EMPTY;
                    if (mov_val>bestmov) {
                        bestmov=mov_val;
                        *col=j;
                        *row=i;
                    }

            }

}


int main() {

print_board(board);
    printf("ready to start?");
    char getc;
    if (scanf("%c",&getc)!=1)return 0;
    if (getc=='n')return 0;
    while (1) {
            int player_row,player_col;
            int col,row;
        printf("please tap row and col");
/*----------------------------check valid----------------------------------*/
        if (scanf("%d %d",&player_row,&player_col)!=2) {
            //人为o ai为x
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }

        {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
        }

        if (player_row < 1 || player_row > BOARD_SIZE || player_col < 1 || player_col > BOARD_SIZE || board[player_row-1][player_col-1] != EMPTY) {
            printf("invalid1\n");
            continue;
        }
        board[player_row-1][player_col-1]=PLAYER;
/*--------------------------------------------------------------------------*/
        print_board(board);

        if (check_winner(PLAYER,player_row-1,player_col-1)==PLAYER){printf("you win\n");return 0;}

        best_move(&col,&row);

        board[row][col]=AI;

        print_board(board);

        printf("ok\n");

        bool l=is_full(board);
        if (l){printf("equal\n");return 0;}
        if (check_winner(AI,row,col)==AI){printf("ai win\n");return 0;}


    }

}
