/* Sweep through space
the following is a state machine that cycles thorugh the enumerated states to pickup and put down a block
or sphere for sorting in the IEEE SEcon2019 confernece
*/
#include <Arduino.h>
#include <Servo.h>
#define elapesdTime 1000
Servo Rackservo;
Servo Handservo;
Servo Baseservo;
float startInterval = 0;

enum graberStates {initial, baseOut, RackDown, handOpen, handClosed, rackUp, baseIn, stop};
graberStates grabber= initial;



void grabbingRoutine(void){
switch(grabber){
  case initial:
    grabber = baseOut;
    break;
  case baseOut:
  do{
  Baseservo.write(0);
  Rackservo.write(90);
  Handservo.write(90);
}while(bit_is_set(PINC, PC1));
grabber=RackDown;
  break;

  case RackDown:
  do{
  Baseservo.write(90);
  Rackservo.write(180);
  Handservo.write(90);
}while(bit_is_set(PIND, PD3));
grabber=handClosed;
  break;

  case handClosed:
  startInterval=millis();
  if(millis()-startInterval>=elapesdTime){
    grabber=rackUp;
  }else if(millis()-startInterval<elapesdTime){
  Baseservo.write(90);
  Rackservo.write(90);
  Handservo.write(180);
}
break;

  case rackUp:
  do{
  Baseservo.write(90);
  Rackservo.write(0);
  Handservo.write(90);
}while(bit_is_set(PIND, PD2));
grabber=baseIn;

break;

  case baseIn:
  do{
  Baseservo.write(180);
  Rackservo.write(90);
  Handservo.write(90);
}while(bit_is_set(PINC, PC0));
grabber=handOpen;
  break;

  case handOpen:
  startInterval=millis();
  if(millis()-startInterval>=elapesdTime){
  grabber=stop;
  }else{
  Baseservo.write(90);
  Rackservo.write(90);
  Handservo.write(0);
}

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
 Rackservo.attach(11);  // attaches the servo on pin 9 to the servo object
 Handservo.attach(9);
 Baseservo.attach(10);

 //this sets a timer interrupt in timer 2 independent of timer 1 which is on
 //pins 9,10 so that i can have a forth servo
 TCCR2A =0;
 TCCR2B &= ~(1<<CS20);
 TCCR2B |= ((1<<CS21)|(1<<CS21));
 TCNT2 = 0;
 OCR2A = 125;
 OCR2B = 125;
 TIMSK2 |=(1<<OCIE2A)|(1<<OCIE2B);


//sets internal pullup to D3, and D2 for using in
//bit is set in SM
 DDRD &= ~((1<<PD2)|(1<<PD3));
 PORTD |= ((1<<PD2)|(1<<PD3));
 //sets internal pullup to A0, and A1 for using in
 //bit is set in SM
 DDRC &= ~((1<< PC0)|(1<<PC1));
 PORTC |= ((1 << PC0)|(1<<PC1));

 //old code for more interrupts
 //DDRB |= (1<<PB5)|(1<<PB4);
 // DDRD |= (1<<PD5)|(1<<PD6);
 //EIMSK |= ((1<<INT1)|(1<<INT0));
 //EICRA &= ~((1<<ISC10)|(1<<ISC00));
 //EICRA |= ((1<<ISC11)|(1<<ISC01));

 // setting up pin A0,A1 to be used as a PCINTERRUPT
 //for some reason i cant check the bit_is_set in the state machine
 //without this bit
  PCMSK1 |= (1<<PC1);
  PCICR |= (1<<PCIE1);
  sei();
while(1) {
grabbingRoutine();
Serial.println(grabber);
}
return(0);
}

//old code with interrupts
/*
ISR(PCINT1_vect){
  Bflag=!Bflag;
  PORTB ^= (1<<PB5);
}
ISR(INT0_vect){
  Tflag=!Tflag;
  PORTD ^= (1<<PD6);
}
ISR(INT1_vect){
  Hflag=!Hflag;
PORTD ^= (1<<PD5);
}
*/

// psudo PWM generation for adding another motor
ISR(TIMER2_COMPA_vect){
  TCNT2 = 0;
  PORTB ^= (1<<PB5);
}
ISR(TIMER2_COMPB_vect){
  TCNT2 = 0;
PORTB ^= (1<<PB4);
}
