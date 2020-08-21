#include<Arduino.h>
#define VERSION "1.0-alpha"
#define commandDelay 9
#define DEBUG true
#define True true
#define False false
#define timeoutShort 10
#define timeoutLong 200
#define MESSAGELEN 100           // 100 Bytes message length
/* Global variables section */
String command="";
String responce="";
long heartBeatTimer;
bool heartBeatVar=false;
char buff[MESSAGELEN];
char KEY[7]="123456";
bool ENCEN=true;
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
char bytexor(char a, char b){
    bool bita;
    bool bitb;
    char out=0;
    for(int i=0;i<8;i++){
        if((a && (1<<i))>0){
            bita=1;
        }
        else{
            bita=0;
        }
        if((b && (1<<i))>0){
            bitb=1;
        }
        else{
            bitb=0;
        }
        //bita=bita ^ bitb;
        if(bita==bitb){
            bita=0;
            Serial.println();
            Serial.println('H');
        }
        else{
            bita=1;
            Serial.println();
            Serial.println('L');
        }
        out = out || (bita<<i);
    }
    return out;
}
void encryptOTP(char * data){
   int len = strlen(data);
   int lenkey = strlen(KEY);
   int pos=0;
   for(int i=0;i<len;i++){
       pos=i%lenkey;
       data[i]=bytexor(data[i], KEY[pos]);
       //Serial.println(data[i],"INT");
       //Serial.println(KEY[pos]) ;
   }
}
/* END Global functions */

void bluetoothInit(){
    //Set role as periferal
    int retry=5;
    while(True){
        command="AT+ROLE0";
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce=="OK+Set:0"){
            break;
        }
    }
    //Set autostart
    retry=5;
    while(True){
        command="AT+IMME0";
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce=="OK+Set:0"){
            break;
        }
    }
    //Set Bluetooth Name
    retry=5;
    while(True){
        command="AT+NAMEStrap";
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce=="OK+Set:Strap"){
            break;
        }
    }
    //Set Bluetooth paring code
    retry=5;
    while(True){
        command="AT+PASS123456";
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce=="OK+Set:123456"){
            break;
        }
    }
    retry=5;
    //Set Bluetooth paring not needed
    while(True){
        command="AT+TYPE0";
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce=="OK+Set:0"){
            break;
        }
    }
    
    retry=5;
    //Set Bluetooth Service UUID
    while(True){
        command="AT+UUID0x0003";
        sendCommAndGetResp(timeoutLong);
        if(DEBUG){
            Serial.println(responce);
        }
        retry--;
        if(retry==0 || responce=="OK+Set:0x0003"){
            break;
        }
    }

    // retry=5;
    // //Set Bluetooth power
    // while(True){
    //     command="AT+POWE7";
    //     sendCommAndGetResp(timeoutLong);
    //     if(DEBUG){
    //         Serial.println(responce);
    //     }
    //     retry--;
    //     if(retry==0 || responce=="OK+Set:7"){
    //         break;
    //     }
    // }
}

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
    if(temp!=""){
        if(temp=="VERSION"){
            Serial.println(VERSION);
        }
        else if(temp=="ENCDIS"){
            ENCEN=false;
        }
        else if(temp=="ENCEN"){
            ENCEN=true;
        }
        else if(temp.substring(0,3)=="ENC"){
            temp.substring(3).toCharArray(buff,MESSAGELEN);
            int len=strlen(buff);
            //Serial.write(buff);
            if(ENCEN)
                encryptOTP(buff);
            //Serial.print(strlen(buff));
            for(int i=0;i<len;i++){
                Serial.print(buff[i]);
                Serial.print('-');
            }
        }
    }
    //command="Hello World!";
    //sendCommAndGetResp(timeoutLong);
    heartBeat();
}
