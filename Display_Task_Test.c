#define A7 (~PINA & 0x80)

//global variables
unsigned char end_endgame;

//local variables
unsigned char i;
unsigned char j;


enum Display_State{Display_Start, Display_Wait, HighScore_Win, HighScore_Lose, Win_Blink_Off, Win_Blink_On, Lose} Display_State;

void Display_Task(){

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
        else if(start_endgame && (!(Led_One_Status) || !(Led_Two_Status))){
          i = 0;
          end_endgame = 1;
          Display_State = HighScore_Lose;
        }
      break;
    case HighScore_Win:
        if(i <= 5){
          Display_State = HighScore_Win;
        }
        else if( i> 5){
          i = 0;
          Display_State = Win_Blink_Off;
        }
      break;
    case HighScore_Lose:
        if(i <= 5){
          Display_State = HighScore_Lose;
        }
        else if( i > 5){
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
          j++
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
        LCD_DisplayString(1,"HighScore:");
        LCD_Cursor(12)
        LCD_WriteData(Highscore);
        LCD_Cursor(17)
        LCD_DisplayString(17,"Score:");
        LCD_Cursor(24);
        LCD_WriteData(score);

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
        LCD_ClearScreen(void);
      break;
    case Win_Blink_On:
        i++;
        LCD_DisplayString(1, "WIN!!! Score: ");
        LCD_Cursor(16);
        LCD_WriteData(score + '0');
      break;
    case Lose:
        LCD_DisplayString(1, "YOU  LOST >.< ");
      break;
  }
}
