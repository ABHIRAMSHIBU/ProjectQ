#include<stdio.h>
#include<stdlib.h>
short seed[7]={23,341,3321,12342,1423,1234,8544};
int main(){
    lcong48(seed);
    long val =lrand48();
    printf("%d\n",val);
    val =lrand48();
    printf("%d\n",val);
    return 0;
}