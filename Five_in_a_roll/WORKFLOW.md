# FIVE IN A ROLL — 工程结构与工作流

> 分工：`src/` 只管画和坐标，`ai/` 只留接口（算法你自己写），`main` 负责组装。
> 棋盘约定：**黑 `1` / 白 `-1` / 空 `0`**，`board[行][列]`，行向下、列向右。
> 头文件全是 C 语法，可以直接被 `gcc -std=c11` 的 `.c` 文件 include；
> 实现文件仍是 `.cpp`（EasyX 只能 C++）。

---

## 1. 目录

```
Five_in_a_roll/
├── CMakeLists.txt
├── WORKFLOW.md
├── board.h                   ★ C 接口：BOARD_N、1/-1/0、move_t、board_* 工具
├── include/ lib/             EasyX 26.9.25（MinGW-w64 x64 MSVCRT）
├── five_in_a_roll.orig.cpp   最早的 API 笔记版，不参与编译
├── src/
│   ├── view.h                ★ C 接口：view_board + 坐标 + 绘制 + 事件
│   ├── view.cpp              实现（C++，包 EasyX）
│   └── main.cpp              组装：回合、落子、胜负、计时、菜单
└── ai/
    ├── eval.h                eval_attack / eval_board / eval_top_moves
    ├── eval.cpp              ← 骨架，待实现
    ├── minimax.h / .cpp      minimax_best / minimax_nodes（骨架）
    ├── alphabeta.h / .cpp    alphabeta_best / alphabeta_nodes（骨架）
    └── mcts.h / .cpp         mcts_best（骨架）
```

**算法文件里只有函数壳和注释**，每个函数返回 `move_none()` / `0`，那就是留给你写的位置。
`main.cpp` 里有个 `fallback_move()` 占位（AI 没返回走法时随便下个空点），
你的算法写好后就再也走不到它，可以删掉。

## 2. 接口速查（C 语法）

```c
#include "board.h"
#include "view.h"
#include "ai/alphabeta.h"

Board b;                     /* int b[15][15] */
move_t m;                    /* struct { int r, c; }，r/c = -1 表示没有 */

m = alphabeta_best(b, BLACK_STONE, 3);

view_board[r][c] = WHITE_STONE;      /* 绘图直接读这块数组 */
view_cell_name(7, 7, buf, 16);       /* -> L"H08" */
view_pixel_to_cell(mx, my, &r, &c);  /* 像素 -> 行列，不命中返回 0 */
```

| 接口 | 说明 |
|---|---|
| `BOARD_N` / `BLACK_STONE` / `WHITE_STONE` / `EMPTY` | 15 / 1 / -1 / 0 |
| `move_t { int r, c; }` | 一步棋 |
| `board_in(r,c)` | 是否在盘内 |
| `board_opponent(side)` | 取反（1 ↔ -1） |
| `board_has_five(b,r,c,side)` | 落这手后是否连五 |
| `board_gen_candidates(b,out,maxn)` | 候选点（棋子周围 2 格），返回个数 |
| `move_none()` | 空 move |
| `view_board` / `view_side` / `view_last_r,c` / `view_win_line` / `view_clock_seconds` / `view_status` / `view_engine` | 画面状态 |
| `view_init/shutdown/running/poll` | 窗口与事件 |
| `view_begin_frame/end_frame/draw_board/draw_hud` | 一帧 |
| `view_draw_button/fill_panel/draw_stone/set_font/text/text_center/set_text_color` | 菜单零件 |
| `VIEW_CLR_ACCENT` 等 | 配色宏 |

`main` 的典型循环：

```c
view_init();
while (view_running()) {
    view_event_t ev;
    while (view_poll(&ev)) {
        if (ev.kind == VIEW_EV_CLICK && ev.r >= 0)
            view_board[ev.r][ev.c] = BLACK_STONE;
    }
    view_begin_frame();
    view_draw_board();
    view_draw_hud();
    view_end_frame();
    Sleep(10);
}
view_shutdown();
```

## 3. 三个算法重复吗

| 模块 | 内容 | 关系 |
|---|---|---|
| `eval` | 只算分，不搜索 | minimax / αβ 都调它，估值只这一份 |
| `minimax` | 全部走完的搜索 | 与 αβ 结果相同、慢 |
| `alphabeta` | 带 α/β 窗口的同一套搜索 | **αβ = minimax 加剪枝** |
| `mcts` | 随机模拟 + UCT | 另一套，只用 `board_has_five` |

候选点、胜负、棋盘工具都已在 `board.h` 里（`static inline`），不用重复写。
minimax 和 αβ 的差别只在递归里那几行，两边都调 `eval_board` / `eval_top_moves`。

## 4. 待实现清单

| 函数 | 要做的事 |
|---|---|
| `eval_attack(b,r,c,side)` | 单点棋型分：成五 / 活四 / 冲四 / 活三 / 眠三 / 活二 / 眠二 |
| `eval_board(b,side)` | 局面分，正数对 side 有利；累加值别超过 `SCORE_WIN` |
| `eval_top_moves(b,side,out,maxn)` | 候选点按"攻 + 守"排序取前 n 个（骨架版是原样取前 n 个） |
| `minimax_best` | depth 层展开，max/min 交替，叶子用 `eval_board` |
| `alphabeta_best` | 同上 + alpha/beta 窗口，`alpha >= beta` 就 break |
| `mcts_best` | 选择/扩展/模拟/回传，UCT 取 visit 最多的孩子 |

`minimax_nodes()` / `alphabeta_nodes()` 记得在搜索里累加，写报告时能对比剪枝效果。

## 5. 换算法 / 调参

`src/main.cpp` 的 `ai_move()` 只有一行要改：

```c
m = alphabeta_best(view_board, g_ai, AI_DEPTH);
/* m = minimax_best(view_board, g_ai, AI_DEPTH); */
/* m = mcts_best(view_board, g_ai, MCTS_ITERS);    */
```

参数在文件顶部：`AI_DEPTH = 3`、`MCTS_ITERS = 800`。

## 6. 构建与验证

CLion 打开 `D:\01\C-Cpp-2026`，Toolchain = Bundled MinGW，Run `five_in_a_roll`。

```powershell
cmake -S D:\01\C-Cpp-2026 -B build -G Ninja `
      -DCMAKE_C_COMPILER="D:\CLion 2026.2.2\bin\mingw\bin\gcc.exe" `
      -DCMAKE_CXX_COMPILER="D:\CLion 2026.2.2\bin\mingw\bin\g++.exe"
cmake --build build --target five_in_a_roll
```

| 工具 | 作用 |
|---|---|
| `D:\dshfiveinaroll\selftest\c_api_probe.c` | 纯 C 编译验证（`gcc -std=c11` 已通过） |
| `D:\dshfiveinaroll\selftest\ai_selftest.cpp` | 算法验收：合法性 / 必胜 / 必堵 / 耗时 / 节点数，**现在是 FAIL，写完就 PASS** |
| `D:\dshfiveinaroll\selftest\harness.cpp` | 直接调 `view_*` 导出一张 PNG |

## 7. 两个坑（踩过了）

1. **EasyX 的 `BLACK`/`WHITE` 是颜色宏**（0 / 0xFFFFFF），棋子常量叫 `BLACK_STONE` / `WHITE_STONE`。
2. **EasyX 的鼠标坐标取真实光标**，`PostMessage` 注入点击无效，界面验证走 `harness.cpp`。
