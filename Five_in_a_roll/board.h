/* ============================================================
 * board.h —— 棋盘类型与公共工具（C 接口，src 和 ai 共用）
 *
 * 约定：黑 = 1，白 = -1，空 = 0；b[行][列]，行向下、列向右。
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

typedef int Board[BOARD_N][BOARD_N];

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

/* (r,c) 落 side 后是否连成五子 */
static inline int board_has_five(int b[BOARD_N][BOARD_N], int r, int c, int side)
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
            while (board_in(rr, cc) && b[rr][cc] == side) {
                ++cnt;
                rr += dr[k] * s;
                cc += dc[k] * s;
            }
        }
        if (cnt >= 5) return 1;
    }
    return 0;
}

/* 生成候选点，返回个数；空盘只给天元 */
static inline int board_gen_candidates(int b[BOARD_N][BOARD_N], move_t *out, int maxn)
{
    int mark[BOARD_N][BOARD_N];
    int r, c, dr, dc, n = 0;
    int any = 0;

    if (maxn <= 0) return 0;

    for (r = 0; r < BOARD_N && !any; ++r)
        for (c = 0; c < BOARD_N && !any; ++c)
            if (b[r][c] != EMPTY) any = 1;

    if (!any) {
        out[0].r = BOARD_N / 2;
        out[0].c = BOARD_N / 2;
        return 1;
    }

    for (r = 0; r < BOARD_N; ++r)
        for (c = 0; c < BOARD_N; ++c) mark[r][c] = 0;

    for (r = 0; r < BOARD_N; ++r) {
        for (c = 0; c < BOARD_N; ++c) {
            if (b[r][c] == EMPTY) continue;
            for (dr = -CAND_RADIUS; dr <= CAND_RADIUS; ++dr) {
                for (dc = -CAND_RADIUS; dc <= CAND_RADIUS; ++dc) {
                    const int rr = r + dr;
                    const int cc = c + dc;
                    if (board_in(rr, cc) && b[rr][cc] == EMPTY) mark[rr][cc] = 1;
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

/* 取"没有位置"的空 move */
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
