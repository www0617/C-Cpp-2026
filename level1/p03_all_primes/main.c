#include<stdio.h>
int test (int i,int *m,int *k)         //历遍所有质数
{    int *g=m;
    for (g=m;g<k;g++)
{
    if(i%(*g)==0) break;
}
    if (g==k)  return 1;
    else return 0;
}
int main()
{int i;  int arr [303]; int *k=arr;int *m=arr;arr [0]=2;      //arr长度是我先用程序试出来的，实际可以用素数定理估计
    for(i=2;i<=2000;i++)
    {
     if(test(i,m,k)==1){*k=i;(k++);}                          //开头2的情况比较特殊，是由于for循环g=k并使test函数输出为1
    }
    for (int *p=arr;p<k;p++)
    {
       printf("%5d",*p);
    }
    return 0;
}
