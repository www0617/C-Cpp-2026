// ============================================================
// src/main.cpp —— 把 view（画面）和 ai（算法）拼起来
//
// 这一版仍然用 .cpp 编译，但里面只用 C 接口（view.h / board.h / ai/*.h），
// 所以你把它改名成 main.c、按 C 语法写，一样能编。
//
// 这里才是"游戏"：回合、落子确认、胜负、计时、菜单/选边/结束页。
// 换算法只改 ai_move() 里那一行。
// ============================================================
#include <cstdio>
#include <cstring>

#include "ai/alphabeta.h"
#include "ai/mcts.h"
#include "ai/minimax.h"
#include "board.h"
#include "view.h"

namespace {

/* ---------------- 可调参数 ---------------- */
const int AI_DEPTH = 3;        /* α-β / minimax 的搜索深度 */
const int MCTS_ITERS = 800;    /* 换成 MCTS 时的模拟次数 */

enum Screen { SCR_MENU, SCR_SIDE, SCR_PLAY, SCR_OVER };
enum { NO_WIN = 0, TIE = 2 };

Screen g_screen = SCR_MENU;
int g_human = BLACK_STONE;
int g_ai = WHITE_STONE;
int g_turn = BLACK_STONE;
int g_winner = NO_WIN;
int g_pending_r = -1, g_pending_c = -1;
int g_hover_r = -1, g_hover_c = -1;
int g_mx = -1, g_my = -1;      /* 鼠标逻辑坐标（菜单高亮用） */
int g_quit = 0;

ULONGLONG g_t0 = 0, g_t1 = 0;
int g_timing = 0;
int g_sec = 0;

/* ---------------- 工具 ---------------- */
int stone_count()
{
    int n = 0;
    for (int r = 0; r < BOARD_N; ++r)
        for (int c = 0; c < BOARD_N; ++c)
            if (view_board[r][c] != EMPTY) ++n;
    return n;
}

int in_rect(int x, int y, int l, int t, int w, int h)
{
    return x >= l && x < l + w && y >= t && y < t + h;
}

/* 以 (r,c) 为落点的五连首尾 */
int find_win_line(int b[BOARD_N][BOARD_N], int r, int c, int s, int line[4])
{
    static const int dr[4] = {1, 0, 1, 1};
    static const int dc[4] = {0, 1, 1, -1};

    for (int k = 0; k < 4; ++k) {
        int cnt = 1;

        int rr = r - dr[k], cc = c - dc[k];
        while (board_in(rr, cc) && b[rr][cc] == s) {
            ++cnt;
            rr -= dr[k];
            cc -= dc[k];
        }
        const int r1 = rr + dr[k], c1 = cc + dc[k];

        rr = r + dr[k];
        cc = c + dc[k];
        while (board_in(rr, cc) && b[rr][cc] == s) {
            ++cnt;
            rr += dr[k];
            cc += dc[k];
        }
        const int r2 = rr - dr[k], c2 = cc - dc[k];

        if (cnt >= 5) {
            line[0] = r1; line[1] = c1;
            line[2] = r2; line[3] = c2;
            return 1;
        }
    }
    return 0;
}

void reset_all()
{
    memset(view_board, 0, sizeof(view_board));
    view_last_r = view_last_c = -1;
    view_hover_r = view_hover_c = -1;
    view_pending_r = view_pending_c = -1;
    view_win_line[0] = view_win_line[1] = -1;
    view_win_line[2] = view_win_line[3] = -1;
    view_clock_seconds = 0;

    g_pending_r = g_pending_c = -1;
    g_hover_r = g_hover_c = -1;
    g_winner = NO_WIN;
    g_turn = BLACK_STONE;
    g_t0 = g_t1 = 0;
    g_timing = 0;
    g_sec = 0;
}

void start_game(int human)
{
    reset_all();
    g_human = human;
    g_ai = -human;
    g_screen = SCR_PLAY;
    g_turn = BLACK_STONE;          /* 黑先 */

    view_side = g_turn;
    view_status = (g_turn == g_human) ? L"YOUR TURN" : L"AI THINKING...";
}

/* 落子：唯一入口（人和 AI 都走这里） */
int place(int r, int c, int s)
{
    if (!board_in(r, c)) return 0;
    if (view_board[r][c] != EMPTY) return 0;
    if (s != g_turn) return 0;

    view_board[r][c] = s;
    view_last_r = r;
    view_last_c = c;

    if (!g_timing) {
        g_t0 = GetTickCount64();
        g_timing = 1;
    }

    int line[4];
    if (find_win_line(view_board, r, c, s, line)) {
        g_winner = s;
        memcpy(view_win_line, line, sizeof(line));
        g_screen = SCR_OVER;
        g_t1 = GetTickCount64();
        g_timing = 0;
    } else if (stone_count() == BOARD_N * BOARD_N) {
        g_winner = TIE;
        g_screen = SCR_OVER;
        g_t1 = GetTickCount64();
        g_timing = 0;
    } else {
        g_turn = -s;
    }

    g_pending_r = g_pending_c = -1;
    g_hover_r = g_hover_c = -1;
    view_pending_r = view_pending_c = -1;
    view_hover_r = view_hover_c = -1;
    view_side = g_turn;
    return 1;
}

/* AI 还没写时的占位：随便找个空点，保证对局能继续。
   你的算法写好后，ai_move() 里不会再走到这里，可以删掉整个函数。 */
move_t fallback_move()
{
    for (int r = 0; r < BOARD_N; ++r)
        for (int c = 0; c < BOARD_N; ++c)
            if (view_board[r][c] == EMPTY) {
                move_t m;
                m.r = r;
                m.c = c;
                return m;
            }
    return move_none();
}

/* 轮到 AI：换算法就改这一行 */
void ai_move()
{
    move_t m;

    if (g_screen != SCR_PLAY || g_turn != g_ai) return;

    m = alphabeta_best(view_board, g_ai, AI_DEPTH);
    /* m = minimax_best(view_board, g_ai, AI_DEPTH); */
    /* m = mcts_best(view_board, g_ai, MCTS_ITERS);    */

    if (m.r < 0) m = fallback_move();   /* TODO: 算法写好后删掉这行 */

    if (m.r >= 0) place(m.r, m.c, g_ai);
}

/* ---------------- 画面 ---------------- */
void draw_menu()
{
    view_set_text_color(VIEW_CLR_ACCENT);
    view_set_font(46, 1);
    view_text_center(400, 100, L"FIVE IN A ROLL");

    view_set_text_color(VIEW_CLR_DIM);
    view_set_font(12, 0);
    view_text_center(400, 180, L"GOMOKU // 15 x 15 // HUMAN vs AI");

    view_draw_button(300, 250, 200, 48, L"START GAME",
                     in_rect(g_mx, g_my, 300, 250, 200, 48));
    view_draw_button(300, 320, 200, 48, L"QUIT GAME",
                     in_rect(g_mx, g_my, 300, 320, 200, 48));

    view_set_text_color(VIEW_CLR_DIM);
    view_set_font(9, 0);
    view_text_center(400, 445, L"(C) 2026  GOMOKU PROJECT  //  BUILD 0.3");
}

void draw_card(int x, int y, int w, int h, int s, const wchar_t* label, int hot)
{
    view_draw_button(x, y, w, h, L"", hot);
    view_draw_stone(x + w / 2, y + 45, s, 20);
    view_set_text_color(hot ? VIEW_CLR_ACCENT : VIEW_CLR_TEXT);
    view_set_font(16, hot);
    view_text_center(x + w / 2, y + h - 42, label);
}

void draw_side()
{
    view_set_text_color(VIEW_CLR_ACCENT);
    view_set_font(30, 1);
    view_text_center(400, 80, L"CHOOSE YOUR SIDE");

    view_set_text_color(VIEW_CLR_DIM);
    view_set_font(10, 0);
    view_text_center(400, 125, L"BLACK MOVES FIRST");

    draw_card(220, 200, 160, 130, BLACK_STONE, L"PLAY AS BLACK",
              in_rect(g_mx, g_my, 220, 200, 160, 130));
    draw_card(420, 200, 160, 130, WHITE_STONE, L"PLAY AS WHITE",
              in_rect(g_mx, g_my, 420, 200, 160, 130));

    view_set_text_color(VIEW_CLR_DIM);
    view_set_font(9, 0);
    view_text_center(400, 380, L"CLICK A SIDE TO START");
    view_text_center(400, 402, L"ESC  BACK");
}

void draw_over()
{
    view_fill_panel(100, 130, 420, 330);

    const wchar_t* result = L"DRAW";
    COLORREF rc = VIEW_CLR_CYAN;
    if (g_winner == g_human) { result = L"YOU WIN"; rc = VIEW_CLR_ACCENT; }
    else if (g_winner == g_ai) { result = L"YOU LOST"; rc = VIEW_CLR_RED; }

    view_set_text_color(rc);
    view_set_font(34, 1);
    view_text_center(260, 150, result);

    wchar_t b[64];
    const int sec = (int)((g_t1 - g_t0) / 1000);
    swprintf(b, 64, L"TIME %02d:%02d   MOVES %03d", sec / 60, sec % 60, stone_count());
    view_set_text_color(VIEW_CLR_DIM);
    view_set_font(11, 0);
    view_text_center(260, 200, b);

    view_draw_button(115, 270, 140, 42, L"PLAY AGAIN",
                     in_rect(g_mx, g_my, 115, 270, 140, 42));
    view_draw_button(265, 270, 140, 42, L"QUIT GAME",
                     in_rect(g_mx, g_my, 265, 270, 140, 42));
}

void draw_frame()
{
    view_begin_frame();

    switch (g_screen) {
    case SCR_MENU:
        draw_menu();
        break;
    case SCR_SIDE:
        draw_side();
        break;
    case SCR_PLAY:
        view_draw_board();
        view_draw_hud();
        break;
    case SCR_OVER:
        view_draw_board();
        view_draw_hud();
        draw_over();
        break;
    }

    view_end_frame();
}

/* ---------------- 输入 ---------------- */
void on_move(int x, int y, int r, int c)
{
    g_mx = x;
    g_my = y;

    const int can_hover = (g_screen == SCR_PLAY) && (g_turn == g_human) &&
                          (r >= 0) && (view_board[r][c] == EMPTY);
    if (can_hover) {
        g_hover_r = r;
        g_hover_c = c;
    } else {
        g_hover_r = g_hover_c = -1;
    }
    view_hover_r = g_hover_r;
    view_hover_c = g_hover_c;
}

void on_click(int x, int y, int r, int c)
{
    g_mx = x;
    g_my = y;

    switch (g_screen) {
    case SCR_MENU:
        if (in_rect(x, y, 300, 250, 200, 48)) g_screen = SCR_SIDE;
        else if (in_rect(x, y, 300, 320, 200, 48)) g_quit = 1;
        break;

    case SCR_SIDE:
        if (in_rect(x, y, 220, 200, 160, 130)) start_game(BLACK_STONE);
        else if (in_rect(x, y, 420, 200, 160, 130)) start_game(WHITE_STONE);
        break;

    case SCR_PLAY: {
        if (r < 0 || g_turn != g_human) break;
        if (view_board[r][c] != EMPTY) {
            g_pending_r = g_pending_c = -1;
        } else if (g_pending_r == r && g_pending_c == c) {
            place(r, c, g_human);          /* 第二次点同一个点：确认落子 */
        } else {
            g_pending_r = r;               /* 第一次点：选中 */
            g_pending_c = c;
        }
        view_pending_r = g_pending_r;
        view_pending_c = g_pending_c;
        break;
    }

    case SCR_OVER:
        if (in_rect(x, y, 115, 270, 140, 42)) g_screen = SCR_SIDE;
        else if (in_rect(x, y, 265, 270, 140, 42)) g_quit = 1;
        break;
    }
}

void on_key(int vk)
{
    if (vk == VK_ESCAPE) {
        if (g_screen != SCR_MENU) g_screen = SCR_MENU;
        else g_quit = 1;
        return;
    }
    if (vk == 'R' || vk == 'r') {
        if (g_screen == SCR_PLAY || g_screen == SCR_OVER) start_game(g_human);
        return;
    }
    if (vk == VK_RETURN && g_screen == SCR_MENU) {
        g_screen = SCR_SIDE;
        return;
    }
    if (vk == 'Q' || vk == 'q') {
        g_quit = 1;
    }
}

}  // namespace

int main()
{
    if (!view_init()) return 1;

    view_status = L"";
    view_engine = L"ALPHA-BETA  d3  (TODO)";
    view_pending_r = view_pending_c = -1;

    int need = 1;

    while (view_running() && !g_quit) {
        view_event_t ev;
        while (view_poll(&ev)) {
            need = 1;
            if (ev.kind == VIEW_EV_CLOSE) { g_quit = 1; break; }
            if (ev.kind == VIEW_EV_MOVE) on_move(ev.x, ev.y, ev.r, ev.c);
            else if (ev.kind == VIEW_EV_CLICK) on_click(ev.x, ev.y, ev.r, ev.c);
            else if (ev.kind == VIEW_EV_KEY) on_key(ev.key);
        }

        if (!view_running() || g_quit) break;

        if (g_timing) {
            const int sec = (int)((GetTickCount64() - g_t0) / 1000);
            if (sec != g_sec) {
                g_sec = sec;
                view_clock_seconds = sec;
                need = 1;
            }
        }

        /* 先画出人这一手，再让 AI 想，界面不会停在上一帧 */
        if (need) {
            draw_frame();
            need = 0;
        }

        if (g_screen == SCR_PLAY && g_turn == g_ai) {
            view_status = L"AI THINKING...";
            draw_frame();
            ai_move();
            view_status = (g_screen == SCR_OVER) ? L"GAME OVER" : L"YOUR TURN";
            need = 1;
        }

        Sleep(10);
    }

    view_shutdown();
    return 0;
}
