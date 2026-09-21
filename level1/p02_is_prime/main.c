#include <stdio.h>
int main (void)
{
    int i,k=0;
    printf("please type an int number\n");
    scanf("%d",&i);
    char ch;
    while(i!=1) {
        ch = getchar();
        if(ch == '\n')
            break;
        if(ch == '.')printf("it is not a prime number\n");
    }
    if (i==1||i<=0) printf("it is not a prime number\n");

    else {
        for (int j=2;j<i;j++)
        {
            if (i%j==0) k++;
        }
        if (k==0) printf("it is a prime number\n");
    }
}