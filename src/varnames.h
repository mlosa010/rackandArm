#ifndef varnames.h
#define varnames.h

int objDumped=0;
char k;
bool fflag = false;
bool dflag = false;
unsigned long startInterval = 0;
unsigned long elapesdTime = 100;
unsigned long grabTime =1000;
unsigned long time = 0;
int speed=0;

enum graberStates {initial, baseOut, RackDown, handOpen, handClosed, rackUp, baseIn,stop};
graberStates grabber = initial;

enum dumpingStates {Inital, BaseOut, HandOpen,BaseIn,pushObj,HandClosed,Stopp};
dumpingStates dumper = Inital;


#endif