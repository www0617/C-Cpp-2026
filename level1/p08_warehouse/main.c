#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* 简单进销存：型号 + 数量，单向链表存储。
   数据格式「型号 数量」，启动读 stock.txt，退出写回。 */

typedef struct things {
    char type[32];
    int num;
    struct things *next;
} thingsinformation;

thingsinformation *head = NULL;   /* 链表头 */
thingsinformation *tail = NULL;   /* 链表尾，方便 O(1) 尾部追加 */

/* 入库：型号已存在就累加数量，不存在就新建节点挂到尾部。
   数量 <= 0 拒绝；累加前先判断会不会溢出 int。 */
void input(char newthing[32], int num) {
    if (num <= 0) {
        printf("invalid num\n");
        return;
    }

    thingsinformation *p = head;
    while (p != NULL) {
        if (strcmp(p->type, newthing) == 0) {
            if (p->num > INT_MAX - num) {   /* 累加会溢出 int */
                printf("overflow, not updated\n");
                return;
            }
            p->num += num;
            printf("updated %s, now %d\n", p->type, p->num);
            return;
        }
        p = p->next;
    }

    thingsinformation *ptrtonewthing =
        (thingsinformation *)malloc(sizeof(thingsinformation));
    if (ptrtonewthing == NULL) return;

    strncpy(ptrtonewthing->type, newthing, 31);   /* 拷 31 字节留一位给 \0 */
    ptrtonewthing->type[31] = '\0';               /* 手动补结尾符，防 strcmp 越界 */
    ptrtonewthing->num  = num;
    ptrtonewthing->next = NULL;

    if (head == NULL) {
        head = ptrtonewthing;
        tail = ptrtonewthing;
    } else {
        tail->next = ptrtonewthing;
        tail = ptrtonewthing;
    }
}

/* 查询：只读遍历链表，不修改任何节点，找不到时明确提示 */
void search(thingsinformation *p) {
    printf("please type the type of the thing\n");
    char searchtype[32];
    if (scanf("%31s", searchtype) != 1) {
        printf("invalid input\n");
        return;
    }
    while (p != NULL) {
        if (strcmp(p->type, searchtype) == 0) {
            printf("the information of the thing is:\n");
            printf("type:%s\n", p->type);
            printf("num:%d\n", p->num);
            return;
        }
        p = p->next;
    }
    printf("no such thing: %s\n", searchtype);
}

/* 出库：先按型号找到节点，再问出多少。
   摘节点分两种情况：
   - 头节点没有前驱，单独处理，摘完要同步修 head/tail
   - 中间/尾部节点用 prev->next 绕过它，尾节点还要把 tail 回退
   数量 == 现有量时整条删除并 free，部分出库只减 num。 */
void output(const char *type) {
    if (head == NULL) {
        printf("the storage is empty\n");
        return;
    }

    if (strcmp(head->type, type) == 0) {
        thingsinformation *old = head;

        printf("the information of the thing is:\n");
        printf("type:%s\n", old->type);
        printf("num:%d\n", old->num);
        printf("how many things do you want to out put,please type a number\n");
        int numout;
        if (scanf("%d", &numout) != 1) { printf("invalid input\n"); return; }
        if (numout <= 0) { printf("invalid input\n"); return; }
        if (numout > old->num) { printf("too much\n"); return; }

        if (numout < old->num) {
            old->num -= numout;
            printf("output finished,%dleft\n", old->num);
            return;
        }

        head = head->next;
        if (tail == old) tail = NULL;
        free(old);
        printf("output finished\n");
        return;
    }

    thingsinformation *prev = head;
    thingsinformation *cur  = head->next;

    while (cur != NULL) {
        if (strcmp(cur->type, type) == 0) {
            printf("the information of the thing is:\n");
            printf("type:%s\n", cur->type);
            printf("num:%d\n", cur->num);
            printf("how many things do you want to out put,please type a number\n");
            int numout;
            if (scanf("%d", &numout) != 1) { printf("invalid input\n"); return; }
            if (numout <= 0) { printf("invalid input\n"); return; }
            if (numout > cur->num) { printf("too much\n"); return; }

            if (numout < cur->num) {
                cur->num -= numout;
                printf("output finished,%dleft\n", cur->num);
                return;
            }

            prev->next = cur->next;
            if (cur == tail) tail = prev;
            free(cur);
            printf("output finished\n");
            return;
        }
        prev = cur;
        cur  = cur->next;
    }

    printf("no such thing: %s\n", type);
}

/* 启动时读库存文件；文件不存在当作空库存，不报错退出 */
void loadfromfile() {
    FILE *fp = fopen("stock.txt", "r");
    if (fp == NULL) {
        printf("no file\n");
        return;
    }
    char newtype[32];
    int num;
    while (fscanf(fp, "%31s %d", newtype, &num) == 2) {
        input(newtype, num);
    }
    fclose(fp);
}

/* 退出时把整条链表写回文件，一行「型号 数量」 */
void savetofile() {
    FILE *fp = fopen("stock.txt", "w");
    if (fp == NULL) {
        printf("failed found no file\n");
        return;
    }
    thingsinformation *p = head;
    while (p != NULL) {
        fprintf(fp, "%s %d\n", p->type, p->num);
        p = p->next;
    }
    fclose(fp);
}

/* 打印全部库存，空表时提示 */
void showall() {
    thingsinformation *p = head;
    while (p != NULL) {
        printf("%s  %d\n", p->type, p->num);
        p = p->next;
    }
    if (head == NULL) printf("storage is empty\n");
}

int main() {
    loadfromfile();
    while (1) {
        printf("please type a number: \n 1 for showing the storage \n 2 for input \n 3 for output \n 4 for exit \n 5 for search goods \n");
        int menu;
        int ret = scanf("%d", &menu);
        if (ret == EOF) {
            menu = 4;
        } else if (ret != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            printf("invalid input\n");
            continue;
        }
        switch (menu) {
            case 1:
                showall();
                break;
            case 2: {
                printf("please type type and num\n");
                int num;
                char newthing[32];
                if (scanf("%31s %d", newthing, &num) != 2) {
                    int ch;
                    while ((ch = getchar()) != '\n' && ch != EOF) { }
                    printf("invalid input\n");
                    break;
                }
                input(newthing, num);
                break;
            }
            case 3: {
                printf("please type the type of the thing\n");
                char searchtype[32];
                if (scanf("%31s", searchtype) != 1) {
                    printf("invalid input\n");
                    break;
                }
                output(searchtype);
                break;
            }
            case 4: {
                savetofile();               /* 先落盘 */
                /* 再释放整条链表，防止内存泄漏 */
                thingsinformation *p = head;
                thingsinformation *nextp;
                while (p != NULL) {
                    nextp = p->next;
                    free(p);
                    p = nextp;
                }
                return 0;
            }
            case 5:
                search(head);
                break;
            default:
                printf("invalid number\n");
        }
    }
    return 0;
}
