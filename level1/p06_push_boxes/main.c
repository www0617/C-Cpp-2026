#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_H       20
#define MAX_W       24
#define MAX_LEVEL    4
#define LEVEL_FMT   "levels/level%d.txt"
#define SCORE_FILE  "levels/scores.txt"

static char map[MAX_H][MAX_W];
static char boxes[MAX_H][MAX_W];
static int  px, py;

static int  gh, gw;
static int  box_total;
static int  level;
static int  steps;

enum {
    CMD_NONE, CMD_UP, CMD_DOWN, CMD_LEFT, CMD_RIGHT,
    CMD_RESTART, CMD_QUIT
};

static int arrowFromConsole(int k)
{
    switch (k) {
    case 72: return CMD_UP;
    case 80: return CMD_DOWN;
    case 75: return CMD_LEFT;
    case 77: return CMD_RIGHT;
    default: return CMD_NONE;
    }
}

static int arrowFromAnsi(int k)
{
    switch (k) {
    case 'A': return CMD_UP;
    case 'B': return CMD_DOWN;
    case 'D': return CMD_LEFT;
    case 'C': return CMD_RIGHT;
    default:  return CMD_NONE;
    }
}

static int readCommand(void)
{
    int k = _getch();

    if (k == -1) {
        printf("\nCannot read keyboard!\n");
        printf("Run -> Edit Configurations -> tick [x] Emulate terminal in output console\n");
        printf("or just run the exe inside cmd.\n");
        return CMD_QUIT;
    }

    if (k == 0 || k == 224) {
        return arrowFromConsole(_getch());
    }

    if (k == 27) {
        int k2 = _getch();
        if (k2 == '[' || k2 == 'O') {
            return arrowFromAnsi(_getch());
        }
        return CMD_NONE;
    }

    switch (k) {
    case 'w': case 'W': return CMD_UP;
    case 's': case 'S': return CMD_DOWN;
    case 'a': case 'A': return CMD_LEFT;
    case 'd': case 'D': return CMD_RIGHT;
    case 'r': case 'R': return CMD_RESTART;
    case 'q': case 'Q': return CMD_QUIT;
    default:            return CMD_NONE;
    }
}

static int loadLevel(int lv)
{
    char path[64];
    char line[MAX_W + 4];
    FILE *fp;
    int y, len, x;

    snprintf(path, sizeof(path), LEVEL_FMT, lv);

    fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    memset(map, ' ', sizeof(map));
    memset(boxes, 0, sizeof(boxes));
    box_total = 0;
    gh = gw = 0;
    px = py = 1;

    y = 0;
    while (y < MAX_H && fgets(line, (int)sizeof(line), fp) != NULL) {
        len = (int)strlen(line);

        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (len == 0) {
            continue;
        }
        if (len > gw) {
            gw = len;
        }

        for (x = 0; x < len; x++) {
            char c = line[x];

            map[y][x] = ' ';
            boxes[y][x] = 0;

            switch (c) {
            case '#':
                map[y][x] = '#';
                break;
            case 'T':
                map[y][x] = 'T';
                break;
            case 'B':
                boxes[y][x] = 1;
                box_total++;
                break;
            case 'P':
                px = x;
                py = y;
                break;
            default:
                break;
            }
        }
        y++;
    }
    fclose(fp);

    gh = y;
    return (gh > 0 && gw > 0) ? 1 : 0;
}

static void drawBoard(void)
{
    int x, y;

    system("cls");

    printf("Level %d    Steps %d    Boxes %d\n", level, steps, box_total);
    printf("Arrow keys / WASD move    R restart    Q quit\n\n");

    for (y = 0; y < gh; y++) {
        for (x = 0; x < gw; x++) {
            if (x == px && y == py) {
                putchar('P');
            } else if (boxes[y][x]) {
                putchar(map[y][x] == 'T' ? '*' : 'B');
            } else {
                putchar(map[y][x]);
            }
        }
        putchar('\n');
    }

    putchar('\n');
    printf("# wall   space floor   P you   B box   T target   * box on target\n");
}

static int tryMove(int dx, int dy)
{
    int nx = px + dx;
    int ny = py + dy;
    int bx, by;

    if (nx < 0 || nx >= gw || ny < 0 || ny >= gh) {
        return 0;
    }
    if (map[ny][nx] == '#') {
        return 0;
    }

    if (boxes[ny][nx]) {
        bx = nx + dx;
        by = ny + dy;

        if (bx < 0 || bx >= gw || by < 0 || by >= gh) return 0;
        if (map[by][bx] == '#') return 0;
        if (boxes[by][bx])     return 0;

        boxes[ny][nx] = 0;
        boxes[by][bx] = 1;
    }

    px = nx;
    py = ny;
    steps++;
    return 1;
}

static int isWin(void)
{
    int x, y;

    if (box_total == 0) {
        return 0;
    }

    for (y = 0; y < gh; y++) {
        for (x = 0; x < gw; x++) {
            if (boxes[y][x] && map[y][x] != 'T') {
                return 0;
            }
        }
    }
    return 1;
}

static int scoreOf(int s)
{
    int sc = 1000 - s * 10;
    return (sc < 0) ? 0 : sc;
}

static void loadScores(int best[])
{
    FILE *fp;
    int lv, st, i;

    for (i = 0; i <= MAX_LEVEL; i++) {
        best[i] = 0;
    }

    fp = fopen(SCORE_FILE, "r");
    if (fp == NULL) {
        return;
    }
    while (fscanf(fp, "%d %d", &lv, &st) == 2) {
        if (lv >= 1 && lv <= MAX_LEVEL) {
            best[lv] = st;
        }
    }
    fclose(fp);
}

static void saveScores(const int best[])
{
    FILE *fp;
    int i;

    fp = fopen(SCORE_FILE, "w");
    if (fp == NULL) {
        printf("Cannot write scores file, check the levels folder.\n");
        return;
    }
    for (i = 1; i <= MAX_LEVEL; i++) {
        if (best[i] > 0) {
            fprintf(fp, "%d %d\n", i, best[i]);
        }
    }
    fclose(fp);
}

int main(void)
{
    int best[MAX_LEVEL + 1];
    int lv;

    loadScores(best);

    for (lv = 1; lv <= MAX_LEVEL; lv++) {
        int quit = 0;
        int won = 0;

        level = lv;
        if (!loadLevel(lv)) {
            break;
        }
        steps = 0;

        while (!won && !quit) {
            int cmd;
            int moved = 0;

            drawBoard();
            cmd = readCommand();

            switch (cmd) {
            case CMD_UP:      moved = tryMove(0, -1); break;
            case CMD_DOWN:    moved = tryMove(0, 1);  break;
            case CMD_LEFT:    moved = tryMove(-1, 0); break;
            case CMD_RIGHT:   moved = tryMove(1, 0);  break;
            case CMD_RESTART:
                if (!loadLevel(lv)) {
                    quit = 1;
                }
                steps = 0;
                break;
            case CMD_QUIT:
                quit = 1;
                break;
            default:
                break;
            }

            if (moved && isWin()) {
                won = 1;
            }
        }

        if (quit) {
            break;
        }

        drawBoard();
        printf("\nLevel %d cleared!  Steps: %d  Score: %d\n", lv, steps, scoreOf(steps));

        if (best[lv] == 0 || steps < best[lv]) {
            best[lv] = steps;
            printf("NEW RECORD!\n");
        } else {
            printf("Best so far: %d steps (%d pts)\n", best[lv], scoreOf(best[lv]));
        }
        saveScores(best);

        printf("\nPress any key for the next level...");
        _getch();
    }

    {
        int total = 0;
        int done = 0;

        printf("\n========== RESULT ==========\n");
        for (lv = 1; lv <= MAX_LEVEL; lv++) {
            if (best[lv] > 0) {
                printf("Level %d: %3d steps  %4d pts\n", lv, best[lv], scoreOf(best[lv]));
                total += scoreOf(best[lv]);
                done++;
            } else {
                printf("Level %d: not cleared\n", lv);
            }
        }
        printf("----------------------------\n");
        printf("Cleared %d level(s), total %d pts\n", done, total);
        printf("Scores saved to %s\n", SCORE_FILE);
        printf("\nPress any key to exit...");
        _getch();
    }

    return 0;
}
