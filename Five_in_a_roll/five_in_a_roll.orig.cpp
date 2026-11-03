//
// Created by wuyuxuan on 2026/10/28.
//
/*
今日任务棋盘与棋子


void setbkcolor(COLORREF color);
void closegraph();close
void polyline(
    const POINT *points,
    int num
);
points
每个点的坐标，数组元素个数为 num。num多条线段的顶点个数。
POINT pts[] = { {50, 200}, {200, 200}, {200, 50} };
polyline(pts, 3);
int pts[] = {50, 200,  200, 200,  200, 50};
polyline((POINT*)pts, 3);


void line(
    int x1,
    int y1,
    int x2,
    int y2
);

void setlinestyle(
    int style,
    int thickness = 1,
    const DWORD *puserstyle = NULL,
    DWORD userstylecount = 0
);

void settextcolor(COLORREF color);


void outtextxy(
    int x,
    int y,
    LPCTSTR str
);

void outtextxy(
    int x,
    int y,
    TCHAR c
);

TCHAR s[] = _T("Hello World");
outtextxy(10, 20, s);输出字符示例

void setlinestyle(
    const LINESTYLE* pstyle
);
void setlinestyle(
    int style,
    int thickness = 1,
    const DWORD *puserstyle = NULL,
    DWORD userstylecount = 0
);画线样式

void setaspectratio(
    float xasp,
    float yasp
);放缩

void solidcircle(
    int x,
    int y,
    int radius
);圆形棋子

void setfillcolor(COLORREF color);填颜色
*/

#include <graphics.h>
#include <conio.h>
#include <tchar.h>

#define ROWS 15
#define COLS 15
#define INFO_X 550
#define INFO_Y 350
#define INFO_W 260      // 至少 220 才能装下这串字
#define INFO_H 30

typedef struct {
    int start_x;                // 棋盘左上角起点 X (50)
    int start_y;                // 棋盘左上角起点 Y (30)
    int gap;                    // 每个格子的间距 (30)
    int board[ROWS][COLS];      // 核心逻辑：15x15 的二维状态数组，0空 1黑 2白
} ChessGame;

// 直接初始化，不需要写那几十个点
ChessGame game = {
    50,             // start_x
    30,             // start_y
    30,             // gap
    { {0} }         // 棋盘全置0
};

enum WordIndex {
    TITLE, START, QUIT, CONFIRM, YOUR_TURN, WIN, LOST, PLAY_AGAIN, WORD_COUNT
};   //用于控制按键

const TCHAR* GameTexts[] = {
    _T("FIVE IN A ROLL"),
    _T("START GAME"),
    _T("QUIT GAME"),
    _T("CONFIRM"),
    _T("YOUR TURN"),
    _T("YOU WIN"),
    _T("YOU LOST"),
    _T("PLAY AGAIN")
};//一些要用的字 定义全局常量数组

void paintdot(int x,int y) {
    setfillcolor(BLUE);
    solidcircle(x,y,10);
}//用于打印棋子

  MOUSEMSG getmouseinformation() {
    MOUSEMSG m = GetMouseMsg();
    return m;
}//读取mose xy          MOUSEMSG m= getmouseinformation();  m.lbutton  yes 1;


void drawBoard() {
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID|PS_ENDCAP_ROUND,1,NULL,0);
    for (int i = 0; i < ROWS; i++) {
        // 画横线
        line(game.start_x, game.start_y + i * game.gap,
             game.start_x + (COLS - 1) * game.gap, game.start_y + i * game.gap);
        // 画竖线
        line(game.start_x + i * game.gap, game.start_y,
             game.start_x + i * game.gap, game.start_y + (ROWS - 1) * game.gap);
    }
}   //画横线

void initthegame() {
    initgraph(640*2.5,480*2);
    setaspectratio(2,2);
    setbkcolor(BLACK);
    cleardevice();
    drawBoard();
    LOGFONT f;
    gettextstyle(&f);             // 1. 获取当前字体设置
    f.lfQuality = PROOF_QUALITY;  // 2. 仅修改质量参数
    settextstyle(&f);
    settextstyle(50, 0, L"Segoe Script");
    outtextxy(500, 50, GameTexts[0]);
    settextstyle(30, 0, L"Times New Roman");
    outtextxy(550, 200, GameTexts[1]);
    outtextxy(550, 300, GameTexts[2]);
}  //棋盘，标题，开始退出

POINT get_point(int row, int col) {
    POINT p;
    p.x = game.start_x + col * game.gap;
    p.y = game.start_y + row * game.gap;
    return p;
}   //绘制时点的坐标

int get_mouse_r_c(int mx, int my, int* r, int* c) {
    int col = (mx - game.start_x + game.gap / 2) / game.gap;
    int row = (my - game.start_y + game.gap / 2) / game.gap;
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return 0;
    POINT p = get_point(row, col);
    if (abs(mx - p.x) + abs(my - p.y) <= 12) {
        *r = row;
        *c = col;
        return 1;
    } 
    return 0;
}  //鼠标离哪个最近

void print_mouse_info(int mx, int my, int r, int c, int hit) {
    TCHAR buf[64];

    // 1) 黑底：先刷掉旧字
    setfillcolor(BLACK);
    solidrectangle(INFO_X - 4, INFO_Y - 2,
                   INFO_X + INFO_W, INFO_Y + INFO_H);

    // 2) 白字
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);

    if (hit)
        _stprintf_s(buf, _T("row=%d col=%d  (%d,%d)"), r, c, mx, my);
    else
        _stprintf_s(buf, _T("鼠标(%d,%d) 未命中"), mx, my);

    // 3) 文字位置 = 黑底位置
    outtextxy(INFO_X, INFO_Y, buf);
}

int main() {
    initthegame ();
    paintdot(20,20);
    int x;
    int y;
    while (1) {
        MOUSEMSG m=getmouseinformation();
        int hit= get_mouse_r_c(m.x,m.y,&x,&y);
        print_mouse_info(m.x,m.y,x,y,hit);
    }


    ExMessage msg;
    getmessage(&msg, EX_KEY );


return 0;
}