#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#define BLOCK_SIZE 4 // 4 byte is the block size
#define KEY_LENGTH 10
#define DATA_LENGTH 10
uint32_t key[KEY_LENGTH]={0x62143c09, // 1645493257
                          0x5715e8a8, // 1461053608
                          0x9673c160, // 2524168544
                          0xa1bc4299, // 2713469593
                          0x258fffb6, // 630194102
                          0x6db3217c, // 1840456060
                          0xe9941e03, // 3918798339
                          0xd921b034, // 3642863668
                          0x7d0b6575, // 2097898869
                          0x1977ead0};// 427289296
                          // 320bit key can encrypt 40 data char min
uint8_t data[DATA_LENGTH]={0xff, //255
                           0x05, //5
                           0x50, //80
                           0x55, //85
                           0x77, //119
                           0x70, //112
                           0x07, //7
                           0x10, //16
                           0x11, //17
                           0x01, //1
                           };
void initRand(uint32_t seed){
    lcong48((unsigned short int *)(&(seed)));
}
void encblock(uint32_t key, uint32_t * data){
    printf("INENC %u\n",key);
    *(data)=*(data)^key;
}
void enc(uint32_t * key, uint8_t data[], int datalen){
    int poskey=0;
    int len=datalen;
    uint32_t dataBlock;
    for (size_t i = 0; i < datalen; i++){
        printf("DEBUG %u\n",data[i]);
    }
    
    for(int i=0;i<len;i+=BLOCK_SIZE){
        dataBlock=0;
        for(int j=i;j<(i+BLOCK_SIZE);j++){ //assmeble data block
            if(j>=datalen){
               printf("j>=datalen function, j=%d\n",j); 
            }
            else{
                //dataBlock a[3]a[2]a[1]a[0]
                dataBlock = dataBlock | (data[j]<<((j-i)*8));
                //printf("%u %u %u j=%d \n",data[j],dataBlock,(data[j]<<((j-i)*8)),j);
            }
        }
        //printf("%u\n",dataBlock);
        //data[i]=data[i]^key[poskey];
        uint32_t dataBlockBack=dataBlock;
        encblock(key[poskey],&dataBlock);
        printf("OUTENC %u %u\n",dataBlockBack,dataBlock);
        poskey=((i/BLOCK_SIZE)+1)%KEY_LENGTH;
        //Encypted data is available in dataBlock
        for(int j=i;j<(i+BLOCK_SIZE);j++){ //disassemble data block into array
            if(j<DATA_LENGTH){
                data[j]=dataBlock & 0xff;
                printf("DEISASS %u %u %d\n",data[j],dataBlock,j);
                dataBlock=dataBlock>>8;
            }
        }
    }
    for (size_t i = 0; i < datalen; i++){
        printf("DEBUG %u\n",data[i]);
    }
    
}

uint8_t sbox[32][2][2]={{{11,0},{31,1}}, // 1
                        {{9,1},{26,0}},  // 2
                        {{10,1},{2,1}},  // 3
                        {{4,1},{7,0}},   // 4
                        {{14,0},{29,1}}, // 5
                        {{27,1},{11,0}}, // 6
                        {{8,0},{21,1}},  // 7
                        {{12,1},{8,0}},  // 8
                        {{6,1},{22,0}},  // 9
                        {{32,1},{6,1}},  // 10
                        {{7,1},{30,0}},  // 11
                        {{25,0},{18,0}}, // 12
                        {{15,1},{28,0}}, // 13
                        {{3,0},{16,1}},  // 14
                        {{22,0},{19,0}}, // 15
                        {{18,1},{17,0}}, // 16
                        {{2,1},{32,0}},  // 17
                        {{1,0},{24,1}},  // 18 
                        {{24,0},{14,1}}, // 19
                        {{28,0},{10,0}}, // 20
                        {{26,0},{9,1}},  // 21
                        {{13,1},{13,0}}, // 22
                        {{30,1},{1,1}},  // 23
                        {{17,0},{20,1}}, // 24
                        {{19,1},{12,0}}, // 25
                        {{5,1},{5,0}},   // 26
                        {{21,0},{23,1}}, // 27
                        {{23,1},{25,0}}, // 28
                        {{31,0},{15,1}}, // 29
                        {{29,0},{3,0}},  // 30
                        {{20,0},{27,1}}, // 31
                        {{16,1},{4,0}}};  // 32
uint8_t swapbox[32][2][2]={{{23,1},{20,32}},  //1
                           {{7,8},{6,5}},     //2
                           {{8,29},{19,22}},  //3
                           {{29,11},{24,27}}, //4
                           {{9,10},{18,2}},   //5
                           {{25,26},{17,26}}, //6
                           {{4,17},{9,23}},   //7
                           {{6,19},{3,29}},   //8
                           {{2,24},{2,18}},   //9
                           {{16,30},{10,11}}, //10
                           {{27,28},{13,20}}, //11
                           {{18,32},{4,12}},  //12
                           {{15,20},{15,16}}, //13
                           {{3,22},{14,21}},  //14
                           {{5,6},{1,30}},    //15
                           {{13,25},{7,31}},  //16
                           {{14,31},{26,3}},  //17
                           {{21,12},{11,17}}, //18
                           {{31,2},{12,24}},  //19
                           {{11,23},{5,28}},  //20
                           {{32,3},{8,14}},   //21
                           {{10,13},{25,4}},  //22
                           {{12,14},{22,6}},  //23
                           {{19,21},{27,19}}, //24
                           {{20,4},{23,25}},  //25
                           {{17,15},{16,13}}, //26
                           {{22,5},{21,7}},   //27
                           {{24,7},{28,8}},   //28
                           {{26,27},{32,1}},  //29
                           {{28,9},{31,10}},  //30
                           {{1,18},{30,15}},  //31
                           {{30,16},{29,9}}}; //32
// above is a list { { {falseSwap1,falseSwap2}, {trueSwap1,trueSwap2} } }
uint32_t setbit(uint32_t val,uint8_t pos,uint8_t setval){
    if(setval==0)
        val = val & (~(1<<pos));
    else
        val = val | (1<<pos);
    return val;
}
uint8_t getbit(uint32_t val,uint8_t pos){
    if((val&(1<<pos))==(1<<pos))
        return 1;
    else
        return 0;
}
uint32_t swapbits(uint32_t val,uint8_t index1,uint8_t index2){
    uint8_t bit1=0;
    uint8_t bit2=0;
    uint32_t temp=1<<index1;
    if(temp == (val & temp))
        bit1=1;
    temp=1<<index2;
    if(temp == (val & temp))
        bit2=1;
    val=setbit(val,index1,bit2);
    val=setbit(val,index2,bit1);
    return val;
}
uint32_t seedKey(uint32_t seed,uint32_t key){
    for(int i=0;i<32;i++){
        if(getbit(seed,i)==1){
            if(key%8==0){
                key=setbit(key,sbox[i][1][0]-1,sbox[i][1][1]);
            }
            key=swapbits(key,swapbox[i][1][0]-1,swapbox[i][1][1]-1);
        }
        else{
            if(key%8==0){
                key=setbit(key,sbox[i][0][0]-1,sbox[i][0][1]);
            }
            key=swapbits(key,swapbox[i][0][0]-1,swapbox[i][0][1]-1);
        }
    }
    return key;
}
int main(){
    // printf("%d\n",getbit(5,2));
    // for(int i=0;i<10;i++)
    //     printf("seed= %d key= %u %u\n",2100280,key[i],seedKey(2100280,key[i]));
    // return 0;
    for(int i=0;i<100;i++){
        for(int j=0;j<10;j++){
            printf("%u\t",seedKey(2100280,key[j]));
            key[j]=seedKey(6782354,key[j]);
        }
        printf("\n");
    }
    // enc(key,data,DATA_LENGTH);
    // for(int i=0;i<DATA_LENGTH;i++){
    //     printf("POS = %d Data = %u\n",i,data[i]);
    // }
}