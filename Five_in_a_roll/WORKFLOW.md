# FIVE IN A ROLL — 工程说明

> 当前状态：**只有界面 + 棋盘接口，没有算法**。
> AI 那一手由 `main.cpp` 里的 `ai_move()` 占位顶上（找第一个空点），
> 你写好算法后只改那个函数体即可。

---

## 1. 目录

```
Five_in_a_roll/
├── CMakeLists.txt            构建脚本
├── board.h                   ★ 棋盘类型 + 公共工具（结构体数组、五连判断、候选点）
├── include/ lib/             EasyX 26.9.25（MinGW-w64 x64 MSVCRT）
├── five_in_a_roll.orig.cpp   最早的笔记版，不参与编译
├── WORKFLOW.md               本文件
└── src/
    ├── view.h                画面接口（棋盘数组、坐标、绘制、事件）
    ├── view.cpp              EasyX 实现
    └── main.cpp              游戏流程：菜单 / 选边 / 落子 / 胜负 / 计时
```

## 2. 棋盘

**唯一数据源**：`view_cells`，类型就是 `chessboard[15][15]`。

```c
typedef struct Chessboard {
    int r;        /* 行 0~14 */
    int c;        /* 列 0~14 */
    int state;    /* 1 黑 / -1 白 / 0 空 */
} chessboard;

typedef chessboard Board[BOARD_N][BOARD_N];
extern Board view_cells;
```

落子就一行：

```c
view_cells[i][j].state = BLACK_STONE;
```

没有第二份数组，所以不存在“同步”问题。

## 3. 棋盘接口（board.h）

```c
int  board_in(int r, int c);                                   /* 是否在盘内 */
int  board_opponent(int side);                                 /* 取对手 */
int  board_is(chessboard (*b)[15], int r, int c, int s);       /* 这格是不是 s 的子 */
int  board_has_five(chessboard (*b)[15], int r, int c, int s); /* 落这手是否五连 */
int  board_gen_candidates(chessboard (*b)[15], move_t *out, int maxn); /* 候选点 */
move_t move_none(void);                                        /* 空走法 {-1,-1} */
```

函数一律用**指针**接棋盘：`chessboard (*b)[BOARD_N]`，调用时直接传 `view_cells`。

## 4. 画面接口（view.h）

| 分类 | 接口 |
|---|---|
| 棋盘 | `view_cells`（棋盘本体）、`view_clear_board()` |
| 显示状态 | `view_side`、`view_last_r/c`、`view_hover_r/c`、`view_pending_r/c`、`view_win_line[4]`、`view_clock_seconds`、`view_status`、`view_engine` |
| 生命周期 | `view_init()`、`view_shutdown()`、`view_running()`、`view_poll()` |
| 绘制 | `view_begin_frame()`、`view_end_frame()`、`view_draw_board()`、`view_draw_hud()` |
| 零件 | `view_draw_button()`、`view_fill_panel()`、`view_draw_stone()`、`view_set_font()`、`view_text()`、`view_text_center()`、`view_text_width()`、`view_set_text_color()` |
| 坐标 | `view_cell_to_pixel()`、`view_pixel_to_cell()`、`view_cell_name()` |

## 5. 加算法时怎么接

1. 新建 `ai/xxx.cpp` + `ai/xxx.h`，函数原型建议：
   ```c
   move_t xxx_best(chessboard (*b)[BOARD_N], int side, int depth);
   ```
2. 在 `CMakeLists.txt` 的 `FIAR_SOURCES` 里加 `ai/xxx.cpp`
3. 在 `main.cpp` 的 `ai_move()` 里调用它，替换掉占位落子那几行

`ai_move()` 现在是这样：

```c
void ai_move()
{
    move_t m = move_none();
    if (g_screen != SCR_PLAY || g_turn != g_ai) return;

    /* TODO: m = alphabeta_best(view_cells, g_ai, 3); */

    if (m.r < 0) { /* 占位：找第一个空点 */ }
    if (m.r >= 0) place(m.r, m.c, g_ai);
}
```

## 6. 构建

CLion 打开 `D:\01\C-Cpp-2026`，Toolchain = Bundled MinGW，Run `five_in_a_roll`。

命令行：

```powershell
cmake -S D:\01\C-Cpp-2026 -B build -G Ninja `
      -DCMAKE_C_COMPILER="D:\CLion 2026.2.2\bin\mingw\bin\gcc.exe" `
      -DCMAKE_CXX_COMPILER="D:\CLion 2026.2.2\bin\mingw\bin\g++.exe"
cmake --build build --target five_in_a_roll
```

## 7. 两个坑

1. **EasyX 的 `BLACK` / `WHITE` 是颜色宏**（0 / 0xFFFFFF）。棋子常量用 `BLACK_STONE` / `WHITE_STONE`。
2. **EasyX 的鼠标坐标取真实光标**，`PostMessage` 注入点击无效，界面验证要直接调函数。
