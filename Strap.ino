#include <Arduino.h>
#include <EEPROM.h> 
#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <LowPower.h>
#include <AESLib.h>
#include <stdlib.h>
#define ID      "1be32f-e11"
#define VERSION "1.0-alpha"
#define RESETPIN 8
#define commandDelay 9
#define DEBUG true
#define True true
#define False false
#define timeoutShort 10
#define timeoutLong 200
#define MESSAGELEN 100           // 100 Bytes message length
#define data_byte_size 4         // EEPROM
/* Global variables section */
String command="";
String responce="";
long heartBeatTimer;
bool heartBeatVar=false;
char buff[MESSAGELEN];
char KEY[]="1234567891234567891234567890123";
bool ENCEN=true;
int keylen=6;
int location=0;
unsigned int counter=0;
/* END Global variables */

/* Global functions Section */
/* Heart beat will help to see if the code is stuck or running */
//Encryption
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
                        {{17,1},{20,1}}, // 24
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
uint32_t seedKey(uint32_t seed,uint8_t Key){
    uint32_t key = Key * 4;
    for(int i=0;i<32;i++){
        if(getbit(seed,i)==1){
            key=setbit(key,sbox[i][1][0]-1,sbox[i][1][1]);
            key=swapbits(key,swapbox[i][1][0]-1,swapbox[i][1][1]-1);
        }
        else{
            //key=setbit(key,sbox[i][0][0]-1,sbox[i][0][1]);
            key=swapbits(key,swapbox[i][0][0]-1,swapbox[i][0][1]-1);
        }
    }
    return key/4;
}
void enc(char * key, char * data, int datalen){
    int poskey=0;
    int len=datalen;
    for(int i=0;i<len;i++){
        data[i]=data[i]^key[poskey];
        poskey=(i+1)%keylen;
    }
}
//Generic
void heartBeat(){
    if(DEBUG && (millis()-heartBeatTimer)>500){
        digitalWrite(13,heartBeatVar);
        heartBeatTimer=millis();
        heartBeatVar=!heartBeatVar;
    }
}
//Bluetooth
void sendCommAndGetResp(unsigned int timeout){
    delay(commandDelay);
    command.toCharArray(buff,MESSAGELEN);
    Serial.write(buff);
    delay(commandDelay);
    Serial.setTimeout(timeout);
    responce=Serial.readString();
}
void bluetoothInit(){
    //Set role as periferal
    int retry=5;
    while(True){
        command=F("AT+ROLE0");
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce==F("OK+Set:0")){
            break;
        }
    }
    //Set autostart
    retry=5;
    while(True){
        command=F("AT+IMME0");
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce==F("OK+Set:0")){
            break;
        }
    }
    //Set Bluetooth Name
    retry=5;
    while(True){
        command=F("AT+NAMEStrap");
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce==F("OK+Set:Strap")){
            break;
        }
    }
    //Set Bluetooth paring code
    retry=5;
    while(True){
        command=F("AT+PASS123456");
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce==F("OK+Set:123456")){
            break;
        }
    }
    retry=5;
    //Set Bluetooth paring not needed
    while(True){
        command=F("AT+TYPE0");
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce==F("OK+Set:0")){
            break;
        }
    }
    
    retry=5;
    //Set Bluetooth Service UUID
    while(True){
        command=F("AT+UUID0x0003");
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce==F("OK+Set:0x0003")){
            break;
        }
    }

    // retry=5;
    // //Set Bluetooth power
    // while(True){
    //     command=F("AT+POWE7");
    //     sendCommAndGetResp(timeoutLong);
    //     if(DEBUG){
    //         Serial.println(responce);
    //     }
    //     retry--;
    //     if(retry==0 || responce==F("OK+Set:7")){
    //         break;
    //     }
    // }
}

// EEPROM FUNCTIONS 
int writeEEPROMCounter(unsigned int counter, int prev_location){
    int new_location = (prev_location+data_byte_size)%1024;
    int temp = 0xFF;
    for(int i=new_location-1;i>=prev_location;i--){
        uint8_t lsbyte = (counter & temp);
        EEPROM.write(i,lsbyte);
        counter = counter >> 8;
    }
    return new_location;
}
int readEEPROMCounter(int location){
    unsigned int counter=0;
    for(int i=location;i<location+data_byte_size;i++){
        uint8_t lsbyte = EEPROM.read(i);
        counter = counter | lsbyte;
        if(i!=location+data_byte_size-1)
            counter = counter<<8;
    }
    return counter;
}
void recoverCounter(unsigned int *counter, int *location){
    *counter=0;
    *location=0;
    for(int i=0;i<256;i++){
        unsigned int temp = readEEPROMCounter(i*data_byte_size);
        if(temp>*counter){
            *counter=temp;
            *location=i;
        }
    }
    *location*=data_byte_size;
}
void EEPROMInit(){
    pinMode(RESETPIN,0);
    digitalWrite(RESETPIN,1);
    if(digitalRead(RESETPIN)==1){
        for(int i=0;i<1024;i++){
            EEPROM.write(i,0);
        }
        Serial.println(F("RST_SUC"));
        delay(1);
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    }  
    else{
        recoverCounter(&counter,&location);
        if(DEBUG==true){
            Serial.println("CTR "+String(counter)+" POS "+String(location));
        }
    }
}
/* END Global functions */

void setup() {
    if(DEBUG){
        //Heart beat code
        pinMode(13,1);
        digitalWrite(13,1);
        heartBeatTimer=millis();
        //End heart beat code
    }
    //Power up Bluetooth Module
    pinMode(12,1);
    digitalWrite(12,0);
    //End power up bluetooth module
    Serial.begin(115200);
    command.reserve(20);
    responce.reserve(20);
    //Initialize EEPROM if RESETPIN disconnected
    EEPROMInit();
    //Initialize BT
    bluetoothInit();
    //Done Initialization
}
void loop(){
    // command="AT";
    // sendCommAndGetResp();
    // if(DEBUG){
    //     Serial.println(responce);
    // }
    
    Serial.setTimeout(timeoutShort);
    String temp=Serial.readString();
    if(temp!=F("")){
        if(temp==F("VERSION")){
            Serial.println(F(VERSION));
        }
        else if(temp==F("ENCDIS")){
            ENCEN=false;
        }
        else if(temp==F("ENCEN")){
            ENCEN=true;
        }
        else if(temp.substring(0,3)==F("ENC")){
            temp+=String(F(":"));
            temp+=F(ID);
            temp+=String(F(":"));
            temp+=String(counter);
            temp.substring(3).toCharArray(buff,MESSAGELEN);
            int len=MESSAGELEN;
            //Serial.write(buff);
            if(ENCEN)
                //enc(KEY,buff,len);
                //Bringup AES
                aes256_enc_single((uint8_t *)KEY,buff);
            command="";
            srand(100);
            //Serial.print(strlen(buff));
            for(int i=0;i<len;i++){
                command+=String(buff[i],HEX);
                if(i<len-1)
                    command+="-";
            }
            sendCommAndGetResp(timeoutShort);
            counter++;
            location=writeEEPROMCounter(counter,location);
        }
    }
    //command="Hello World!";
    //sendCommAndGetResp(timeoutLong);
    heartBeat();
}
