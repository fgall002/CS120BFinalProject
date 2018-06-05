#include <avr/io.h>
#include "timer.h"
#include "io.c"

//#define A1 (~ PINA & 0x02)
//define A2 (~ PINA & 0x04)
#define Led_Lights PORTB
#define MAX 80

//global variables
unsigned char echo;
unsigned char Led_One_Status;
unsigned char Led_Two_Status;

//local variables
unsigned short A1 = 0;
unsigned short A2 = 0;

//Testing CODE
unsigned char Pad;
#define A1 (~PINA & 0x02)
#define A2 (~PINA & 0x04)


enum Shot_Pad_State{Shot_Pad_Start, Wait_Shot, Wait_Echo, LED_ONE_State, LED_TWO_State} Shot_Pad_State;




void Shot_Pad_Tick(){
	switch(Shot_Pad_State){
		case Shot_Pad_Start:
		echo = 0;
		Led_Lights = 3;
		Pad =0;
		//    A1 = ADC;
		//  A2 = ADC;
		Led_One_Status= 1;
		Led_Two_Status = 1;
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
			echo += 2;
			Led_One_Status = 0;
			Shot_Pad_State = LED_ONE_State;
		}
		else if(!(A1) && (A2) && Led_Two_Status){
			echo += 2;
			Led_Two_Status = 0;
			Shot_Pad_State = LED_TWO_State;
		}
		else{
			Shot_Pad_State = Wait_Shot;
		}
		break;
		case LED_ONE_State:
		echo += 0;
		Shot_Pad_State = Wait_Shot;
		break;
		case LED_TWO_State:
		echo += 0;
		Shot_Pad_State = Wait_Shot;
		break;
		default:
		Shot_Pad_State = Shot_Pad_Start;
		break;
	}



	switch(Shot_Pad_State){
		case Shot_Pad_Start:
		break;
		case Wait_Shot:
		if(!(Pad)){
			if((A1) && !(A2) && Led_One_Status){
				echo += 1;
				Led_One_Status = 0;
				if(!(Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 0;
				}
				else if(!(Led_One_Status) && (Led_Two_Status)){
					Led_Lights = 2;
				}
			}
			else if(!(A1) && (A2) && Led_Two_Status){
				echo += 1;
				Led_Two_Status = 0;
				if(!(Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 0;
				}
				else if((Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 1;
				}
			}
		}
		echo += 0;
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
	//Start_Button_State = Start_Button_Start;
	//Timer_CTRL_State = Timer_Start;
	//  Pad_State = Pad_Start;
	//Display_State = Display_Start;
	Shot_Pad_State = Shot_Pad_Start;


	while(1) {
		//	Start_Button_Handler_Tick();
		//	Timer_CTRL_Tick();
		//	Pad_Tick();
		//	Display_Task();
		Shot_Pad_Tick();
		LCD_Cursor(1);
		LCD_WriteData(echo + '0');
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
	}
}
