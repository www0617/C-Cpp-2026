/* ============================================================
 * ai/minimax.cpp —— 极小化极大（骨架，待你实现）
 *
 * 要做的事：
 *   1. 从当前局面开始，往后展开 depth 层；
 *   2. 轮到 side 的层取最大值，轮到对手的层取最小值；
 *   3. 叶子调用 eval_board() 打分；
 *   4. 某一手落下就成五，直接给 ±SCORE_WIN（并偏好更快赢）；
 *   5. 每层用 eval_top_moves() 只展开前 N 个候选（不然 15×15 跑不完）。
 *
 * 把展开的节点数累加进 g_nodes，minimax_nodes() 用来和 alpha-beta
 * 对比"剪枝到底省了多少"。
 * ============================================================ */
#include "minimax.h"

#include "eval.h"

static long g_nodes = 0;

move_t minimax_best(int b[BOARD_N][BOARD_N], int side, int depth)
{
    (void)b;
    (void)side;
    (void)depth;
    g_nodes = 0;
    return move_none();    /* TODO */
}

long minimax_nodes(void)
{
    return g_nodes;
}
