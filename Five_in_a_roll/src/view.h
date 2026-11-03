/* ============================================================
 * src/view.h —— 绘图层对外接口（C 语法，C/C++ 都能 include）
 *
 * 只做三件事：
 *   1) 暴露棋盘二维数组：view_board[r][c] = 1 黑 / -1 白 / 0 空
 *   2) 坐标换算：行列 <-> 像素，行列 <-> "H8"
 *   3) 把这些画出来
 *
 * 规则、回合、胜负、AI 都不在这里——写在 main.c 里。
 * ============================================================ */
#ifndef FIAR_VIEW_H
#define FIAR_VIEW_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601          /* GdiAlphaBlend / GetTickCount64 */
#endif

#include <windows.h>

#include "board.h"

/* 棋盘几何 */
#define VIEW_BOARD_X 50
#define VIEW_BOARD_Y 30
#define VIEW_CELL 30

/* 配色（名字带 VIEW_CLR_，避免和 EasyX 的 RED/BLUE 等颜色宏撞车） */
#define VIEW_CLR_ACCENT RGB(0, 255, 170)
#define VIEW_CLR_CYAN RGB(0, 210, 255)
#define VIEW_CLR_TEXT RGB(190, 255, 235)
#define VIEW_CLR_DIM RGB(70, 130, 115)
#define VIEW_CLR_RED RGB(255, 90, 90)

/* view_poll 返回的事件类型 */
#define VIEW_EV_NONE 0
#define VIEW_EV_MOVE 1
#define VIEW_EV_CLICK 2
#define VIEW_EV_KEY 3
#define VIEW_EV_CLOSE 4

typedef struct view_event_s {
    int kind;
    int x;      /* 逻辑坐标 */
    int y;
    int r;      /* 命中的交叉点，-1 = 没命中 */
    int c;
    int key;    /* 虚拟键码 */
} view_event_t;

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- 棋盘状态（绘图直接读） ---------------- */
extern Board view_board;           /* 1 黑 / -1 白 / 0 空 */
extern int view_side;
extern int view_last_r, view_last_c;
extern int view_hover_r, view_hover_c;
extern int view_pending_r, view_pending_c;
extern int view_win_line[4];
extern int view_clock_seconds;
extern const wchar_t *view_status;
extern const wchar_t *view_engine;

/* ---------------- 生命周期 ---------------- */
int  view_init(void);
void view_shutdown(void);
int  view_running(void);
int  view_poll(view_event_t *ev);

/* ---------------- 绘制 ---------------- */
void view_begin_frame(void);
void view_end_frame(void);
void view_draw_board(void);
void view_draw_hud(void);
void view_draw_button(int x, int y, int w, int h, const wchar_t *label, int hot);
void view_fill_panel(int l, int t, int r, int b);
void view_draw_stone(int x, int y, int side, int radius);
void view_set_font(int h, int bold);
void view_text(int x, int y, const wchar_t *s);
void view_text_center(int cx, int y, const wchar_t *s);
int  view_text_width(const wchar_t *s);
void view_set_text_color(COLORREF c);

/* ---------------- 坐标 ---------------- */
int  view_cell_to_pixel(int r, int c, int *x, int *y);
int  view_pixel_to_cell(int mx, int my, int *r, int *c);
void view_cell_name(int r, int c, wchar_t *buf, int n);

#ifdef __cplusplus
}
#endif

#endif /* FIAR_VIEW_H */
