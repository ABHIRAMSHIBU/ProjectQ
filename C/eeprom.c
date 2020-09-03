#include<stdio.h>
#include<stdint.h>
#define data_byte_size 4
uint8_t eeprom[1024];

void writeEEPROM(uint8_t data, int location){
    if(location>=0 && location<1024)
        eeprom[location]=data;
}
uint8_t readEEPROM(int location){
    if(location>=0 && location<1024)
        return eeprom[location];
}
int writeEEPROMCounter(unsigned int counter, int prev_location){
    int new_location = (prev_location+data_byte_size)%1024;
    int temp = 0xFF;
    for(int i=new_location-1;i>=prev_location;i--){
        uint8_t lsbyte = (counter & temp);
        printf("i=%d, lsbyte=%d\n",i,lsbyte);
        writeEEPROM(lsbyte,i);
        counter = counter >> 8;
    }
    return new_location;

}
int readEEPROMCounter(int location){
    unsigned int counter=0;
    for(int i=location;i<location+data_byte_size;i++){
        uint8_t lsbyte = readEEPROM(i);
        counter = counter | lsbyte;
        // printf("i=%d, lsbyte=%d, counter=%d\n",i,lsbyte,counter);
        if(i!=location+data_byte_size-1)
            counter = counter<<8;
    }
    
    return counter;
}
void recoverCounter(unsigned int *counter, int *position){
    *counter=0;
    *position=0;
    for(int i=0;i<256;i++){
        unsigned int temp = readEEPROMCounter(i*data_byte_size);
        printf("i=%d, temp=%u\n",i,temp);
        if(temp>*counter){
            printf("Hello World");
            *counter=temp;
            *position=i;
        }
    }
    *position*=data_byte_size;
}
void displayEEPROM(){
    printf("Address\tData\n");
    for(int i=0;i<1024;i++){
        if(i==0){
            printf("%d\t",i);
        }
        else if((i)%8==0){
            printf("\n");
            printf("%d\t",i);
        }
        printf("%d\t",eeprom[i]);
        if(i==1023){
            printf("\n");
        }   
    }
}
int main(){
    displayEEPROM();
    writeEEPROMCounter(4278124479,20);
    displayEEPROM();
    unsigned int counter = readEEPROMCounter(20);
    printf("The counter value at pos=%d is %d",20,counter);
    int position;
    unsigned int c=0;
    recoverCounter(&c,&position);
    printf("The search found counter=%u at position=%d",c,position);   
    return 0;
}