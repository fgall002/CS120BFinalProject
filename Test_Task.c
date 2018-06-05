#include <avr/io.h>
#include "io.c"

#define A0 (~PINA & 0x01)

//global variables

unsigned char start;
unsigned char end;

//Enum
enum Start_Button_State{Start_Button_Start, Game_Off, Game_On}Start_Button_State;

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
