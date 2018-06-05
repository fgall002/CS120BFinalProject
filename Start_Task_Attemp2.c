#include <avr/io.h>
#include "io.c"

#define MaxBall 10;

//global variables
static unsigned char game_on;
static unsigned char start_endgame;
static unsigned char end_endgame;
static unsigned short score;

//Local Variables
unsigned short time;
const unsigned short tick_limit = 300 ;
unsigned short Ballcount;
unsigned char Led_One_Status;
unsigned char Led_Two_Status;

enum Timer_CTRL_State{Timer_Start, Wait_Start, Play, Wait_END_Game}Timer_CTRL_State;

void Timer_CTRL_Tick(){
  //Transition States
  switch(Timer_CTRL_State){
    case Timer_Start:
        start_endgame = 0
        Timer_CTRL_State = Wait_Start;

        //Test CODE
        start = 1;
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
        if(time <= tick_limit || Ballcount <= MaxBall){
          Timer_CTRL_State = Play;
        }
        else if(!(time <= tick_limit) && !(Ballcount <= MaxBall)){
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
        if(echo == 1){
          score += 10;
        }
        else if(echo == 2){
          score += 20;
        }
        if(BallScan){
          Ballcount++;
        }
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
  Start_Button_State = Start_Button_Start;

  while(1) {
    Start_Button_Handler_Tick();
    //Pad_Tick();
    LCD_Cursor(1);
    LCD_WriteData(start + '0');
  continue;}
}
