#include <avr/io.h>
#include "timer.h"
#include "io.c"
#define A4 (~PINA & 0x10)

//Global variables
unsigned char BallScan;

enum Ball_Sensor_State{Ball_Sensor_Start, Wait_Sensor, Sensor_Count, Sensor_Hold} Ball_Sensor_State;


void Ball_Sensor_Tick(){
  switch (Ball_Sensor_State) {
    case Ball_Sensor_Start:
        BallScan= 0;
      break;
    case Wait_Sensor:
        if(!(game_on) || !(A4)){
          Ball_Sensor_State = Wait_Sensor;
        }
        else if(game_on && A4){
          Ball_Sensor_State = Sensor_Count;
        }
        else{
          Ball_Sensor_State = Wait_Sensor;
        }
      break;
    case Sensor_Count:
        BallScan = 0;
        Ball_Sensor_State = Sensor_Hold;
      break;
    case Sensor_Hold:
        if(A4){
          Ball_Sensor_State = Sensor_Hold;
        }
        else if(!(A4)){
          Ball_Sensor_State = Wait_Sensor;
        }
        else{
          Ball_Sensor_State = Sensor_Hold;
        }
      break;
    default:
        Ball_Sensor_State = Ball_Sensor_Start;
      break;
  }
  switch (Ball_Sensor_State) {
    case Ball_Sensor_Start:
      break;
    case Wait_Sensor:
      break;
    case Sensor_Count:
        BallScan = 1;
      break;
    case Sensor_Hold:
      break;
  }
}


int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
  DDRB = 0xFF; PORTB = 0x00;

	LCD_init();
	TimerSet(100);
	TimerOn();
	//Start_Button_State = Start_Button_Start;
	//Timer_CTRL_State = Timer_Start;
	//Pad_State = Pad_Start;
	Ball_Sensor_State = Ball_Sensor_Start;

	while(1) {
    Ball_Sensor_Tick();
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
	}
}
