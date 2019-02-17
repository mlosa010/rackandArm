/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/
#include <Arduino.h>
#include <Servo.h>

Servo Rackservo;
Servo Handservo;
Servo Baseservo;
volatile bool I0 = 1;
volatile bool I1 = 1;
volatile bool I2 = 1;
volatile bool I3 = 0;

enum graberStates {initial, baseOut, RackDown, handOpen, handClosed, rackUp, baseIn, stop};
graberStates grabber= initial;

void grabbingRoutine(void){
switch(grabber){
  case initial:
    grabber = baseOut;
    break;
  case baseOut:
    if(bit_is_set(PORTB, PB5))
    grabber=baseOut;

    if(bit_is_clear(PORTB, PB5)){
    grabber=RackDown;
  }else
  grabber=stop;

  break;

  case RackDown:
  if(bit_is_set(PORTD, PD5))
  grabber= RackDown;

  if(bit_is_clear(PORTD, PD5)){
  grabber=handClosed;
}else
grabber=stop;
  break;

  case handClosed:

  if(bit_is_set(PORTD, PD6))
  grabber=rackUp;
  break;
/*
  case rackUp:

  if(!I0&&!I1&&I2)
  grabber=rackUp;

  if(!I0&&I1&&!I2)
  grabber=baseIn;

  break;

  case baseIn:

  if(I0&&I1&&!I2)
  grabber=baseIn;

  if(!I0&&I1&&!I2)
  grabber=baseIn;

  if(I3)
  grabber=handOpen;

  break;

  case handOpen:

  if(!A3&&A0&&A1&&!A2)
  grabber=stop;

  break;

*/
  default:
  grabber = stop;
}
switch (grabber) {
  case baseOut:
  Baseservo.write(0);
  Rackservo.write(90);
  Handservo.write(90);
  break;

  case RackDown:
  Baseservo.write(90);
  Rackservo.write(180);
  Handservo.write(90);
  break;

  case handClosed:
  Baseservo.write(90);
  Rackservo.write(90);
  Handservo.write(0);
  break;

  case rackUp:
  Baseservo.write(90);
  Rackservo.write(0);
  Handservo.write(90);
  break;

  case baseIn:
  Baseservo.write(180);
  Rackservo.write(90);
  Handservo.write(90);
  break;

  case handOpen:
  Baseservo.write(90);
  Rackservo.write(90);
  Handservo.write(180);

  case stop:
  Baseservo.write(90);
  Rackservo.write(90);
  Handservo.write(90);
  break;

  default:
  Baseservo.write(90);
  Rackservo.write(90);
  Handservo.write(90);
  break;

}

}
int main(void) {
  Serial.begin(9600);              //  setup serial
  Rackservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Handservo.attach(11);
  Baseservo.attach(10);
  DDRD &= ~(1<<PD3);
  PORTD |= (1<<PD3);
pinMode(2,INPUT_PULLUP);
 DDRD |= (1<<PD4)|(1<<PD5)|(1<<PD6);
 DDRB |= (1<<PB5);
 grabber = initial;
 DDRC &= ~((1<< PC0)|(1<<PC1));
 PORTC |= ((1 << PC0)|(1<<PC1));
 //attachInterrupt(0, pin2, FALLING);
 EIMSK |= ((1<<INT1)|(1<<INT0));
 EICRA &= ~((1<<ISC10)|(1<<ISC00));
 EICRA |= ((1<<ISC11)|(1<<ISC01));

 // setting up pin A0,A1 to be used as a PCINTERRUPT
  PCMSK1 |= ((1<<PCINT8)|(1<<PC1));
  PCICR |= (1<<PCIE1);
  sei();



while(1) {
grabbingRoutine();
Serial.println(grabber);
}
return(0);
}

ISR(PCINT1_vect){
  //if(bit_is_clear(PORTC, PC0))
  //PORTD ^= (1<<PD4);//A0=!A0;

//  if(bit_is_clear(PORTC, PC1))
  PORTB ^= (1<<PB5);
  I0=!I0;

}
ISR(INT0_vect){
  PORTD ^= (1<<PD5);
  I1=!I1;
}
ISR(INT1_vect){
  I2=!I2;
  PORTD ^= (1<<PD6);
}
