#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

/* 单向链表节点：存一个整数值，以及指向下一个节点的指针 */
typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* 往链表尾部追加一个节点（按输入顺序建表）。
   head 是二级指针：因为可能要把头节点本身改掉，所以要传地址。 */
void appendNode(Node **head, int value) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) return;          /* 分配失败就放弃，别踩空指针 */

    newNode->value = value;
    newNode->next = NULL;

    if (*head == NULL) {                  /* 空表：新节点就是头 */
        *head = newNode;
        return;
    }

    Node *p = *head;
    while (p->next != NULL) {             /* 走到最后一个节点 */
        p = p->next;
    }
    p->next = newNode;
}

/* 就地反转链表：只改指针方向，不新建节点。
   关键顺序——先存下 next，再掉头，最后前进，否则断链就找不回下一个了。 */
Node *reverseList(Node *head) {
    Node *curr = head;
    Node *next = NULL;
    Node *prev = NULL;

    while (curr != NULL) {
        next = curr->next;                /* ① 先记住下一个 */
        curr->next = prev;                /* ② 掉头 */
        prev = curr;                      /* ③ prev 前进 */
        curr = next;                      /* ④ curr 前进 */
    }
    return prev;                          /* 循环结束时 prev 就是新头 */
}

/* 释放整条链表：先存下一个，再 free 当前，否则释放完就找不到下一个了 */
void freeList(Node *head) {
    while (head != NULL) {
        Node *next = head->next;
        free(head);
        head = next;
    }
}

/* 从 searchp 节点开始找值为 5 的节点，返回它的序号（从 startnum 起数）。
   命中就 return，没命中才前进；走到 NULL 还没找到就返回 -1。 */
int search(Node *searchp, int startnum) {
    int num = startnum;
    while (searchp != NULL) {
        if (searchp->value == 5) {
            return num;
        }
        num++;
        searchp = searchp->next;
    }
    return -1;
}

int main() {
    int arr[] = {1, 2, 5, 4, 5};
    int n = (int)(sizeof(arr) / sizeof(arr[0]));   /* 程序自己数数组长度 */

    Node *head = NULL;
    for (int i = 0; i < n; i++) {
        appendNode(&head, arr[i]);
    }

    /* 1. 打印原链表 */
    Node *p = head;
    while (p != NULL) {
        printf("%d -> ", p->value);
        p = p->next;
    }
    printf("NULL\n");

    /* 2. 反转并打印 */
    head = reverseList(head);
    p = head;
    while (p != NULL) {
        printf("%d -> ", p->value);
        p = p->next;
    }
    printf("NULL\n");

    /* 3. 找第一个 5 */
    Node *p1 = head;
    int first = search(p1, 1);
    printf("first 5 num: %d\n", first);

    /* 4. 从第一个 5 之后继续找下一个 5 */
    int second = -1;
    if (first != -1) {                       /* 第一个都没找到就别找下一个了 */
        p1 = p1->next;                       /* 挪到第一个 5 的下一个 */
        second = search(p1, first + 1);      /* 序号接着 first+1 数起 */
    }
    printf("second 5 num: %d\n", second);

    freeList(head);
    return 0;
}
