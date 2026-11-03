/* ============================================================
 * src/view.cpp —— EasyX 绘制 + 坐标换算 + 消息轮询
 * 对外是 C 接口（见 view.h），内部是 C++（EasyX 只能用 C++）。
 * 不判胜负、不管回合，只把 view_board 里的东西画出来。
 * ============================================================ */
#include "view.h"

#include <graphics.h>

#include <cstdio>
#include <cwchar>

namespace {

const int SCALE = 2;                 /* 逻辑 800x480 -> 物理 1600x960 */
const int LOGIC_W = 800;
const int LOGIC_H = 480;
const int PHYS_W = LOGIC_W * SCALE;
const int PHYS_H = LOGIC_H * SCALE;

/* 极客配色 */
const COLORREF C_BG = RGB(5, 8, 10);
const COLORREF C_PANEL = RGB(8, 16, 18);
const COLORREF C_GRID = RGB(0, 140, 108);
const COLORREF C_GRID_D = RGB(0, 45, 36);
const COLORREF C_ACCENT = VIEW_CLR_ACCENT;
const COLORREF C_CYAN = VIEW_CLR_CYAN;
const COLORREF C_TEXT = VIEW_CLR_TEXT;
const COLORREF C_DIM = VIEW_CLR_DIM;
const COLORREF C_RED = VIEW_CLR_RED;

const int GHOST_PX = (12 * 2 + 6) * SCALE;
IMAGE* g_ghost[3] = {nullptr, nullptr, nullptr};

HWND g_hwnd = nullptr;
int g_running = 0;

int px(int col) { return VIEW_BOARD_X + col * VIEW_CELL; }
int py(int row) { return VIEW_BOARD_Y + row * VIEW_CELL; }

void set_font_raw(int h, int bold)
{
    settextstyle(h, 0, L"Consolas", 0, 0,
                 bold ? FW_BOLD : FW_NORMAL, false, false, false);
}

void text_center_raw(int cx, int y, const wchar_t* s)
{
    outtextxy(cx - textwidth(s) / 2, y, s);
}

COLORREF mix_color(COLORREF a, COLORREF b, double t)
{
    const int r = (int)(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t);
    const int g = (int)(GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t);
    const int bl = (int)(GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t);
    return RGB(r, g, bl);
}

void draw_stone_r(int x, int y, int s, int r)
{
    const bool black = (s == BLACK_STONE);

    /* 外圈弱光 */
    setlinecolor(black ? RGB(0, 70, 56) : RGB(0, 110, 95));
    setlinestyle(PS_SOLID, 1);
    circle(x, y, r + 2);

    const COLORREF base = black ? RGB(30, 40, 42) : RGB(214, 232, 227);
    const COLORREF hi = black ? RGB(80, 110, 100) : RGB(255, 255, 255);

    setfillcolor(base);
    solidcircle(x, y, r);

    /* 朝左上的高光层：暗玻璃珠 / 白瓷 */
    for (int i = 1; i <= 6; ++i) {
        const double t = i / 6.0;
        const int rr = (int)(r * (1.0 - 0.55 * t));
        const int ox = (int)(-r * 0.28 * t);
        const int oy = (int)(-r * 0.28 * t);
        setfillcolor(mix_color(base, hi, t * 0.75));
        solidcircle(x + ox, y + oy, rr);
    }

    setfillcolor(black ? RGB(130, 160, 150) : RGB(255, 255, 255));
    solidcircle(x - r / 3, y - r / 3, r / 5);

    setlinecolor(black ? C_GRID : C_CYAN);
    circle(x, y, r);
}

void build_ghost_images()
{
    for (int s = BLACK_STONE; s <= WHITE_STONE; s += 2) {
        IMAGE* img = new IMAGE(GHOST_PX, GHOST_PX);

        SetWorkingImage(img);
        setaspectratio(1, 1);
        setbkcolor(C_PANEL);
        cleardevice();

        draw_stone_r(GHOST_PX / 2, GHOST_PX / 2, s, 12 * SCALE);

        SetWorkingImage(nullptr);
        setaspectratio(SCALE, SCALE);
        g_ghost[s == BLACK_STONE ? 1 : 2] = img;
    }
    setbkcolor(C_BG);
}

void draw_ghost(int x, int y, int s)
{
    const int idx = (s == BLACK_STONE) ? 1 : 2;
    if (!g_ghost[idx]) return;

    HDC dst = GetImageHDC(nullptr);
    HDC src = GetImageHDC(g_ghost[idx]);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 120;
    bf.AlphaFormat = 0;

    /* GDI 不认 EasyX 的 setaspectratio，这里临时切 1:1 用物理坐标 */
    setaspectratio(1, 1);
    GdiAlphaBlend(dst, x * SCALE - GHOST_PX / 2, y * SCALE - GHOST_PX / 2,
                  GHOST_PX, GHOST_PX, src, 0, 0, GHOST_PX, GHOST_PX, bf);
    setaspectratio(SCALE, SCALE);

    setlinecolor(C_ACCENT);
    setlinestyle(PS_DOT, 1);
    circle(x, y, 14);
    setlinestyle(PS_SOLID, 1);
}

void panel_frame(int l, int t, int r, int b)
{
    setlinecolor(RGB(0, 80, 64));
    setlinestyle(PS_SOLID, 1);
    rectangle(l, t, r, b);

    setlinecolor(C_ACCENT);
    setlinestyle(PS_SOLID, 2);
    {
        const int L = 10;
        line(l, t, l + L, t);     line(l, t, l, t + L);
        line(r - L, t, r, t);     line(r, t, r, t + L);
        line(l, b - L, l, b);     line(l, b, l + L, b);
        line(r, b - L, r, b);     line(r - L, b, r, b);
    }
    setlinestyle(PS_SOLID, 1);
}

void draw_coords()
{
    set_font_raw(9, 0);
    settextcolor(C_DIM);

    wchar_t b[8];
    for (int c = 0; c < BOARD_N; ++c) {
        b[0] = (wchar_t)(L'A' + c);
        b[1] = 0;
        text_center_raw(px(c), 13, b);
    }
    for (int r = 0; r < BOARD_N; ++r) {
        swprintf(b, 8, L"%02d", r + 1);
        outtextxy(47 - textwidth(b), py(r) - textheight(b) / 2, b);
    }
}

}  // namespace

/* ---------------- 状态 ---------------- */
Board view_board = {{0}};
int view_side = BLACK_STONE;
int view_last_r = -1, view_last_c = -1;
int view_hover_r = -1, view_hover_c = -1;
int view_pending_r = -1, view_pending_c = -1;
int view_win_line[4] = {-1, -1, -1, -1};
int view_clock_seconds = 0;
const wchar_t* view_status = L"";
const wchar_t* view_engine = L"";

/* ---------------- 坐标 ---------------- */
int view_cell_to_pixel(int r, int c, int* x, int* y)
{
    if (!board_in(r, c)) return 0;
    if (x) *x = px(c);
    if (y) *y = py(r);
    return 1;
}

int view_pixel_to_cell(int mx, int my, int* r, int* c)
{
    const int col = (mx - VIEW_BOARD_X + VIEW_CELL / 2) / VIEW_CELL;
    const int row = (my - VIEW_BOARD_Y + VIEW_CELL / 2) / VIEW_CELL;
    if (!board_in(row, col)) return 0;

    const int dx = mx - px(col);
    const int dy = my - py(row);
    if (dx < 0 ? -dx > VIEW_CELL / 2 - 3 : dx > VIEW_CELL / 2 - 3) return 0;
    if (dy < 0 ? -dy > VIEW_CELL / 2 - 3 : dy > VIEW_CELL / 2 - 3) return 0;

    if (r) *r = row;
    if (c) *c = col;
    return 1;
}

void view_cell_name(int r, int c, wchar_t* buf, int n)
{
    if (!buf || n <= 0) return;
    if (!board_in(r, c)) {
        wcsncpy(buf, L"--", n);
        buf[n - 1] = 0;
        return;
    }
    buf[0] = (wchar_t)(L'A' + c);
    const int v = r + 1;
    buf[1] = (wchar_t)(L'0' + v / 10);
    buf[2] = (wchar_t)(L'0' + v % 10);
    buf[3] = 0;
}

/* ---------------- 生命周期 ---------------- */
int view_init(void)
{
    SetProcessDPIAware();

    g_hwnd = initgraph(PHYS_W, PHYS_H);
    if (!g_hwnd) return 0;

    SetWindowTextW(g_hwnd, L"FIVE IN A ROLL // GOMOKU");

    RECT rc;
    GetWindowRect(g_hwnd, &rc);
    const int w = rc.right - rc.left, h = rc.bottom - rc.top;
    SetWindowPos(g_hwnd, nullptr,
                 (GetSystemMetrics(SM_CXSCREEN) - w) / 2,
                 (GetSystemMetrics(SM_CYSCREEN) - h) / 2,
                 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    setaspectratio(SCALE, SCALE);
    setbkcolor(C_BG);
    setbkmode(TRANSPARENT);

    build_ghost_images();
    BeginBatchDraw();

    g_running = 1;
    return 1;
}

void view_shutdown(void)
{
    EndBatchDraw();
    closegraph();
    g_running = 0;
}

int view_running(void)
{
    if (!g_running) return 0;
    if (!IsWindow(g_hwnd)) g_running = 0;
    return g_running;
}

int view_poll(view_event_t* ev)
{
    if (!g_hwnd) return 0;

    ExMessage msg;
    while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
        ev->kind = VIEW_EV_NONE;
        ev->r = ev->c = -1;
        ev->key = 0;

        if (msg.message == WM_CLOSE) {
            ev->kind = VIEW_EV_CLOSE;
            g_running = 0;
            return 1;
        }
        if (msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONDOWN) {
            ev->x = msg.x / SCALE;
            ev->y = msg.y / SCALE;
            view_pixel_to_cell(ev->x, ev->y, &ev->r, &ev->c);
            ev->kind = (msg.message == WM_LBUTTONDOWN) ? VIEW_EV_CLICK : VIEW_EV_MOVE;
            return 1;
        }
        if (msg.message == WM_KEYDOWN) {
            ev->kind = VIEW_EV_KEY;
            ev->key = msg.vkcode;
            return 1;
        }
    }
    return 0;
}

/* ---------------- 绘制 ---------------- */
void view_set_font(int h, int bold) { set_font_raw(h, bold); }

void view_text_center(int cx, int y, const wchar_t* s) { text_center_raw(cx, y, s); }

void view_text(int x, int y, const wchar_t* s) { outtextxy(x, y, s); }

int view_text_width(const wchar_t* s) { return textwidth(s); }

void view_set_text_color(COLORREF c) { settextcolor(c); }

void view_draw_stone(int x, int y, int side, int radius)
{
    draw_stone_r(x, y, side, radius);
}

void view_fill_panel(int l, int t, int r, int b)
{
    setfillcolor(C_PANEL);
    solidrectangle(l, t, r, b);
    panel_frame(l, t, r, b);
}

void view_begin_frame(void)
{
    cleardevice();

    /* 背景网格 */
    setlinecolor(RGB(8, 20, 17));
    setlinestyle(PS_SOLID, 1);
    for (int x = 0; x < LOGIC_W; x += 20) line(x, 0, x, LOGIC_H);
    for (int y = 0; y < LOGIC_H; y += 20) line(0, y, LOGIC_W, y);
}

void view_end_frame(void)
{
    FlushBatchDraw();
}

void view_draw_board(void)
{
    setfillcolor(C_PANEL);
    solidrectangle(34, 8, 480, 462);

    /* 网格弱光 */
    setlinecolor(C_GRID_D);
    setlinestyle(PS_SOLID, 3);
    for (int i = 0; i < BOARD_N; ++i) {
        line(px(0), py(i), px(BOARD_N - 1), py(i));
        line(px(i), py(0), px(i), py(BOARD_N - 1));
    }

    /* 网格 */
    setlinecolor(C_GRID);
    setlinestyle(PS_SOLID, 1);
    for (int i = 0; i < BOARD_N; ++i) {
        line(px(0), py(i), px(BOARD_N - 1), py(i));
        line(px(i), py(0), px(i), py(BOARD_N - 1));
    }

    /* 外框 */
    setlinecolor(RGB(0, 210, 160));
    rectangle(px(0), py(0), px(BOARD_N - 1), py(BOARD_N - 1));

    /* 星位 */
    const int star[5][2] = {{3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}};
    for (int i = 0; i < 5; ++i) {
        setlinecolor(RGB(0, 80, 64));
        circle(px(star[i][1]), py(star[i][0]), 5);
        setfillcolor(C_CYAN);
        solidcircle(px(star[i][1]), py(star[i][0]), 3);
    }

    draw_coords();

    /* 棋子 */
    for (int r = 0; r < BOARD_N; ++r)
        for (int c = 0; c < BOARD_N; ++c)
            if (view_board[r][c] != EMPTY)
                draw_stone_r(px(c), py(r), view_board[r][c], 12);

    /* 悬停 / 待确认的半透明棋子 */
    if (view_pending_r >= 0)
        draw_ghost(px(view_pending_c), py(view_pending_r), view_side);
    else if (view_hover_r >= 0)
        draw_ghost(px(view_hover_c), py(view_hover_r), view_side);

    /* 最后一手 */
    if (view_last_r >= 0) {
        setlinecolor(C_CYAN);
        setlinestyle(PS_SOLID, 1);
        circle(px(view_last_c), py(view_last_r), 16);
        setfillcolor(C_CYAN);
        solidcircle(px(view_last_c), py(view_last_r), 2);
    }

    /* 连五 */
    if (view_win_line[0] >= 0) {
        const int x1 = px(view_win_line[1]), y1 = py(view_win_line[0]);
        const int x2 = px(view_win_line[3]), y2 = py(view_win_line[2]);
        setlinecolor(RGB(0, 80, 60));
        setlinestyle(PS_SOLID, 6);
        line(x1, y1, x2, y2);
        setlinecolor(C_ACCENT);
        setlinestyle(PS_SOLID, 2);
        line(x1, y1, x2, y2);
        setlinestyle(PS_SOLID, 1);
    }

    /* 棋盘外框 */
    panel_frame(34, 8, 480, 462);
}

void view_draw_hud(void)
{
    panel_frame(495, 18, 792, 462);

    set_font_raw(18, 1);
    settextcolor(C_ACCENT);
    outtextxy(510, 28, L"FIVE IN A ROLL");

    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 52, L"GOMOKU // 15x15 // HUMAN vs AI");

    setlinecolor(RGB(0, 80, 64));
    line(510, 72, 778, 72);

    /* 当前执子 */
    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 86, L"TURN");
    draw_stone_r(520, 112, view_side, 7);

    set_font_raw(18, 1);
    settextcolor(C_TEXT);
    outtextxy(536, 100, view_side == BLACK_STONE ? L"BLACK" : L"WHITE");

    /* 计时 */
    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 146, L"TIME");

    wchar_t tb[16];
    swprintf(tb, 16, L"%02d:%02d", view_clock_seconds / 60, view_clock_seconds % 60);
    set_font_raw(32, 1);
    settextcolor(C_ACCENT);
    outtextxy(510, 158, tb);

    setlinecolor(RGB(0, 80, 64));
    line(510, 204, 778, 204);

    /* 手数 / 最后一手 / 光标 */
    int moves = 0;
    for (int r = 0; r < BOARD_N; ++r)
        for (int c = 0; c < BOARD_N; ++c)
            if (view_board[r][c] != EMPTY) ++moves;

    wchar_t b[16];

    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 216, L"MOVES");
    swprintf(b, 16, L"%03d", moves);
    set_font_raw(14, 0);
    settextcolor(C_TEXT);
    outtextxy(778 - textwidth(b), 212, b);

    view_cell_name(view_last_r, view_last_c, b, 16);
    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 238, L"LAST");
    set_font_raw(14, 0);
    settextcolor(C_TEXT);
    outtextxy(778 - textwidth(b), 234, b);

    {
        const int cr = (view_pending_r >= 0) ? view_pending_r : view_hover_r;
        const int cc = (view_pending_c >= 0) ? view_pending_c : view_hover_c;
        view_cell_name(cr, cc, b, 16);
    }
    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 260, L"CURSOR");
    set_font_raw(14, 0);
    settextcolor(C_TEXT);
    outtextxy(778 - textwidth(b), 256, b);

    setlinecolor(RGB(0, 80, 64));
    line(510, 286, 778, 286);

    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 296, L"STATUS");

    set_font_raw(12, 1);
    settextcolor(C_TEXT);
    outtextxy(510, 312, view_status ? view_status : L"");

    if (view_engine && view_engine[0]) {
        set_font_raw(9, 0);
        settextcolor(C_DIM);
        outtextxy(510, 344, L"AI");
        set_font_raw(11, 1);
        settextcolor(C_CYAN);
        outtextxy(510, 356, view_engine);
    }

    set_font_raw(9, 0);
    settextcolor(C_DIM);
    outtextxy(510, 418, L"LMB  SELECT / CONFIRM");
    outtextxy(510, 436, L"R  RESTART      ESC  MENU      Q  QUIT");

    set_font_raw(8, 0);
    settextcolor(RGB(35, 75, 65));
    outtextxy(700, 444, L"BUILD 0.3");
}

void view_draw_button(int x, int y, int w, int h, const wchar_t* label, int hot)
{
    setfillcolor(hot ? RGB(8, 42, 35) : C_PANEL);
    solidrectangle(x, y, x + w, y + h);

    setlinecolor(hot ? C_ACCENT : RGB(0, 90, 72));
    setlinestyle(PS_SOLID, 1);
    rectangle(x, y, x + w, y + h);

    setlinecolor(hot ? C_ACCENT : RGB(0, 70, 56));
    setlinestyle(PS_SOLID, 2);
    {
        const int L = 8;
        line(x, y, x + L, y);         line(x, y, x, y + L);
        line(x + w - L, y, x + w, y); line(x + w, y, x + w, y + L);
        line(x, y + h - L, x, y + h); line(x, y + h, x + L, y + h);
        line(x + w, y + h - L, x + w, y + h);
        line(x + w - L, y + h, x + w, y + h);
    }
    setlinestyle(PS_SOLID, 1);

    set_font_raw(16, hot);
    settextcolor(hot ? C_ACCENT : C_TEXT);
    text_center_raw(x + w / 2, y + (h - textheight(label)) / 2, label);
}

