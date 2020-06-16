#include <Arduino.h>
#include <Servo.h>
#include "varnames.h"
Servo Rackservo;
Servo Handservo;
Servo BaseservoL;
Servo BaseservoR;
Servo Dumpservo;


void setup() {
  Serial.begin(9600);              

    DDRD &= ~((1<<PD2)|(1<<PD3));
    DDRD |= ((1<<PD4)|(1<<PD5));
    PORTD &= ~((1<<PD4)|(1<<PD5));
    PORTD |= ((1<<PD2)|(1<<PD3));
    DDRC &= ~((1<< PC0)|(1<<PC1)|(1<<PC2));
    PORTC |= ((1 << PC0)|(1<<PC1)|(1<<PC2));
}
void resetState()
{
  grabber = initial;
  dumper = Inital;
  fflag=false;
  dflag=false;
  k='i';
}

void loop() {
  Serial.println(grabber);
  Serial.print(dumper);
  Serial.print("c \n");

  if(Serial.available()){
    k=Serial.read();
    Serial.end();
  }
  if(fflag || dflag)
    resetState();

  if(k == 'p')
    grabbingRoutine();

  if(k=='d')
    takeADump();
}

/////////////////////define inline functions used in rutines//////////
//for those who dont know inline functions are simple code repacements like #defines
//but they look like functions
inline void BASEOUTE(){
  BaseservoR.attach(5);
  BaseservoL.attach(11);
  do {
    BaseservoR.write(60);
    BaseservoL.write(120);
  } while (bit_is_set(PINC, PC0));
  BaseservoL.detach();
  BaseservoR.detach();
  grabber = RackDown;
}
inline void RACKDOWN(){
  Rackservo.attach(10);
  do {
    Rackservo.writeMicroseconds(2400);
  } while (bit_is_set(PINC, PC2));
  Rackservo.detach();
  startInterval = millis();
  grabber = handClosed;
}
inline void HANDCLOSED(){
  Handservo.attach(9);
  startInterval = millis();
  do {
    Handservo.writeMicroseconds(1000);
  } while (millis() - startInterval < grabTime);
  //Handservo.detach();
  grabber = rackUp;
}
inline void RACKUP(){
  Rackservo.attach(10);
  do {
    Rackservo.writeMicroseconds(500);//rack up
  } while (bit_is_set(PIND, PD2));
  Rackservo.detach();

  grabber = stop;
}
inline void BASEIN(){
  BaseservoR.attach(5);
  BaseservoL.attach(11);
  do {
    BaseservoR.write(180);
    BaseservoL.write(0);
  } while (bit_is_set(PINC, PC1));
  //Handservo.detach();
  BaseservoL.detach();
  BaseservoR.detach();
  grabber = handOpen;
}
inline void HANDOPEN(){
  Handservo.attach(9);
  startInterval = millis();
  do {
    Handservo.writeMicroseconds(2000);
  }while(millis() - startInterval < elapesdTime);
  Handservo.detach();
  grabber = stop;
}
inline void STOP(){
  BaseservoL.detach();
  Rackservo.detach();
  Handservo.detach();
  fflag = true;
}

inline void STOPD(){
  //BaseservoL.detach();
  //Rackservo.detach();
  //Handservo.detach();
  Dumpservo.attach(11);
  startInterval = millis();
  do{
    Dumpservo.write(0);
  }while(millis() - startInterval <elapesdTime);
  Dumpservo.detach();
  objDumped=0;
  dflag = true;
}

inline void DROPOBJ(){
  Handservo.attach(9);
  startInterval = millis();
  do {
    Handservo.write(180);
  }while(millis() - startInterval < elapesdTime);
  objDumped++;
  Handservo.detach();
  dumper = BaseIn;
}

inline void PUSHOBJ(){
  Dumpservo.attach(6);
  startInterval = millis();
  do{
    Dumpservo.write(180);
  }while(millis() - startInterval < elapesdTime);
  Dumpservo.detach();
  dumper = HandClosed;
}

inline void BASEIND(){
  BaseservoR.attach(5);
  BaseservoL.attach(11);
  do {
    BaseservoR.write(180);
    BaseservoL.write(0);
  } while (bit_is_set(PINC, PC1));
  BaseservoR.detach();
  BaseservoL.detach();
  //if(objDumped==1){
  //  dumper =  pushObj;
  //}else if (objDumped==2){
  dumper = Stopp;
//}
}
inline void BASEOUTD(){
  BaseservoR.attach(5);
  BaseservoL.attach(11);
  do {
    BaseservoR.write(60);
    BaseservoL.write(120);
  } while (bit_is_set(PINC, PC0));
  BaseservoL.detach();
  BaseservoR.detach();
  dumper = HandOpen;

}
inline void HANDCLOSEDD(){
  Handservo.attach(9);
    do {
      Handservo.write(70);
    } while (millis() - startInterval < elapesdTime);
    //Handservo.detach();
    dumper = BaseOut;
}

//////////////////////define routines////////////////////////
void grabbingRoutine() {
  switch (grabber) {
    case initial:
    Serial.begin(9600);
    grabber = baseOut;
    break;
    case baseOut:
      BASEOUTE();
      break;
    case RackDown:
      RACKDOWN();
      break;
    case handClosed:
      HANDCLOSED();
      break;
    case rackUp:
      RACKUP();
      break;
    case baseIn:
      BASEIN();
      break;
    case handOpen:
      HANDOPEN();
      break;
    case stop:
      STOP();
      break;
    default:
      STOP();
      break;

  }
}

void takeADump(){
switch (dumper) {
  case Inital:
   Serial.begin(9600);
   dumper=HandOpen;
   break;

  case HandOpen:
  DROPOBJ();
  break;

  case BaseIn:
  BASEIND();
  break;

  case HandClosed:
  HANDCLOSEDD();
  break;

  case BaseOut:
  BASEOUTD();
  break;

  case pushObj:
  PUSHOBJ();
  break;

  case Stopp:
  STOPD();
  dflag = true;
  break;

  default:
  STOPD();
  break;
}
}
