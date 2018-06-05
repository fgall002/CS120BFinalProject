#include <avr/io.h>
#include "io.c"


#define A3 ~PINA & 0x04

//global variables
unsigned char Pad;


//Testing CODE
unsigned char game_on;

//Enum
enum Pad_State{Pad_Start, Wait_Pressure, Down, Hold_Pressure}Pad_State;

void Pad_Tick(){

  //Transition

  switch(Pad_State){
    case Pad_Start:
        Pad = 0;
        //Testing code
        game_on = 1;
        //
        Pad_State = Wait_Pressure;
      break;
    case Wait_Pressure:
        if(!(game_on) || !(A3)){
          Pad_State= Wait_Pressure;
        }
        else if(game_on && A3){
          Pad_State = Down;
        }
      break;
    case Down:
        Pad = 0;
        Pad_State =Hold_Pressure;
      break;
    case Hold_Pressure:
      if(A3){
        Pad_State = Hold_Pressure;
      }
      else if(!(A3)){
        Pad_State = Wait_Pressure
      }
      break;
    default:
      break;
  }

  //Actions
  switch(Pad_State){
    case Pad_Start:
      break;
    case Wait_Pressure:
      break;
    case Down:
        Pad = 1;
      break;
    case Hold_Pressure:
      break;
    default:
      break;
  }
}



int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();
	TimerSet(100);
	TimerOn();
	Timer_CTRL_State = Timer_Start;

	while(1) {
		//Start_Button_Handler_Tick();
		//Pad_Tick();
		Timer_CTRL_Tick();
		LCD_Cursor(1);
		LCD_WriteData(pad + '0');

		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
	}
}
