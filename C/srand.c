#include<stdio.h>
#include<stdlib.h>
unsigned int seed = 232;
int main(){
    srandom(seed);
    unsigned int out = random();
    printf("%d\n",out);
    out = random();
    printf("%d\n",out);
    printf("%d",sizeof(seed));
    printf("%d",RAND_MAX);
    long count=2;
    // while(1){
    //     if(rand()>=2147483648){
    //         printf("Count = %d\n",count);
    //     }
    //     count+=1;
    // }
}
