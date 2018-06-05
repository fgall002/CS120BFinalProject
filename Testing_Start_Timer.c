#include <avr/io.h>
#include "timer.h"
#include "io.c"


//Define
#define A0 (~PINA & 0x01)
#define MaxBall 10

//global variables
//Start_Task
unsigned char start;
unsigned char end;

//Timer Control Task
static unsigned char game_on;
static unsigned char start_endgame;
static unsigned char end_endgame;
static unsigned short score;

//Local Variables
//Timer Control Task
unsigned short time;
const unsigned short tick_limit = 300;
unsigned short Ballcount;
unsigned char Led_One_Status;
unsigned char Led_Two_Status;

enum Start_Button_State{Start_Button_Start, Game_Off, Game_On}Start_Button_State;
enum Timer_CTRL_State{Timer_Start, Wait_Start, Play, Wait_END_Game}Timer_CTRL_State;

//Tick Function

void Start_Button_Handler_Tick(){

  //Transition
  switch(Start_Button_State){
    case Start_Button_Start:
        start = 0;
        end = 1;
        Start_Button_State = Game_Off;
      break;
    case Game_Off:
        if(!(A0) && !(start) && end){
          Start_Button_State = Game_Off;
        }
        else if(A0 && !(start) && end){
          start = 1;
          end = 0;
          Start_Button_State = Game_On;
        }
      break;
    case Game_On:
      if(!(end) && start){
        Start_Button_State = Game_On;
      }
      else if(end && start){
        end = 1;
        start = 0;
        Start_Button_State = Game_Off;
      }
      break;
    default:
        Start_Button_State = Start_Button_Start;
      break;
  }

  //Action States
  switch(Start_Button_State){
    case Start_Button_Start:
      break;
    case Game_Off:
      break;
    case Game_On:
      break;
    default:
      break;
  }

}

void Timer_CTRL_Tick(){
	//Transition States
	switch(Timer_CTRL_State){
		case Timer_Start:
		start_endgame = 0;
		Timer_CTRL_State = Wait_Start;

		break;
		case Wait_Start:
		if(!(start)){
			Timer_CTRL_State = Wait_Start;
		}
		else if(start){
			time = 0;
			score = 0;
			game_on = 1;
			Led_One_Status = 1;
			Led_Two_Status = 1;
			Ballcount = 0;
			Timer_CTRL_State = Play;
		}
		break;
		case Play:
		if(time <= tick_limit && Ballcount <= MaxBall){
			Timer_CTRL_State = Play;
		}
		else if((time > tick_limit) || (Ballcount > MaxBall)){
			start_endgame = 1;
			game_on = 0;
			Timer_CTRL_State = Wait_END_Game;
		}
		break;
		case Wait_END_Game:
		if(!(end_endgame)){
			Timer_CTRL_State = Wait_END_Game;
		}
		else if(end_endgame){
			start_endgame = 0;
			Timer_CTRL_State = Wait_Start;
		}
		break;
		default:
		Timer_CTRL_State = Timer_Start;
		break;
	}

	//Actions
	switch(Timer_CTRL_State){
		case Timer_Start:
		break;
		case Wait_Start:
		break;
		case Play:
		time++;
		/*if(echo == 1){
			score += 10;
		}
		else if(echo == 2){
			score += 20;
		}
		if(BallScan){
			Ballcount++;
		}*/
		//Finish rest of instruction to count for score;

		break;
		case Wait_END_Game:
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
  Start_Button_State = Start_Button_Start;
	Timer_CTRL_State = Timer_Start;

	while(1) {
		Start_Button_Handler_Tick();
		//Pad_Tick();
		Timer_CTRL_Tick();
		LCD_Cursor(1);
		LCD_WriteData(game_on + '0');

		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
	}
}
