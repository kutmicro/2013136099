#include "U8glib.h"
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
 
SoftwareSerial mySerial(10, 11); // RX, TX
 
U8GLIB_NHD_C12864 u8g(13, 11, 10, 9, 8);  // SPI Com: SCK = 13, MOSI = 11, CS = 10, CD = 9, RST = 8
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
int volumNum = 10; //소리의 크기(시작볼륨)
const int trig = 3;           // 변수 trig를 생성하고 3 대입한다
const int echo = 2;          // 변수 echo를 생성하고 2 대입한다
int waterfind;          //물탱크 안의 물 확인(0=물의 양 충분, 1 = 부족, 2=센서 에러)

void draw(void) {
 
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  if(waterfind==0 ){//물의 양 LCD에출력
    u8g.drawStr( 0, 20, "Water : enough");
  }
  else if(waterfind ==1){
     u8g.drawStr( 0, 20, "Water : lack");
  }
  else if(waterfind ==2){
     u8g.drawStr( 0, 20, "Water : error");
  }
  
  if(volumNum == 1){//소리 LCD에출력
    u8g.drawStr( 0, 50, "Sound : 1");
  }
  else if(volumNum == 2){
    u8g.drawStr( 0, 50, "Sound : 2");
  }
  else if(volumNum == 3){
    u8g.drawStr( 0, 50, "Sound : 3");
  }
  else if(volumNum == 4){
    u8g.drawStr( 0, 50, "Sound : 4");
  }
  else if(volumNum == 5){
    u8g.drawStr( 0, 50, "Sound : 5");
  }
  else if(volumNum == 6){
    u8g.drawStr( 0, 50, "Sound : 6");
  }
  else if(volumNum == 7){
    u8g.drawStr( 0, 50, "Sound : 7");
  }
  else if(volumNum == 8){
    u8g.drawStr( 0, 50, "Sound : 8");
  }
  else if(volumNum == 9){
    u8g.drawStr( 0, 50, "Sound : 9");
  }
  else if(volumNum == 10){
    u8g.drawStr( 0, 50, "Sound : 10");
  }
  else if(volumNum == 11){
    u8g.drawStr( 0, 50, "Sound : 11");
  }
  else if(volumNum == 12){
    u8g.drawStr( 0, 50, "Sound : 12");
  }
  else if(volumNum == 13){
    u8g.drawStr( 0, 50, "Sound : 13");
  }
  else if(volumNum == 14){
    u8g.drawStr( 0, 50, "Sound : 14");
  }
  else if(volumNum == 15){
    u8g.drawStr( 0, 50, "Sound : 15");
  }
  else if(volumNum == 16){
    u8g.drawStr( 0, 50, "Sound : 16");
  }
  else if(volumNum == 17){
    u8g.drawStr( 0, 50, "Sound : 17");
  }
  else if(volumNum == 18){
    u8g.drawStr( 0, 50, "Sound : 18");
  }
  else if(volumNum == 19){
    u8g.drawStr( 0, 50, "Sound : 19");
  }
  else if(volumNum == 20){
    u8g.drawStr( 0, 50, "Sound : 20");
  }
  else if(volumNum == 21){
    u8g.drawStr( 0, 50, "Sound : 21");
  }
  else if(volumNum == 22){
    u8g.drawStr( 0, 50, "Sound : 22");
  }
  else if(volumNum == 23){
    u8g.drawStr( 0, 50, "Sound : 23");
  }
  else if(volumNum == 24){
    u8g.drawStr( 0, 50, "Sound : 24");
  }
  else if(volumNum == 25){
    u8g.drawStr( 0, 50, "Sound : 25");
  }
  else if(volumNum == 26){
    u8g.drawStr( 0, 50, "Sound : 26");
  }
  else if(volumNum == 27){
    u8g.drawStr( 0, 50, "Sound : 27");
  }
  else if(volumNum == 28){
    u8g.drawStr( 0, 50, "Sound : 28");
  }
  else if(volumNum == 29){
    u8g.drawStr( 0, 50, "Sound : 29");
  }
  else if(volumNum == 30){
    u8g.drawStr( 0, 50, "Sound : 30");
  }

}

void setup(void) {
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  pinMode(trig, OUTPUT);  // trig핀(3)을 출력모드로 설정한다
  pinMode(echo, INPUT); // echo핀(2)을 입력모드로 설정한다
  Serial.begin(9600);
  mySerial.begin (9600);
  mp3_set_serial (mySerial);    //set softwareSerial for DFPlayer-mini mp3 module 
  delay(1);                     // delay 1ms to set volume
  mp3_set_volume (volumNum);          // value 0~30볼륨설정
  u8g.setContrast(0); // Config the contrast to the best effect
  u8g.setRot180();// rotate screen, if required
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
}

void loop(void) {
  //초음파센서
  digitalWrite(trig, HIGH);  // trig핀에 HIGH신호를 보낸다
  delayMicroseconds(10);  // 10마이크로초(1/100,000초) 동안
  digitalWrite(trig, LOW);  // trig핀에 LOW신호를 보낸다
  int distance = pulseIn(echo, HIGH) * 17 / 1000; 
  // 변수 distance = echo핀이 다시 HIGH신호를 받기까지의 시간 * 17/1000
  if(distance <= 5 && distance >=0 ){//물의 양에 따라 waterfind에 다르게 저장 (0=물의 양 충분, 1 = 부족, 2=센서 에러)
    waterfind = 0;
  }
  else if(distance > 5 && distance <=15){
    waterfind = 1;
  }
  else {
    waterfind = 2;
  }
  Serial.print(distance);  // 변수 distance를 출력한다
  Serial.println("cm");   // 문자열 cm을 출력하고 줄바꿈
  delay(100);             // 딜레이 0.1초
  
  
  //컨트롤러 신호를 시리얼 모니터에 출력
  Serial.print("Switch:  ");
  Serial.print(digitalRead(SW_pin));
  Serial.print("\n");
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_pin));
  Serial.print("\n");
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_pin));
  Serial.print("\n\n");
  if((analogRead(X_pin)>800&&analogRead(X_pin)<850)&&(analogRead(Y_pin)>700&&analogRead(X_pin)<850)){//볼륨조절(볼륨 높이기)
    if(volumNum <30){
      volumNum++;
    }
    else if(volumNum == 30){
      volumNum = 1;
    }
    mp3_set_volume (volumNum);  //볼륨 설정
  }
  if((analogRead(X_pin)>350&&analogRead(X_pin)<450)&&(analogRead(Y_pin)>350&&analogRead(X_pin)<450)){//볼륨조절(볼륨 줄이기)
   if(volumNum <=30&&volumNum >1){
      volumNum--;
    }
    else if(volumNum ==  1){
      volumNum = 30;
    }
    mp3_set_volume (volumNum);  //볼륨 설정
  }

  // picture loop
  // LCD출력
  u8g.firstPage();  
  do {
    draw();
  } 
  while( u8g.nextPage() );
  
  delay(500);
}
