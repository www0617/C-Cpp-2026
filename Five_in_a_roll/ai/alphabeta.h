/* ============================================================
 * ai/alphabeta.h —— α-β 剪枝（C 接口，主力算法）
 * ============================================================ */
#ifndef FIAR_ALPHABETA_H
#define FIAR_ALPHABETA_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

move_t alphabeta_best(int b[BOARD_N][BOARD_N], int side, int depth);

/* 上次搜索展开的节点数（和 minimax_nodes() 对比用） */
long alphabeta_nodes(void);

#ifdef __cplusplus
}
#endif

#endif /* FIAR_ALPHABETA_H */
