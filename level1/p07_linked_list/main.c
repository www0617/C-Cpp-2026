#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
        int value;
        struct Node * next;
    }   Node  ;

void appendNode(Node **head, int value) {
    Node* ptrtonewNode=(Node*)malloc(sizeof(Node));
    if (ptrtonewNode== NULL) return;
    ptrtonewNode->value=value;
    ptrtonewNode->next=NULL;
    if (*head==NULL) {
        *head=ptrtonewNode;
        return;
    }
    Node *p=*head;
    while (p->next!=NULL) {
        p=p->next;
    }

    p->next=ptrtonewNode;
}




Node* reverseList(Node *head) {

    Node*curr=head;
    Node*next=NULL;
    Node*prev=NULL;
    while (curr!=NULL) {
        next=curr->next;
        curr->next = prev;
        prev=curr;
        curr=next;

    }
    return prev;

}
void freeList(Node *head) {
    while (head!=NULL) {
        Node*p=head->next;
        free(head);
        head=p;

    }

}
    int search(Node* searchp,int startnum) {
        int num=startnum;
    while (searchp!=NULL) {

            if ((searchp)->value==5){

                return num;
        }
        num++;
        (searchp)=(searchp)->next;
        }


            return -1;


}

int main() {

    int arr[] = {1, 2, 5, 4, 5};
    int n = (int)(sizeof(arr) / sizeof(arr[0]));

    Node *head = NULL;
    for (int i = 0; i < n; i++) {
        appendNode(&head, arr[i]);
    }

    Node*p=head;

    while (p!=NULL) {
        printf("%d -> ",p->value);
        p=p->next;
    }
    printf("NULL\n");



    {
        head = reverseList (head);
        p = head;
        while (p != NULL) {
            printf("%d -> ", p->value);
            p = p->next;
        }
        printf("NULL\n");
    }


    Node*p1=head;
    int first = search(p1, 1);
    printf("first 5 num: %d\n", first);

    int second = -1;
    if (first != -1) {
        p1 = p1->next;
        second = search(p1, first+1);
    }
    printf("second 5 num: %d\n", second);


    freeList(head);
    return 0;
}