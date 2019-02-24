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

enum graberStates {initial, baseOut, RackDown, handOpen, handClosed, rackUp, baseIn,stop};
graberStates grabber = initial;

enum dumpingStates {Inital, BaseOut, HandOpen,BaseIn,pushObj,HandClosed,Stopp};
dumpingStates dumper = Inital;
void grabbingRoutine() {
  switch (grabber) {
    case initial:
    Serial.begin(9600);
      grabber = baseOut;
      break;
    case baseOut:
      do {
        BaseservoR.write(180);
        BaseservoL.write(0);//move forward
        Rackservo.write(90);//still
        Handservo.write(90);//still
      } while (bit_is_set(PINC, PC1));
      grabber = RackDown;
      break;

    case RackDown:
      do {
        PORTD |= (1<<PD4);//digitalWrite(4,HIGH);
        BaseservoR.write(90);
        BaseservoL.write(90);//still
        Rackservo.write(180);//rack down
        Handservo.write(90);//still
      } while (bit_is_set(PIND, PD3));
      startInterval = millis();
      grabber = handClosed;
      break;

    case handClosed:
      do {
        PORTD &= ~(1<<PD4);
        BaseservoL.write(90);
        BaseservoR.write(90);
        Rackservo.write(90);
        Handservo.write(0);
      } while (millis() - startInterval < elapesdTime);
      grabber = rackUp;
      break;
    case rackUp:
      do {
        PORTD |= (1<<PD4);
        BaseservoL.write(90);
        BaseservoR.write(90);//still
        Rackservo.write(0);//rack up
        Handservo.write(90);//still
      } while (bit_is_set(PIND, PD2));
      grabber = baseIn;

      break;

    case baseIn:
      do {
        PORTD &= ~(1<<PD4);
        BaseservoR.write(0);
        BaseservoL.write(180);
        Rackservo.write(90);
        Handservo.write(90);
      } while (bit_is_set(PINC, PC0));
      grabber = handOpen;
      break;

    case handOpen:
      startInterval = millis();
      do {
        BaseservoR.write(90);
        BaseservoL.write(90);
        Rackservo.write(90);
        Handservo.write(180);
      }while(millis() - startInterval < elapesdTime);
      grabber = stop;
      break;

    case stop:
      BaseservoR.write(90);
      BaseservoL.write(90);
      Rackservo.write(90);
      Handservo.write(90);
      fflag = true;
      break;

    default:
      BaseservoR.write(90);
      BaseservoL.write(90);
      Rackservo.write(90);
      Handservo.write(90);
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



void setup() {
  Serial.begin(9600);              //  setup serial
  Rackservo.attach(11);  // attaches the servo on pin 9 to the servo object
  Handservo.attach(9);
  BaseservoL.attach(10);
  BaseservoR.attach(6);
  Dumpservo.attach(5);

    DDRD &= ~((1<<PD2)|(1<<PD3));//pinMode(2,3INPUTS);
    DDRD |= (1<<PD4);
    PORTD &= ~(1<<PD4);
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
  //break();
}

void loop() {
  Serial.println(grabber);
  Serial.print(dumper);

  if(Serial.available()){
    k=Serial.read();
    Serial.end();
  }
  if(fflag || dflag)
  {
    resetState();
  }
  if(k == 'p'){
    //Serial.end();
    //grabber=initial;
    grabbingRoutine();


  }
  if(k=='d'){

    takeADump();
  }
}


//////////////////////define routines////////////////////////
