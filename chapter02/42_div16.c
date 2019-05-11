/*************************************************************************
	> File Name: 42_div16.c
	> Description: 42_div16.c
	> Author: Bowser
	> Mail: yuqlang@163.com 
	> Created Time: 2019-05-04 19:18:41
 ************************************************************************/

#include<stdio.h>
int div16();

int main(){
    int x;
    scanf("%d",&x);
    x = div16(x);
    printf("%d",x);
    return 0;
}

int div16(int x) {
    int bias = (x >> 31) & 0xF; //计算偏量
    return (x + bias) >> 4;
}
