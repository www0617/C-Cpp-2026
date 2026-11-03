/* ============================================================
 * ai/mcts.h —— 蒙特卡洛树搜索 UCT（C 接口）
 * ============================================================ */
#ifndef FIAR_MCTS_H
#define FIAR_MCTS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* iterations 越大越强、越慢；界面上 600~2000 比较合适 */
move_t mcts_best(int b[BOARD_N][BOARD_N], int side, int iterations);

#ifdef __cplusplus
}
#endif

#endif /* FIAR_MCTS_H */
