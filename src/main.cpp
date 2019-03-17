#include <Arduino.h>
#include <Servo.h>
Servo Rackservo;
Servo Handservo;
Servo BaseservoL;
Servo BaseservoR;
Servo Dumpservo;
int objDumped=0;
char k;
bool fflag = false;
bool dflag = false;
unsigned long startInterval = 0;
unsigned long elapesdTime = 250;
unsigned long grabTime =1000;
unsigned long time = 0;
int speed=0;

enum graberStates {initial, baseOut, RackDown, handOpen, handClosed, rackUp, baseIn,stop};
graberStates grabber = initial;

enum dumpingStates {Inital, BaseOut, HandOpen,BaseIn,pushObj,HandClosed,Stopp};
dumpingStates dumper = Inital;

inline void BASEOUTE();
inline void RACKDOWN();
inline void HANDCLOSED();
inline void RACKUP();
inline void BASEIN();
inline void HANDOPEN();
inline void STOP();
inline void DROPOBJ();
inline void PUSHOBJ();
inline void RETRACTPUSHER();
inline void BASEIND();
inline void BASEOUTD();
void grabbingRoutine();
void takeADump();

void setup() {
  Serial.begin(9600);              //  setup serial
  BaseservoR.attach(6);
  BaseservoR.detach();


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
  BaseservoL.attach(6);
  do {
    BaseservoR.write(60);
    BaseservoL.write(120);
  } while (bit_is_set(PINC, PC1));
  BaseservoL.detach();
  BaseservoR.detach();
  grabber = RackDown;
}
inline void RACKDOWN(){
  Rackservo.attach(10);
  do {
    Rackservo.writeMicroseconds(2000);
  } while (bit_is_set(PINC, PC2));
  Rackservo.detach();
  startInterval = millis();
  grabber = handClosed;
}
inline void HANDCLOSED(){
  Handservo.attach(9);
  do {
    Handservo.write(170);
  } while (millis() - startInterval < grabTime);
  //Handservo.detach();
  grabber = rackUp;
}
inline void RACKUP(){
  Rackservo.attach(10);
  do {
    Handservo.write(170);
    Rackservo.writeMicroseconds(500);//rack up
  } while (bit_is_set(PIND, PD2));
  Rackservo.detach();

  grabber = baseIn;
}
inline void BASEIN(){
  BaseservoR.attach(5);
  BaseservoL.attach(6);
  do {
    BaseservoR.write(180);
    BaseservoL.write(0);
  } while (bit_is_set(PINC, PC0));
  Handservo.detach();
  BaseservoL.detach();
  Handservo.detach();
  grabber = handOpen;
}
inline void HANDOPEN(){
  Handservo.attach(9);
  startInterval = millis();
  do {
    Handservo.write(70);
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
  BaseservoL.detach();
  Rackservo.detach();
  Handservo.detach();
  Dumpservo.attach(10);
  startInterval = millis();
  do{
    Dumpservo.write(0);
  }while(millis() - startInterval < elapesdTime);
  Dumpservo.detach();
  objDumped=0;
  dflag = true;
}

inline void DROPOBJ(){
  Handservo.attach(9);
  startInterval = millis();
  do {
    Handservo.write(70);
  }while(millis() - startInterval < elapesdTime);
  objDumped++;
  Handservo.detach();
  dumper = BaseIn;
}

inline void PUSHOBJ(){
  Dumpservo.attach(11);
  startInterval = millis();
  do{
    Dumpservo.write(180);
  }while(millis() - startInterval < elapesdTime);
  Dumpservo.detach();
  dumper = HandClosed;
}

inline void BASEIND(){
  BaseservoR.attach(5);
  BaseservoL.attach(6);
  do {
    BaseservoR.write(180);
    BaseservoL.write(0);
  } while (bit_is_set(PINC, PC0));
  BaseservoR.detach();
  BaseservoL.detach();
  if(objDumped==2){
    dumper =  pushObj;
  }else if (objDumped==1){
  dumper = HandClosed;
}else
  dumper=Stopp;
}
inline void BASEOUTD(){
  BaseservoR.attach(5);
  BaseservoL.attach(6);
  do {
    BaseservoR.write(60);
    BaseservoL.write(120);
  } while (bit_is_set(PINC, PC1));
  BaseservoL.detach();
  BaseservoR.detach();
  dumper = HandOpen;

}
inline void HANDCLOSEDD(){
  Handservo.attach(9);
    do {
      Handservo.write(180);
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
