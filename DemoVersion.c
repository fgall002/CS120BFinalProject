#include <avr/io.h>
#include <avr/eeprom.h>
#include "timer.h"
#include "io.c"
#include "scheduler.h"


void LCD_CreateCharacter(unsigned char pixel[8], unsigned char reg){
	if(reg <8)
		LCD_WriteCommand(0x40+ (reg * 8));
		for(int i = 0; i < 8; i++){
				LCD_WriteData(pixel[i]);
		}
		LCD_WriteCommand(0x80);
}

//Define
#define A0 (~PINA & 0x01)
#define A1 (~PINA & 0x02)
#define A2 (~PINA & 0x04)
#define A3 (~PINA & 0x08)
#define A4 (~PINA & 0x10)
#define A7 (~PINA & 0x80)
#define MaxBall 10
#define MAX 80
#define Led_Lights PORTB

//global variables
unsigned char Character1[8] = { 0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00 };

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

//Ball_Scan
unsigned char BallScan;

//Local Variables
//Timer Control Task
unsigned short time;
unsigned char current_time;
//const unsigned short tick_limit = 100;
const unsigned short tick_limit = 300;
unsigned short Ballcount;
unsigned char Led_One_Status;
unsigned char Led_Two_Status;

//Display_Task
unsigned char i;
unsigned char j;
unsigned short Highscore;

//Shot_Task
unsigned char echo_count;
const unsigned char echo_limit = 300;



enum Start_Button_State{Start_Button_Start, Game_Off, Game_On}Start_Button_State;
enum Timer_CTRL_State{Timer_Start, Wait_Start, Play, Wait_END_Game}Timer_CTRL_State;
enum Pad_State{Pad_Start, Wait_Pressure, Down, Hold_Pressure}Pad_State;
enum Display_State{Display_Start, Display_Wait, HighScore_Win, HighScore_Lose, Win_Blink_Off, Win_Blink_On, Lose} Display_State;
enum Shot_Pad_State{Shot_Pad_Start, Wait_Shot, Wait_Echo, LED_ONE_State, LED_TWO_State} Shot_Pad_State;
enum Ball_Sensor_State{Ball_Sensor_Start, Wait_Sensor, Sensor_Count, Sensor_Hold} Ball_Sensor_State;

//Tick Function

int Start_Button_Handler_Tick(int state){

  //Transition
  switch(Start_Button_State){
    case Start_Button_Start:
        start = 0;
        end = 1;
		Highscore = eeprom_read_word((uint16_t*)5);
        Start_Button_State = Game_Off;
      break;
    case Game_Off:
	LCD_CreateCharacter(Character1,0);
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
	LCD_ClearScreen();
	LCD_Cursor(1);
	LCD_WriteData(0);
	LCD_DisplayString(5, "GAME OFF");
	LCD_Cursor(16);
	LCD_WriteData(0);
      break;
    case Game_On:
      break;
    default:
      break;
  }
	return state;
}

//Start_Tick

int Timer_CTRL_Tick(int state){
	//Transition States
	switch(Timer_CTRL_State){
		case Timer_Start:
		start_endgame = 0;
		//UCOOMENT THIS LINE TO SET HIGHSCORE TO ZERO RIGHTNOW
/*		eeprom_write_word((uint16_t*)5, (uint16_t)0);*/
		Highscore = eeprom_read_word((uint16_t*)5);
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
			current_time = 30;
			Led_Lights = 3;
			Timer_CTRL_State = Play;
		}
		break;
		case Play:
		if(time <= tick_limit && Ballcount <= MaxBall && Led_One_Status && Led_Two_Status){
			Timer_CTRL_State = Play;
		}
		else if((time > tick_limit) || (Ballcount > MaxBall) || (!(Led_One_Status) && !(Led_Two_Status))){
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


// 		if(BallScan){
// 			Ballcount++;
// 		}
		//Finish rest of instruction to count for score;
		//Testing code
		LCD_ClearScreen();
		LCD_Cursor(1);
		LCD_DisplayString(1,"HS:");
		LCD_Cursor(5);
		LCD_WriteData(eeprom_read_word((uint16_t*)5) + '0');
		LCD_DisplayString(8, "Score:");
		LCD_Cursor(15);
		LCD_WriteData(score + '0');
		if(time % 10 == 0){
			current_time--;
		}
		LCD_DisplayString(17,"Time:");
		if(current_time <=9){
			LCD_Cursor(22);
			LCD_WriteData(0+ '0');
			LCD_Cursor(23);
			LCD_WriteData(current_time + '0');
		}
		else if(current_time > 9){
			LCD_Cursor(22);
			LCD_WriteData(current_time /10 + '0');
			LCD_Cursor(23);
			LCD_WriteData(current_time % 10 + '0');
		}
		LCD_DisplayString(25, "Balls:");
			if((MaxBall - Ballcount) <=9){
				LCD_Cursor(31);
				LCD_WriteData(0+ '0');
				LCD_Cursor(31);
				LCD_WriteData((MaxBall - Ballcount) + '0');
			}
			else if((MaxBall - Ballcount) > 9){
				LCD_Cursor(31);
				LCD_WriteData((MaxBall - Ballcount) /10 + '0');
				LCD_Cursor(32);
				LCD_WriteData((MaxBall - Ballcount) % 10 + '0');
		}
		break;
		case Wait_END_Game:
		break;
	}
}

//Timer_Tick

int Pad_Tick( int state){
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
		LCD_Cursor(1);
		LCD_WriteData(Pad + '0');
		break;
		case Hold_Pressure:
		break;
		default:
		break;
	}
	return state;
}

//Display_Tick

int Display_Task(int state){

	switch (Display_State) {
		case Display_Start:
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
		if(i<=25){
			Display_State = Lose;
		}
		else if(i>25){
			end = 1;
			start = 0;
			i = 0;
			//Test
			//eeprom_write_word((uint16_t*)0, (uint16_t)0);

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
			//Highscore = score;
			eeprom_write_word((uint16_t*)5, (uint16_t)score);
			LCD_ClearScreen();
			LCD_DisplayString(1, "NEW HIGH SCORE");
			LCD_Cursor(16);
			LCD_WriteData(eeprom_read_word((uint16_t*)5) + '0');
		}
		i++;
		break;
		case HighScore_Lose:
		if(score > Highscore){
			//Highscore = score;
			LCD_ClearScreen();
			eeprom_write_word((uint16_t*)5, (uint16_t)score);
			LCD_DisplayString(1, "NEW HIGH SCORE");
			LCD_Cursor(16);
			LCD_WriteData(eeprom_read_word((uint16_t*)5) + '0');
		}
		i++;
		break;
		case Win_Blink_Off:
		i++;
		LCD_ClearScreen();
		break;
		case Win_Blink_On:
		i++;
		LCD_ClearScreen();
		LCD_DisplayString(1, "WIN!!! Score: ");
		LCD_Cursor(16);
		LCD_WriteData(score + '0');
		break;
		case Lose:
		i++;
		LCD_ClearScreen();
		LCD_DisplayString(1, "YOU  LOST >.< ");
		break;
	}
	return state;
}

//Shot_Pad_Tick

int Shot_Pad_Tick(int state){
	switch(Shot_Pad_State){
		case Shot_Pad_Start:
				echo = 0;
				echo_count = 0;
				Shot_Pad_State = Wait_Shot;
				break;
		case Wait_Shot:
				//LCD_DisplayString(1, "WAit shot");
				echo = 0;
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
				//LCD_DisplayString(1, "WaitEcho");
				if(!(A1) && !(A2) && echo_count > echo_limit){
					echo_count = 0;
					Shot_Pad_State = Wait_Shot;
				}
				else if((A1) && !(A2) && Led_One_Status){
					echo = 2;
					//keep this
					score +=2;
					//
					Led_One_Status = 0;
					Shot_Pad_State = LED_ONE_State;
				}
				else if(!(A1) && (A2) && Led_Two_Status){
					echo = 2;
					//keep this
					score +=2;
					//
					Led_Two_Status = 0;
					Shot_Pad_State = LED_TWO_State;
				}
				else if((!(A1) && !(A2) && echo_count <= echo_limit)){
					Shot_Pad_State = Wait_Echo;
				}
				break;
		case LED_ONE_State:
				//if(echo_count > echo_limit){
				echo = 0;
				//}
				Shot_Pad_State = Wait_Shot;
				break;
				case LED_TWO_State:
				//if(echo_count > echo_limit){
				echo = 0;
				//}
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
				//keep this
				score +=1;
				//
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
				//keep this
				score +=1;
				//
				Led_Two_Status = 0;
				if(!(Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 0;
				}
				else if((Led_One_Status) && !(Led_Two_Status)){
					Led_Lights = 1;
				}
			}
		}
		break;
		case Wait_Echo:
				echo_count++;
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
	return state;
}


int Ball_Sensor_Tick(int state){
	switch (Ball_Sensor_State) {
		case Ball_Sensor_Start:
		BallScan= 0;
		Ball_Sensor_State = Wait_Sensor;
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
		Ballcount++;
		break;
		case Sensor_Hold:
		break;
	}
	return state;
}






int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;






	// Period for the tasks
	unsigned long int Start_Button_Handler_Tick_calc = 50;
	unsigned long int Timer_CTRL_Tick_calc = 100;
	unsigned long int Pad_Tick_calc = 30;
	unsigned long int Display_Task_calc = 100;
	unsigned long int Shot_Pad_Tick_calc = 30;
	unsigned long int Ball_Sensor_Tick_cal = 50;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(Start_Button_Handler_Tick_calc, Timer_CTRL_Tick_calc);
	tmpGCD = findGCD(tmpGCD, Pad_Tick_calc);
	tmpGCD = findGCD(tmpGCD, Display_Task_calc);
	tmpGCD = findGCD(tmpGCD, Shot_Pad_Tick_calc);
	tmpGCD = findGCD(tmpGCD, Ball_Sensor_Tick_cal);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;



	//Recalculate GCD periods for scheduler
	unsigned long int Start_Button_Handler_Tick_period = Start_Button_Handler_Tick_calc/GCD;
	unsigned long int Timer_CTRL_Tick_period = Timer_CTRL_Tick_calc/GCD;
	unsigned long int Pad_Tick_period = Pad_Tick_calc/GCD;
	unsigned long int Display_Task_period = Display_Task_calc/GCD;
	unsigned long int Shot_Pad_Tick_period = Shot_Pad_Tick_calc/GCD;
	unsigned long int Ball_Sensor_Tick_Period = Ball_Sensor_Tick_cal/GCD;

	//Declare an array of tasks
	static task task1, task2, task3, task4, task5, task6;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5,&task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);


	/*Start_Button_State = Start_Button_Start;
	Timer_CTRL_State = Timer_Start;
	Pad_State = Pad_Start;
	Display_State = Display_Start;
	Shot_Pad_State = Shot_Pad_Start;*/


	// Task 1
	task1.state = Start_Button_Start;//Task initial state.
	task1.period = Start_Button_Handler_Tick_period;//Task Period.
	task1.elapsedTime = Start_Button_Handler_Tick_period;//Task current elapsed time.
	task1.TickFct = &Start_Button_Handler_Tick;//Function pointer for the tick.

	// Task 2
	task2.state = Timer_Start;//Task initial state.
	task2.period = Timer_CTRL_Tick_period;//Task Period.
	task2.elapsedTime = Timer_CTRL_Tick_period;//Task current elapsed time.
	task2.TickFct = &Timer_CTRL_Tick;//Function pointer for the tick.

	// Task 3
	task3.state = Pad_Start;//Task initial state.
	task3.period = Pad_Tick_period;//Task Period.
	task3.elapsedTime = Pad_Tick_period; // Task current elasped time.
	task3.TickFct = &Pad_Tick; // Function pointer for the tick.

	// Task 4
	task4.state =  Display_Start;//Task initial state.
	task4.period = Display_Task_period;//Task Period.
	task4.elapsedTime = Display_Task_period; // Task current elasped time.
	task4.TickFct = &Display_Task; // Function pointer for the tick.

	// Task 5
	task5.state = Shot_Pad_Start;//Task initial state.
	task5.period = Shot_Pad_Tick_period;//Task Period.
	task5.elapsedTime = Shot_Pad_Tick_period; // Task current elasped time.
	task5.TickFct = &Shot_Pad_Tick; // Function pointer for the tick.

	// task6

	task6.state = Ball_Sensor_Start;//Task initial state.
	task6.period = Ball_Sensor_Tick_Period;//Task Period.
	task6.elapsedTime = Ball_Sensor_Tick_Period; // Task current elasped time.
	task6.TickFct = &Ball_Sensor_Tick; // Function pointer for the tick.


	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	LCD_init();

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}

	// Error: Program should not exit!
	return 0;


}
