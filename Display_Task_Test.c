#include <avr/io.h>
#include "timer.h"
#include "io.c"

#define A7 (~PINA & 0x80)

//global variables
unsigned char end_endgame;

//local variables
unsigned char i;
unsigned char j;

//Testing Code;
unsigned char start_endgame;
unsigned char Led_One_Status;
unsigned char Led_Two_Status;
unsigned short Highscore;
unsigned short score;




enum Display_State{Display_Start, Display_Wait, HighScore_Win, HighScore_Lose, Win_Blink_Off, Win_Blink_On, Lose} Display_State;

void Display_Task(){

	switch (Display_State) {
		case Display_Start:
		Led_One_Status = 0;
		Led_Two_Status = 1;
		start_endgame = 1;
		Highscore = 5;
		score = 7;
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
		else if(start_endgame && (!(Led_One_Status) || !(Led_Two_Status))){
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
			Display_State = Display_Wait;
		}
		break;
		case Lose:
		if(!(A7)){
			Display_State = Lose;
		}
		else if(A7){
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
		LCD_DisplayString(1, "Wait");

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

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();
	TimerSet(100);
	TimerOn();
	//Start_Button_State = Start_Button_Start;
	//Timer_CTRL_State = Timer_Start;
	//Pad_State = Pad_Start;
	Display_State = Display_Start;

	while(1) {
		//Start_Button_Handler_Tick();
		//Timer_CTRL_Tick();
		//Pad_Tick();
		Display_Task();
		//LCD_Cursor(1);
		//LCD_WriteData(score + '0');

		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
	}
}
