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

/* 从 p 开始找第一个值为 v 的节点，返回节点指针；没找到返回 NULL。

   这里返回"节点指针"而不是"序号"，是关键：
   调用方要接着从命中节点的下一个继续找，就必须先拿到命中节点本身。
   旧写法 search(p1, 1) 只返回序号、p1 仍停在 head，
   于是 p1 = p1->next 从 head 往后挪，而不是从"第一个 5"往后挪——
   只有第一个 5 恰好是头节点时才碰巧算对。 */
Node *findNode(Node *p, int v) {
    while (p != NULL && p->value != v) {
        p = p->next;
    }
    return p;
}

/* 求节点在链表中的序号（从 1 起）；节点不在链表里返回 -1 */
int nodeIndex(Node *head, Node *target) {
    int i = 1;
    for (Node *p = head; p != NULL; p = p->next, ++i) {
        if (p == target) {
            return i;
        }
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
    Node *firstNode = findNode(head, 5);
    int first = (firstNode != NULL) ? nodeIndex(head, firstNode) : -1;
    printf("first 5 num: %d\n", first);

    /* 4. 从第一个 5 之后继续找下一个 5。
       起点必须是"命中节点->next"，而不是 head->next。 */
    int second = -1;
    if (firstNode != NULL && firstNode->next != NULL) {
        Node *secondNode = findNode(firstNode->next, 5);
        if (secondNode != NULL) {
            second = nodeIndex(head, secondNode);
        }
    }
    printf("second 5 num: %d\n", second);

    /* 5. 再验一遍"第一个 5 不在头节点"的情况。
       反转后的链表是 5 4 5 2 1，第一个 5 恰好在头节点，
       旧写法在这里碰巧算对；下面用未反转的 1 2 5 4 5 才能真正验证定位是否正确。 */
    {
        Node *h2 = NULL;
        for (int i = 0; i < n; i++) {
            appendNode(&h2, arr[i]);
        }

        Node *f2 = findNode(h2, 5);
        int i2 = (f2 != NULL) ? nodeIndex(h2, f2) : -1;
        int j2 = -1;
        if (f2 != NULL && f2->next != NULL) {
            Node *s2 = findNode(f2->next, 5);
            if (s2 != NULL) {
                j2 = nodeIndex(h2, s2);
            }
        }
        printf("unreversed 1 2 5 4 5 -> first 5 num: %d, second 5 num: %d\n", i2, j2);
        printf("expected            -> first 5 num: 3, second 5 num: 5\n");
        freeList(h2);
    }

    freeList(head);
    return 0;
}
