/* ============================================================
 * ai/alphabeta.cpp —— α-β 剪枝（骨架，待你实现）
 *
 * 和 minimax 是同一套搜索，多两个参数：
 *   alpha：我方已经能保证的下限
 *   beta ：对方已经能保证的上限
 * 一旦 alpha >= beta，说明这一支再搜下去也不会被选，直接 break。
 *
 * 搜索里对"已经成五"的处理要和 minimax 一致，这样两边结果才可比，
 * 也才能用节点数说明剪枝的效果（g_nodes 计数保留）。
 * ============================================================ */
#include "alphabeta.h"

#include <climits>

#include "eval.h"

static long g_nodes = 0;

move_t alphabeta_best(int b[BOARD_N][BOARD_N], int side, int depth)
{
    (void)b;
    (void)side;
    (void)depth;
    g_nodes = 0;
    return move_none();    /* TODO */
}

long alphabeta_nodes(void)
{
    return g_nodes;
}
