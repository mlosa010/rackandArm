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
