#include<stdio.h>
int test (int i)
{    int j=2;
    while(j<=i)
    {
        if(i%j==0)break;
        else j++;
    }
    if(j==i)
    {
        printf("%d",i);
    }
}
int main()
{int i;
    for(i=25;i<=2000;i++)
    {
        test(i);
    }
    return 0;
}                              //第一种方法
