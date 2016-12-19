#include "U8glib.h" //LCD라이브러리
#include <SoftwareSerial.h>//시리얼 통신 라이브러리
#include <DFPlayer_Mini_Mp3.h>//mp3 모듈 라이브러리
 
U8GLIB_NHD_C12864 u8g(13, 11, 10, 9, 8);  // SPI Com: SCK = 13, MOSI = 11, CS = 10, CD = 9, RST = 8(LCD)
const int trig = 3;           // 변수 trig를 생성하고 3 대입한다(초음파)
const int echo = 2;          // 변수 echo를 생성하고 2 대입한다(초음파)
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
#define HUMI_PIN A2 // 토양 습도 센서
#define CONTROL_PIN 5// 릴레이

#define musictimespring 219000//봄의 노래 길이
#define musictimesummer 201000//여름의 노래 길이
#define musictimewinter 216000//겨울의 노래 길이
#define CHECK_INTERVAL 10000 //흙의 습도를 측정하는 간격
#define AUTO_STOP_INTERVAL 5000 //물 펌프가 동작하는 시간
#define HUMIDITY_THRESHOLD 250 //흙의 습도가 몇일때 동작할지

int waterfind;      //물탱크 안의 물 확인(0=물의 양 충분, 1 = 부족, 2=센서 에러)
int volumNum = 10; //소리의 크기(시작볼륨)
int waterpump = 0; //물탱크(0=허용안됨, 1=허용)
int music = 0; //음악 실행(0=끔, 1=킴)
int menuchoice = 1;//메뉴 선택( 1=물펌프, 2=음악, 3=소리, 4=음악선택)
int menu = 0;//메뉴 화면(0=기본 메뉴, 1=음악메뉴)
int musicplay = 0; //현재 선택된 곡(1=순차재생, 2=봄 반복, 3=여름반복, 4=겨울반복)
int playingmusic = 0; //현재 재생되고 있는 곡(1=봄, 2=여름, 3=겨울)
int isValveOn = 0;//펌프가 작동시 1, 작동안할시 0
unsigned long prevMusicTime = 0; //음악 실행시작 시간 체크
unsigned long prevReadTime = 0; //습도 측정 시간 체크
unsigned long prevValveTime = 0; //펌프 가동 시간 체크

void draw(void) {

  char charvolumNum[2]; // 볼륨을 출력하기 위해 char형 변수 선언
  u8g.setFont(u8g_font_4x6); //폰트 및 글자 크기 설정
  if(waterfind==0 ){//물의 양 LCD에출력
    u8g.drawStr( 3, 10, "   Water : enough");
  }
  else if(waterfind ==1){
     u8g.drawStr( 3, 10, "   Water : lack");
  }
  else if(waterfind ==2){
     u8g.drawStr( 3, 10, "   Water : error");
  }
  
  if(menu ==0){//기본 화면
    if(waterpump == 0 ){
       u8g.drawStr( 3, 20, "   Water Pump : off ");
    }
    else if(waterpump == 1 ){
      u8g.drawStr( 3, 20, "   Water Pump : on ");
    }
    if( music == 0){
      u8g.drawStr( 3, 30, "   Music : off ");
    }
   else if(music == 1){
      u8g.drawStr( 3, 30, "   Music : on ");
    }
   dtostrf(volumNum, 3, 0, charvolumNum);//volumNum을 char형으로 바꿈
   u8g.drawStr( 3, 40, "   Sound : ");
   u8g.drawStr( 45, 40, charvolumNum);
   u8g.drawStr( 3, 50, "   Music List  ");
  }
  
  if(menu == 1){//음악 리스트 화면
      u8g.drawStr( 3, 20, "   Order Play");
    if(playingmusic == 1){
      u8g.drawStr( 3, 30, "   The Four Seasons - Spring *");
    }
    else{
      u8g.drawStr( 3, 30, "   The Four Seasons - Spring");
    }
    if(playingmusic == 2){
      u8g.drawStr( 3, 40, "   The Four Seasons - Summer *");
    }
    else{
      u8g.drawStr( 3, 40, "   The Four Seasons - Summer");
    }
    if(playingmusic == 3){
     u8g.drawStr( 3, 50, "   The Four Seasons - Winter *");
    }
    else{
      u8g.drawStr( 3, 50, "   The Four Seasons - Winter");
    }
  }
  
  if(menuchoice == 1){// 선택 출력
    u8g.drawStr( 0, 20, ">");
  }
  else if(menuchoice == 2){
    u8g.drawStr( 0, 30, ">");
  }
  else if(menuchoice == 3){
    u8g.drawStr( 0, 40, ">");
  }
  else{
    u8g.drawStr( 0, 50, ">");
  }

}

void setup(void) {
  
  Serial.begin (9600);
  mp3_set_serial (Serial);   //set softwareSerial for DFPlayer-mini mp3 module 
  delay(1);                     // delay 1ms to set volume
  mp3_set_volume (volumNum);          // value 0~30볼륨설정
  
  pinMode(trig, OUTPUT);  // trig핀(3)을 출력모드로 설정한다
  pinMode(echo, INPUT); // echo핀(2)을 입력모드로 설정한다
  pinMode(CONTROL_PIN, OUTPUT); //CONTROL_PIN을 출력핀으로 설정
  digitalWrite(CONTROL_PIN, LOW);//CONTROL_PIN을 0볼트로,펌프핀

  //LCD 기본 설정
  u8g.setContrast(0);
  u8g.setRot180();
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

   //펌프
   // turn off the water pump,물펌프 끄기, millis-아두이노가 실행되고 난 후 경과한 시간
  if(isValveOn > 0 && millis() - prevValveTime > AUTO_STOP_INTERVAL) {//펌프가 작동하고, 펌프동작시간이후
    digitalWrite(CONTROL_PIN, LOW);//CONTROL_PIN을 내린다.,물펌프 끄기
    isValveOn = 0;//펌프작동안함
    Serial.println("Stop pumping...");
  }
  
  //------------------------------------------------------
  //----- Check humidity info습도채크
  //------------------------------------------------------
  if(millis() - prevReadTime > CHECK_INTERVAL) {//습도측정 간격 채크
    int humi = analogRead(HUMI_PIN);//값을 읽고 디지털 값으로 변환,습도
    prevReadTime = millis();//습도체크 시간 재설정
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.println(" %");

    // send data to server
    if(humi < HUMIDITY_THRESHOLD && waterpump == 1 && waterfind ==0) {//습도가 지정한 습도보다 낮을때,물펌프동작을 허용할때,물탱크에 물의 양이 충분할때
      digitalWrite(CONTROL_PIN, HIGH);//CONTROL_PIN을 올린다.,물펌프 키기
      prevValveTime = millis();//펌프 가동 시간 재설정
      isValveOn = 1;//펌프작동
      Serial.println("Start pumping...");
    }
  }

  //LCD 실드의 조이스틱
  if((analogRead(X_pin)>800&&analogRead(X_pin)<850)&&(analogRead(Y_pin)>700&&analogRead(X_pin)<850)){//위
    if(menuchoice>=2 && menuchoice<=4){
       menuchoice--;
    }
  }
  if((analogRead(X_pin)>350&&analogRead(X_pin)<450)&&(analogRead(Y_pin)>350&&analogRead(X_pin)<450)){//아래
     if(menuchoice>=1 && menuchoice<=3){
       menuchoice++;
     }
  }
  if((analogRead(X_pin)==0)&&(analogRead(X_pin)<150)){//좌
    if(menu == 0){//기본 메뉴
     if(menuchoice == 1){
        waterpump = 0;
      }
      else if(menuchoice == 2){
        music = 0;
        if(musicplay >= 1){
          musicplay = 0;
          playingmusic = 0;
          mp3_stop();
        }
     }
      else if(menuchoice == 3){
       if(volumNum>0){
         volumNum--;
         mp3_set_volume (volumNum);
        }
     }
    }
    else if(menu == 1){//음악 리스트 메뉴
      menu = 0;
      menuchoice = 4;
    }
  }
  if((analogRead(X_pin)>=550&&analogRead(X_pin)<650)&&(analogRead(Y_pin)>550&&analogRead(X_pin)<650)){//우
    if(menu == 0){//기본 매뉴
      if(menuchoice == 1){
        waterpump = 1;
      }
      else if(menuchoice == 2){
      music = 1;
      }
      else if(menuchoice == 3){
        if(volumNum<30){
         volumNum++;
         mp3_set_volume (volumNum);
       }
      }
      else if(menuchoice == 4){
        menu = 1;
        menuchoice = 1;
      }
    }
    else if(menu == 1){//음악 리스트 메뉴
      if(menuchoice == 1 && music == 1){
        playingmusic = 1;
        mp3_play (playingmusic);
        musicplay = 1;
        prevMusicTime = millis();
      }
      else if(menuchoice == 2 && music == 1){
        playingmusic = 1;
        mp3_play (playingmusic);
        musicplay = 2;
        prevMusicTime = millis();
      }
      else if(menuchoice == 3 && music == 1){
        playingmusic = 2;
        mp3_play (playingmusic);
        musicplay = 3;
        prevMusicTime = millis();
      }
      else if(menuchoice == 4 && music == 1){
        playingmusic = 3;
        mp3_play (playingmusic);
        musicplay = 4;
        prevMusicTime = millis();
      }
    }
  }
  
  if(musicplay == 1){//음악 반복 재생(순차재생의 경우)
    if(playingmusic == 1){
      if(music==1&&millis() -  prevMusicTime > musictimespring){
      prevMusicTime = millis();
      playingmusic++;
      mp3_play (playingmusic);
      }
    }
    else if(playingmusic == 2){
      if(music==1&&millis() -  prevMusicTime > musictimesummer){
      prevMusicTime = millis();
      playingmusic++;
      mp3_play (playingmusic);
      }
    }
    else if(playingmusic == 3){
      if(music==1&&millis() -  prevMusicTime > musictimewinter){
      prevMusicTime = millis();
      playingmusic = 1;
      mp3_play (playingmusic);
      }
    }
  }
  else if(musicplay >= 2){//봄곡 반복의 경우
    if(music==1&&millis() -  prevMusicTime > musictimespring){
      prevMusicTime = millis();
      mp3_play (playingmusic);
    }
  }
  else if(musicplay == 3){//여름곡 반복의 경우
    if(music==1&&millis() -  prevMusicTime > musictimesummer){
      prevMusicTime = millis();
      mp3_play (playingmusic);
    }
  }
  else if(musicplay == 4){//겨울 곡 반복의 경우
    if(music==1&&millis() -  prevMusicTime > musictimewinter){
      prevMusicTime = millis();
      mp3_play (playingmusic);
    }
  }

  

  // picture loop
  // LCD출력
  u8g.firstPage();  
  do {
    draw();
  } 
  while( u8g.nextPage() );
  
  delay(100);
}
