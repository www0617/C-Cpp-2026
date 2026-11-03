/* ============================================================
 * ai/minimax.h —— 不做剪枝的极小化极大（C 接口，对照用）
 * ============================================================ */
#ifndef FIAR_MINIMAX_H
#define FIAR_MINIMAX_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 返回 side 要下的一手；depth 为往后看几层（含自己这一层） */
move_t minimax_best(int b[BOARD_N][BOARD_N], int side, int depth);

/* 上次搜索展开的节点数（和 alphabeta_nodes() 对比用） */
long minimax_nodes(void);

#ifdef __cplusplus
}
#endif

#endif /* FIAR_MINIMAX_H */
