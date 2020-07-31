#define commandDelay 9
String command="";
String responce="";
char buff[30];
void setup() {
  // put your setup code here, to run once:
pinMode(13,1);
digitalWrite(13,0);
Serial.begin(115200);
command.reserve(20);
responce.reserve(20);
}
void sendCommAndGetResp(){
  delay(commandDelay);
  command.toCharArray(buff,30);
  Serial.write(buff);
  delay(commandDelay);
  Serial.setTimeout(1);
  responce=Serial.readString();
}
//void loop() {
//    // put your main code here, to run repeatedly:
//  char buff[3]="";
//  Serial.write("AT");
//  Serial.setTimeout(1);
//  for(int i=0;i<2;i++){
//    buff[i]=Serial.read();
//    //Serial.print(c);
////    if(c==NULL){
////      break;
////    }
//  }
//  buff[2]='\0';
//  delay(9);
//  Serial.println(buff);
//  delay(9);
//}
//}
void loop(){
  command="AT";
  sendCommAndGetResp();
  Serial.println(responce);
}
