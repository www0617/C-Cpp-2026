# include <stdio.h>
#define N 64
void HAN(int k,char start ,char end ,char tempt)
{if(k==1){printf("%c->%c\n",start,end);return;}
    HAN(k-1,start,tempt,end);
    printf("%c->%c\n",start,end);
    HAN(k-1,tempt,end,start);}
int main(void)
{
    char a='A';char b='B' ;char c='C';
    HAN(N,a,c,b);
}