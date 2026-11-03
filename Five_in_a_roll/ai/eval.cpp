/* ============================================================
 * ai/eval.cpp —— 估值（骨架，待你实现）
 *
 * 这里是你要填的两个函数：
 *
 *   eval_attack(b, r, c, side)
 *       假设 side 落在 (r,c)，这一手值多少分。
 *       常见做法：四个方向各数一段"连续几子 + 两头是否为空"，
 *       成五 > 活四 > 冲四 > 活三 > 眠三 > 活二 > 眠二。
 *       注意分值要有量级差，不然搜索分不清"马上赢"和"看着不错"。
 *
 *   eval_board(b, side)
 *       整盘从 side 视角的分数，正数对 side 有利。
 *       最省事：把所有候选点的 eval_attack 加起来相减；
 *       注意累加值不要超过 SCORE_WIN，否则"一步连五"会被双四压过去。
 *
 *   eval_top_moves(b, side, out, maxn)
 *       把候选点按"进攻分 + 防守分"排序，取前 maxn 个。
 *       minimax / alphabeta 每层都靠它收窄分支，不排序就跑不动。
 * ============================================================ */
#include "eval.h"

int eval_attack(int b[BOARD_N][BOARD_N], int r, int c, int side)
{
    (void)b;
    (void)r;
    (void)c;
    (void)side;
    return 0;              /* TODO */
}

int eval_board(int b[BOARD_N][BOARD_N], int side)
{
    (void)b;
    (void)side;
    return 0;              /* TODO */
}

int eval_top_moves(int b[BOARD_N][BOARD_N], int side, move_t *out, int maxn)
{
    /* 骨架版先退回"候选点原样取前 maxn 个"，保证搜索能跑起来。
       你写完 eval_attack 之后，把这里换成按分数排序。 */
    (void)side;
    return board_gen_candidates(b, out, maxn);
}
