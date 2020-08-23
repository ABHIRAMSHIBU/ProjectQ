#include <Arduino.h>
#include <EEPROM.h> 
#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <LowPower.h>
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
char KEY[7]="123456";
bool ENCEN=true;
int keylen=6;
int location=0;
unsigned int counter=0;
/* END Global variables */

/* Global functions Section */
/* Heart beat will help to see if the code is stuck or running */
void heartBeat(){
    if(DEBUG && (millis()-heartBeatTimer)>500){
        digitalWrite(13,heartBeatVar);
        heartBeatTimer=millis();
        heartBeatVar=!heartBeatVar;
    }
}
void sendCommAndGetResp(unsigned int timeout){
    delay(commandDelay);
    command.toCharArray(buff,MESSAGELEN);
    Serial.write(buff);
    delay(commandDelay);
    Serial.setTimeout(timeout);
    responce=Serial.readString();
}
void enc(char * key, char * data, int datalen){
    int poskey=0;
    int len=datalen;
    for(int i=0;i<len;i++){
        data[i]=data[i]^key[poskey];
        poskey=(i+1)%keylen;
    }
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
            int len=strlen(buff);
            //Serial.write(buff);
            if(ENCEN)
                enc(KEY,buff,len);
            command="";
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
