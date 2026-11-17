/* ============================================================
 * board.h —— 棋盘类型与公共工具（C 接口，src 和 ai 共用）
 *
 * 棋盘 = chessboard 结构体数组，每格自带 r / c / state。
 * 只有一份数据，落子直接改 state：
 *     view_cells[i][j].state = BLACK_STONE;
 *
 * 函数统一用指针接收：chessboard (*b)[BOARD_N]
 * 调用时直接传数组名（会自动退化成指针）。
 *
 * 注意：EasyX 的 easyx.h 把 BLACK/WHITE 定义成了颜色宏（0 / 0xFFFFFF），
 *       所以棋子常量叫 BLACK_STONE / WHITE_STONE。
 * ============================================================ */
#ifndef FIAR_BOARD_H
#define FIAR_BOARD_H

#define BOARD_N 15

#define BLACK_STONE 1
#define WHITE_STONE (-1)
#define EMPTY 0

/* 候选点：只搜已有棋子周围 CAND_RADIUS 格 */
#define CAND_RADIUS 2
#define CAND_MAX 128

/* 一个交叉点：位置 + 状态 */
typedef struct Chessboard {
    int r;          /* 行 0~14 */
    int c;          /* 列 0~14 */
    int state;      /* 1 黑 / -1 白 / 0 空 */
} chessboard;

/* 整个棋盘的类型（15 行） */
typedef chessboard Board[BOARD_N][BOARD_N];

typedef struct move_s {
    int r;
    int c;
} move_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int board_in(int r, int c)
{
    return r >= 0 && r < BOARD_N && c >= 0 && c < BOARD_N;
}

static inline int board_opponent(int side)
{
    return -side;
}

/* (r,c) 这格是不是 side 的子（越界按“不是”处理） */
static inline int board_is(chessboard (*b)[BOARD_N], int r, int c, int side)
{
    return board_in(r, c) && b[r][c].state == side;
}

/* (r,c) 落 side 后是否连成五子（只读，不改棋盘） */
static inline int board_has_five(chessboard (*b)[BOARD_N], int r, int c, int side)
{
    static const int dr[4] = {1, 0, 1, 1};
    static const int dc[4] = {0, 1, 1, -1};
    int k;

    for (k = 0; k < 4; ++k) {
        int cnt = 1;
        int s;
        for (s = -1; s <= 1; s += 2) {
            int rr = r + dr[k] * s;
            int cc = c + dc[k] * s;
            while (board_in(rr, cc) && b[rr][cc].state == side) {
                ++cnt;
                rr += dr[k] * s;
                cc += dc[k] * s;
            }
        }
        if (cnt >= 5) return 1;
    }
    return 0;
}

/* 生成候选点：已有棋子周围 CAND_RADIUS 格内的空点；空盘只给天元 */
static inline int board_gen_candidates(chessboard (*b)[BOARD_N],
                                       move_t *out, int maxn)
{
    int mark[BOARD_N][BOARD_N];
    int r, c, dr, dc, n = 0;
    int any = 0;

    if (maxn <= 0) return 0;

    for (r = 0; r < BOARD_N && !any; ++r)
        for (c = 0; c < BOARD_N && !any; ++c)
            if (b[r][c].state != EMPTY) any = 1;

    if (!any) {
        out[0].r = BOARD_N / 2;
        out[0].c = BOARD_N / 2;
        return 1;
    }

    for (r = 0; r < BOARD_N; ++r)
        for (c = 0; c < BOARD_N; ++c) mark[r][c] = 0;

    for (r = 0; r < BOARD_N; ++r) {
        for (c = 0; c < BOARD_N; ++c) {
            if (b[r][c].state == EMPTY) continue;
            for (dr = -CAND_RADIUS; dr <= CAND_RADIUS; ++dr) {
                for (dc = -CAND_RADIUS; dc <= CAND_RADIUS; ++dc) {
                    const int rr = r + dr;
                    const int cc = c + dc;
                    if (board_in(rr, cc) && b[rr][cc].state == EMPTY)
                        mark[rr][cc] = 1;
                }
            }
        }
    }

    for (r = 0; r < BOARD_N && n < maxn; ++r)
        for (c = 0; c < BOARD_N && n < maxn; ++c)
            if (mark[r][c]) {
                out[n].r = r;
                out[n].c = c;
                ++n;
            }

    return n;
}

/* 空走法 {-1,-1} */
static inline move_t move_none(void)
{
    move_t m;
    m.r = -1;
    m.c = -1;
    return m;
}

#ifdef __cplusplus
}
#endif

#endif /* FIAR_BOARD_H */
