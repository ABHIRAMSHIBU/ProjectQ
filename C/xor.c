#include<stdio.h>
#include<string.h>
char data[20]="123456";
char key[10]="123456";
int keylen=7;
void enc(char * key, char * data, int datalen){
    int poskey=0;
    int len=datalen;
    for(int i=0;i<len;i++){
        printf("data[i]=%d",data[i]);
        printf(" key[poskey]=%d",key[poskey]);
        data[i]=data[i]^key[poskey];
        printf("\t");
        poskey=(i+1)%keylen;
    }
    printf("\n");
}
int main(){
    int datalen=strlen(data);
    for(int i=0;i<datalen;i++){
      printf("%d-",data[i]);
    }
    printf("\n");
    enc(key,data,datalen);
    for(int i=0;i<datalen;i++){
      printf("%d-",data[i]);
    }
    printf("\n");
    enc(key,data,datalen);
    for(int i=0;i<datalen;i++){
      printf("%d-",data[i]);
    }
    printf("\n");
}