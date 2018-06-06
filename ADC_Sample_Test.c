#include <avr/io.h>
#define OutputB PortB
void ACD_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

unsigned short my_short;
#define max 80
unsigned char my_char;
//unsigned char OutputB;


enum States{Start, Off, On} state;

void SM(){
	switch(state){
		case Start:
		state = Off;
		break;
		case Off:
		state = On;
		break;
		case On:
		my_short = ADC;
		state = 0n;
		break;
		default:
		state = Off;
		break;
	}
	switch(state){
		case Start:
		OutputB = 0;
		break;
		case Off:
		OutputB = 0;
		break;
		case On;
	    my_char = (char)my_short;
		//test with my_char or my_short
		if(my_char >= max/2){
			OutputB = 0x01;
		}
		else if(my_char < max/2){
			OutputB = 0x00;
		}
		break;
		default:
		break;
	}
}

int main(void){
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0x00;
	//DDRC = 0x00; PORTC = 0x00
	OutputB = 0;
	state = Start;
	ADC_init();
	while(1){
		SM();
	}

}
