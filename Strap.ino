#include<Arduino.h>
#define VERSION "1.0-alpha"
#define commandDelay 9
#define DEBUG true
#define True true
#define False false
#define timeoutShort 1
#define timeoutLong 200
/* Global variables section */
String command="";
String responce="";
long heartBeatTimer;
bool heartBeatVar=false;
char buff[30];
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
    command.toCharArray(buff,30);
    Serial.write(buff);
    delay(commandDelay);
    Serial.setTimeout(timeout);
    responce=Serial.readString();
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
        Serial.println(temp);
    }
    heartBeat();
}
