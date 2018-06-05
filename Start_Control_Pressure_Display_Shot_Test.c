#include <avr/io.h>
#include "timer.h"
#include "io.c"


//Define
#define A0 (~PINA & 0x01)
#define A1 (~PINA & 0x02)
#define A2 (~PINA & 0x04)
#define A3 (~PINA & 0x08)
#define A7 (~PINA & 0x80)
#define MaxBall 10
#define MAX 80
#define Led_Lights PORTB

//global variables
//Start_Task
unsigned char start;
unsigned char end;

//Timer Control Task
static unsigned char game_on;
static unsigned char start_endgame;
static unsigned short score;

//Presure_Pad Task
unsigned char Pad;

//Display_Task
static unsigned char end_endgame;

//Shot_Task
unsigned char echo;

//Local Variables
//Timer Control Task
unsigned short time;
const unsigned short tick_limit = 100;
//const unsigned short tick_limit = 300;
unsigned short Ballcount;
unsigned char Led_One_Status;
unsigned char Led_Two_Status;

//Display_Task
unsigned char i;
unsigned char j;
unsigned short Highscore;



enum Start_Button_State{Start_Button_Start, Game_Off, Game_On}Start_Button_State;
enum Timer_CTRL_State{Timer_Start, Wait_Start, Play, Wait_END_Game}Timer_CTRL_State;
enum Pad_State{Pad_Start, Wait_Pressure, Down, Hold_Pressure}Pad_State;
enum Display_State{Display_Start, Display_Wait, HighScore_Win, HighScore_Lose, Win_Blink_Off, Win_Blink_On, Lose} Display_State;
enum Shot_Pad_State{Shot_Pad_Start, Wait_Shot, Wait_Echo, LED_ONE_State, LED_TWO_State} Shot_Pad_State;

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
      else if(end){
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
	LCD_DisplayString(1, "GAME OFF");
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
      Led_Lights = 3;
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
			//test
			start=0;
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
		LCD_DisplayString(1, "timer");

		//Testing code
	/*	if(Pad){
			score +=1;
		}*/

    //Testing code
		LCD_Cursor(1);
		LCD_WriteData(score + '0');


		//
		if(echo == 1){
			score += 1;
		}
		else if(echo == 2){
			score += 2;
		}
		/*if(BallScan){
			Ballcount++;
		}*/
		//Finish rest of instruction to count for score;

		break;
		case Wait_END_Game:
		break;
	}
}

void Pad_Tick(){
	//Transition
	switch(Pad_State){
		case Pad_Start:
		Pad = 0;
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
			Pad_State = Wait_Pressure;
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


void Display_Task(){

	switch (Display_State) {
		case Display_Start:
			//tEST CODE
		 Highscore=3;
		Display_State = Display_Wait;
		break;
		case Display_Wait:
		if(!(start_endgame)){
			Display_State =  Display_Wait;
		}
		else if(start_endgame &&  !(Led_One_Status) && !(Led_Two_Status)){
			i = 0;
			j = 0;
			end_endgame = 1;
			Display_State = HighScore_Win;
		}
		else if(start_endgame && (!(Led_One_Status) || !(Led_Two_Status)) || (Led_One_Status && Led_Two_Status)){
			i = 0;
			end_endgame = 1;
			Display_State = HighScore_Lose;
		}
		break;
		case HighScore_Win:
		if(i <= 25){
			Display_State = HighScore_Win;
		}
		else if( i> 25){
			i = 0;
			Display_State = Win_Blink_Off;
		}
		break;
		case HighScore_Lose:
		if(i <= 25){
			Display_State = HighScore_Lose;
		}
		else if( i > 25){
			i= 0;
			Display_State = Lose;
		}
		break;
		case Win_Blink_Off:
		if(i <= 5){
			Display_State = Win_Blink_Off;
		}
		else if( i > 5){
			i= 0;
			j++;
			Display_State = Win_Blink_On;
		}
		break;
		case Win_Blink_On:
		if(i <= 5){
			Display_State = Win_Blink_On;
		}
		else if( i > 5 && j<=5){
			i= 0;
			Display_State = Win_Blink_Off;
		}
		else if( i > 5 && j >5){
			//Testing code
			end = 1;
			start = 0;
			Display_State = Display_Wait;
		}
		break;
		case Lose:
		if(!(A7)){
			Display_State = Lose;
		}
		else if(A7){
			//Testing code
			end = 1;
			start = 0;
			Display_State = Display_Wait;
		}
		break;
		default:
		break;
	}

	switch (Display_State) {
		case Display_Start:
		break;
		case Display_Wait:
		//LCD_DisplayString(1,"HighScore:");
		//LCD_Cursor(12);
		//LCD_WriteData(Highscore);
		//LCD_Cursor(17);
		//LCD_DisplayString(17,"Score:");
		//LCD_Cursor(24);
		//LCD_WriteData(score);
		end_endgame = 0;
		break;
		case HighScore_Win:
		if(score > Highscore){
			Highscore = score;
			LCD_DisplayString(1, "NEW HIGH SCORE");
		}
		i++;
		break;
		case HighScore_Lose:
		if(score > Highscore){
			Highscore = score;
			LCD_DisplayString(1, "NEW HIGH SCORE");
		}
		i++;
		break;
		case Win_Blink_Off:
		i++;
		LCD_ClearScreen();
		break;
		case Win_Blink_On:
		i++;
		LCD_DisplayString(1, "WIN!!! Score: ");
		//LCD_Cursor(16);
		//LCD_WriteData(score + '0');
		break;
		case Lose:
		LCD_DisplayString(1, "YOU  LOST >.< ");
		break;
	}
}

//Shot_Pad_Tick

void Shot_Pad_Tick(){
	switch(Shot_Pad_State){
		case Shot_Pad_Start:
		echo = 0;
		Shot_Pad_State = Wait_Shot;
		break;
		case Wait_Shot:
		if(!(Pad)){
			Shot_Pad_State = Wait_Shot;
		}
		else if(Pad){
			Shot_Pad_State = Wait_Echo;
		}
		else{
			Shot_Pad_State = Wait_Shot;
		}
		break;
		case Wait_Echo:
		if(!(A1) && !(A2)){
			Shot_Pad_State = Wait_Shot;
		}
		else if((A1) && !(A2) && Led_One_Status){
			echo = 2;
			Led_One_Status = 0;
			Shot_Pad_State = LED_ONE_State;
		}
		else if(!(A1) && (A2) && Led_Two_Status){
			echo = 2;
			Led_Two_Status = 0;
			Shot_Pad_State = LED_TWO_State;
		}
		else{
			Shot_Pad_State = Wait_Shot;
		}
		break;
		case LED_ONE_State:
		echo = 0;
		Shot_Pad_State = Wait_Shot;
		break;
		case LED_TWO_State:
		echo = 0;
		Shot_Pad_State = Wait_Shot;
		break;
		default:
		Shot_Pad_State = Shot_Pad_Start;
		break;
	}

  //Actions

	switch(Shot_Pad_State){
		case Shot_Pad_Start:
		break;
		case Wait_Shot:
		if(!(Pad)){
			if((A1) && !(A2) && Led_One_Status){
				echo = 1;
				Led_One_Status = 0;
				if(!(Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 0;
				}
				else if(!(Led_One_Status) && (Led_Two_Status)){
					Led_Lights = 2;
				}
			}
			else if(!(A1) && (A2) && Led_Two_Status){
				echo = 1;
				Led_Two_Status = 0;
				if(!(Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 0;
				}
				else if((Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 1;
				}
			}
		}
		echo = 0;
		break;
		case Wait_Echo:
		break;
		case LED_ONE_State:
		if(!(Led_One_Status) && !(Led_Two_Status)){
			Led_Lights = 0;
		}
		else if(!(Led_One_Status) && (Led_Two_Status)){
			Led_Lights = 2;
		}
		break;
		case LED_TWO_State:
		if(!(Led_One_Status) && !(Led_Two_Status)){
			Led_Lights = 0;
		}
		else if((Led_One_Status) && !(Led_Two_Status)){
			Led_Lights = 1;
		}
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
  Start_Button_State = Start_Button_Start;
	Timer_CTRL_State = Timer_Start;
  Pad_State = Pad_Start;
  Display_State = Display_Start;
  Shot_Pad_State = Shot_Pad_Start;

	while(1) {
		Start_Button_Handler_Tick();
		Timer_CTRL_Tick();
		Pad_Tick();
    Shot_Pad_Tick();
		Display_Task();
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
	}
}
