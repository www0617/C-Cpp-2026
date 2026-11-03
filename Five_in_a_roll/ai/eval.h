/* ============================================================
 * ai/eval.h —— 估值（C 接口）
 *
 *   eval_attack(b, r, c, side)  假设 side 下在 (r,c)，这一手的价值
 *   eval_board(b, side)         从 side 视角看整盘谁更有利
 *   eval_top_moves(...)         候选点按价值排序取前 n 个（搜索共用）
 * ============================================================ */
#ifndef FIAR_EVAL_H
#define FIAR_EVAL_H

#include "board.h"

#define SCORE_WIN 1000000

#ifdef __cplusplus
extern "C" {
#endif

int eval_attack(int b[BOARD_N][BOARD_N], int r, int c, int side);
int eval_board(int b[BOARD_N][BOARD_N], int side);
int eval_top_moves(int b[BOARD_N][BOARD_N], int side, move_t *out, int maxn);

#ifdef __cplusplus
}
#endif

#endif /* FIAR_EVAL_H */
