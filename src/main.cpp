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
unsigned long elapesdTime = 2000;
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
void grabbingRoutine();
void takeADump();

void setup() {
  Serial.begin(9600);              //  setup serial
  Rackservo.attach(11,500,2400);  // attaches the servo on pin 9 to the servo object
  Handservo.attach(9);
  BaseservoL.attach(10);
  Dumpservo.attach(5);
  Rackservo.writeMicroseconds(600);

    DDRD &= ~((1<<PD2)|(1<<PD3));
    DDRD |= ((1<<PD4)|(1<<PD5));
    PORTD &= ~((1<<PD4)|(1<<PD5));
    PORTD |= ((1<<PD2)|(1<<PD3));
    DDRC &= ~((1<< PC0)|(1<<PC1));
    PORTC |= ((1 << PC0)|(1<<PC1));
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
  do {
    BaseservoL.write(180);
    Rackservo.writeMicroseconds(600);
    Handservo.write(90);
  } while (bit_is_set(PINC, PC1));
  grabber = RackDown;
}
inline void RACKDOWN(){
  do {
    PORTD |= (1<<PD4);
    BaseservoL.write(90);
    Rackservo.writeMicroseconds(2400);
    Handservo.write(90);
  } while (bit_is_set(PIND, PD3));
  startInterval = millis();
  PORTD &= ~(1<<PD4);
  grabber = handClosed;
}
inline void HANDCLOSED(){
  do {
    BaseservoL.write(90);
    Rackservo.writeMicroseconds(600);
    Handservo.write(0);
  } while (millis() - startInterval < elapesdTime);
  grabber = rackUp;
}
inline void RACKUP(){
  do {
    PORTD |= (1<<PD4);
    BaseservoL.write(90);
    Rackservo.writeMicroseconds(0);//rack up
    Handservo.write(90);//still
  } while (bit_is_set(PIND, PD2));
  PORTD &= ~(1<<PD4);
  grabber = baseIn;
}
inline void BASEIN(){
  PORTD &= ~(1<<PD4);
  do {
    BaseservoL.write(0);
    Rackservo.writeMicroseconds(600);
    Handservo.write(90);
  } while (bit_is_set(PINC, PC0));
  grabber = handOpen;
}
inline void HANDOPEN(){
  startInterval = millis();
  do {
    BaseservoL.write(90);
    Rackservo.writeMicroseconds(600);
    Handservo.write(180);
  }while(millis() - startInterval < elapesdTime);
  grabber = stop;
}
inline void STOP(){
  BaseservoL.write(90);
  Rackservo.writeMicroseconds(600);
  Handservo.write(90);
  fflag = true;
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
   dumper=BaseOut;
   break;

  case BaseOut:
  do {
    BaseservoR.write(180);
    BaseservoL.write(0);//move forward
    Rackservo.write(90);//still
    Handservo.write(90);//still
  } while (bit_is_set(PINC, PC1));
  dumper = HandOpen;
  break;

  case HandOpen:
  startInterval = millis();
  do {
    BaseservoR.write(90);
    BaseservoL.write(90);
    Rackservo.write(90);
    Handservo.write(180);
  }while(millis() - startInterval < elapesdTime);
  objDumped++;
  dumper = BaseIn;
  break;

  case BaseIn:
  do {
    PORTD &= ~(1<<PD4);
    BaseservoR.write(0);
    BaseservoL.write(180);
    Rackservo.write(90);
    Handservo.write(90);
  } while (bit_is_set(PINC, PC0));
  if(objDumped==3){
    dumper = Stopp;
  }else
  dumper = pushObj;
  break;

  case pushObj:
  startInterval = millis();
  do{
    Dumpservo.write(0);
    BaseservoR.write(90);
    BaseservoL.write(90);
    Rackservo.write(90);
    Handservo.write(90);
  }while(millis() - startInterval < elapesdTime);
  dumper = HandClosed;
  break;

  case HandClosed:
    do {
      PORTD &= ~(1<<PD4);
      Dumpservo.write(180);
      BaseservoL.write(90);
      BaseservoR.write(90);
      Rackservo.write(90);
      Handservo.write(0);
    } while (millis() - startInterval < elapesdTime);
    dumper = BaseOut;
    break;

    case Stopp:
      Dumpservo.write(90);
      BaseservoR.write(90);
      BaseservoL.write(90);
      Rackservo.write(90);
      Handservo.write(90);
      dflag = true;
      break;

      default:
        Dumpservo.write(90);
        BaseservoR.write(90);
        BaseservoL.write(90);
        Rackservo.write(90);
        Handservo.write(90);
        break;
}
}
