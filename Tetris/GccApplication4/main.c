/*
 * LED_LUT.c
 *
 * Created: 3/3/2017 8:09:43 PM
 * Author : Paul Alvarez
 */ 
#include <avr/io.h>
#include <io.c>
#include <timer.h>
#include <time.h>
#include "usart_ATmega1284.h"
char col[8]={0,1,1,1,1,1,1,1};
char upc[8]={0,1,1,1,1,1,1,1};	
char row[9][8]={
	{0,0,0,0,0,0,0,0},//0
	{0,0,0,0,0,0,0,0},//1 empty 
	{0,0,0,0,0,0,0,0},//2 bottom
	{0,0,0,0,0,0,0,0},//3rows downward
	{0,0,0,0,0,0,0,0},//4
	{0,0,0,0,0,0,0,0},//5
	{0,0,0,0,0,0,0,0},//6
	{0,0,0,0,0,0,0,0},//7
	{0,0,0,0,0,0,0,0}  ////8
};
char output[9][8]={
	{0,0,0,0,0,0,0,0},// empty
	{0,0,0,0,0,0,0,0},//l bottom
	{0,0,0,0,0,0,0,0},//rows downward
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0} //r
};
char data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char data2[8]={0,0,0,0,0,0,0,0};
char data3[8]={0,1,0,0,1,0,1,0};
#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
//#include <usart.h>

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------
unsigned char SM2_output = 0x00;
unsigned char SM3_output = 0x00;
unsigned short adcvalue = 0x0000;
unsigned char temp =0x00;
unsigned char button =0x00;
unsigned char rotbutton=0x00;
unsigned char  count=0x06;
unsigned char pselection=0x02;
struct piece block;
unsigned char part1row=7;
unsigned char part2row=6;
unsigned char column1=4;
unsigned char column2=3;
unsigned char rowfull=0;
unsigned char rowcnt=0;
unsigned char delr=0;
unsigned char receive=0x00;
unsigned char score=0x00;
unsigned char cnt=0x00;
unsigned char str[3];
unsigned char upr=0x00;
unsigned char lwr=0x00;
unsigned char upl=0x00;
unsigned char lwl=0x00;
unsigned char toprowcnt=0x00;
unsigned char endg=0x00;
char message[8]="";
//--------End Shared Variables------------------------------------------------
void transmit_data(unsigned short data)
{
	for (int i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x02;
		// set SER = next bit of data to be sent.
		//PORTB |= ((data >> i) & 0x01);
		temp= (data>>i)& 0x0001;
		//temp= array[i] & 0x0001;
		PORTB |= temp <<5;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x80;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x01;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}

void transmit_data2(unsigned short data)
{
	for (int i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x02;
		// set SER = next bit of data to be sent.
		//PORTB |= ((data >> i) & 0x01);
		temp= (data>>i)& 0x0001;
		//temp= array[i] & 0x0001;
		PORTB |= temp <<4;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x08;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}
struct  piece{
	char part1[2];
	char part2[2];
	char part3[2];
		
	};
// random choses a  tetris shape
struct piece randpiece()
{
	struct piece prandom;
	if (pselection==4)
	{
		pselection=0;
	}
	  if (pselection==0)
	  {
		  prandom.part1[0]=0;prandom.part1[1]=0;
		  prandom.part2[0]=1;prandom.part2[1]=0;
		  prandom.part3[0]=1;prandom.part3[1]=1;
	  }
	  else if (pselection==1)
	  {
		  prandom.part1[0]=0;prandom.part1[1]=0;
		  prandom.part2[0]=1;prandom.part2[1]=1;
		  prandom.part3[0]=0;prandom.part3[1]=0;
	  }
	  else if (pselection==2)
	  {
		  prandom.part1[0]=0;prandom.part1[1]=0;
		  prandom.part2[0]=1;prandom.part2[1]=1;
		  prandom.part3[0]=1;prandom.part3[1]=1;
	  }
	  else if (pselection==3)
	  {
		  prandom.part1[0]=0;prandom.part1[1]=0;
		  prandom.part2[0]=1;prandom.part2[1]=0;
		  prandom.part3[0]=0;prandom.part3[1]=1;
	  }
	 pselection++;
	 return prandom;
}
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}
//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum Movement_States {SM_Start,init,insert,movedown,left,right,hd,remov,rotate,gameov};

 //checks movement and changes position if movement
int SMTick1(int state) {

	// Local Variables
	//unsigned char press = ~PINA & 0x01;
	//State machine transitions
	switch (state) {
	
	case SM_Start: 	
	state = init;
	break;

	case init:	
	if (GetBit(button,0)!=1)
	{
		if (USART_IsSendReady(1))
		{
			USART_Send(0xFF,1);
			state = insert;
		}
	}
	else if (USART_HasReceived(1)) {
		receive = USART_Receive(1);
		
		if (GetBit(receive,0)!=1)
		{
			state=insert;
		}
		USART_Flush(1);
	}
	//state = insert;
	break;
	case remov:
	state=insert;
	break;
	
	case insert:
	if (rowfull==1)
	{
		state=remov;
	}
	else
	state=movedown;
	break;
	
	case movedown:
	if (adcvalue>=0&&adcvalue<200)
		{
			state=left;
		}
		else if (adcvalue<=999&&adcvalue>550)
		{
			state=right;
		}
		else if (endg==1)
		{
			state=gameov;
		}
		else if (count==7)
		{
			state=insert;
		}
		else if(GetBit(rotbutton,0)!=1)
		{
			state=rotate;
		}
		if (USART_HasReceived(1)) {
			state=gameov;
		}
		break;
	
	case left:
		if (adcvalue>=50&&adcvalue<200)
		{
			state=left;
		}
		else
		{
			state=movedown;
		}
	break;
	
	case right:
		if (adcvalue<=999&&adcvalue>600&&adcvalue==0)
		{
			state=right;
		}
		else{
			state=movedown;
		}
	break;
	
	case rotate:
    state=movedown;
	break;
	
	case  gameov:
	if (GetBit(button,0)!=1)
	{
		state=init;
	}
	else
	state=gameov;
	break;
	
	default:		
	state = SM_Start; // default: Initial state
	break;
	}

	//State machine actions
	switch(state) {
	case SM_Start:	
	break;

	case init:	
	LCD_DisplayString(1, "Press Button to start");
	endg=0;
	for (int i=0;i<9;i++)
	{
		for (int j=0;j<8;j++)
		{
			row[i][j]=output[i][j];
		}
	}
		for (int j=0;j<8;j++)
		{
			row[0][j]=output[0][j];
		}
    	for (int j=0;j<8;j++)
		{
			row[8][j]=output[8][j];
		}
	
	count=0;
	break;
	
	case remov:
	rowfull=0;
	//removes filled row then moves all rows down one
	for(int i=0;i<8;i++){
		row[delr][i]=0;
	}
	for (int i=delr;i<7;i++)
	{
		for (int j=0;j<8;j++)
		{
			row[i][j]=row[i+1][j];
		}
	}
	cnt++;
	score=cnt+'0';
	str[0]=score;
	str[1]='\0';
	LCD_DisplayString(1, str);
	break;
	
	case insert:
	  block=randpiece();
		  part2row=7;
		  part1row=8;
		  //set to variable to rotate parts later
		upr=block.part2[0];
		upl=block.part3[0];
		lwr=block.part2[1];
		lwl=block.part3[1];
	   row[part1row][column1]=upr;
	   row[part1row][column2]=upl;
	   row[part2row][column1]=lwr;
	   row[part2row][column2]=lwl;
	   count=0;
	   score=cnt+'0';
	   str[0]=score;
	   str[1]='\0';
	//   LCD_DisplayString(1, str);
	break;
	case movedown:
	 if (row[part2row][column1]==0&&row[part2row-1][column1]==1&& row[part2row-1][column2]==0&&row[part2row][column2]==1)//check lower right is empty
	 {
		 if (part2row==0)
		 {
			 count=7;
		 }
		 else
		 {
			 count++;
		 }
	row[part1row-1][column2]=upl;
	 row[part2row-1][column2]=lwl;
	 row[part1row-1][column1]=upr;
	 row[part1row][column1]=0;
	 row[part1row][column2]=0;
	 part2row--;
	 part1row--;
	 }
	 else if (row[part2row][column1]==1&&row[part2row-1][column1]==0&& row[part2row-1][column2]==1&&row[part2row][column2]==0)//check if lower left is empty and continues moving
	 {
		  if (part2row==0)
		  {
			  count=7;
		  }
		  else
		  {
			  count++;
		  }
		 row[part2row-1][column1]=lwr;
		 row[part1row-1][column1]=upr;
		 row[part1row-1][column2]=upl;
		 row[part1row][column1]=0;
		 row[part1row][column2]=0;
		 part2row--;
		 part1row--;
		
		 count++;
	 }
	  else if (row[part2row][column1]==0&&row[part2row-1][column1]==1&& row[part2row-1][column2]==1&&row[part2row][column2]==0)//check if both bottom is empty and continues moving
	  {
		  if (part2row==0)
		  {
			  count=7;
		  }
		  else
		  {
			  count++;
		  }
		  row[part2row-1][column1]=lwr;
		  row[part2row-1][column2]=lwl;
		 row[part1row-1][column1]=upr;
		  row[part1row-1][column2]=upl;
		  row[part1row][column1]=0;
		  row[part1row][column2]=0;
		  part2row--;
		  part1row--;
		  
		  count++;
	  }
	 else if (row[part2row-1][column1]==0&& row[part2row-1][column2]==0) // if below is empty movedown
	 {
		 if (part2row==0)
		 {
			 count=7;
		 }
		 else
		{
		 count++;
		}
		 row[part2row-1][column1]=lwr;
		 row[part2row-1][column2]=lwl;
		 
		 row[part1row-1][column1]=upr;
		 row[part1row-1][column2]=upl;
		 row[part1row][column1]=0;
		 row[part1row][column2]=0;
		 part2row--;
		 part1row--;
		 
	 }
	 else 
	 {
		 count=7;
	}
	break;
	
	case left:
	if (row[part2row][column2]==0&&row[part2row][column2-1]==1&&row[part1row][column2]==1&&row[part1row][column2-1]==0) //checks whats on lower left then moves if clear if not then stays on
	{																													//0,1 
		if (column2!=0)																									//1,0
		{
	column1--;
	column2--;
	row[part1row][column2]=upl;
	row[part1row][column1]=upr;
	row[part2row][column1]=lwr;
	 row[part2row][column1+1]=0;
	 row[part1row][column1+1]=0;
		}
	}
	else if (row[part2row][column2]==1&&row[part2row][column2-1]==0&&row[part1row][column2]==0&&row[part1row][column2-1]==1)//checks upper right empty
	{																														//  1,0
		if (column2!=0)																										//  0,1
		{
			column1--;
			column2--;
			row[part1row][column2]=upr;
			row[part2row][column2]=lwl;
			row[part2row][column1]=lwr;
			row[part2row][column1+1]=0;
			row[part1row][column1+1]=0;
		}
	}
	else if (row[part2row][column2]==0&&row[part2row][column2-1]==0&&row[part1row][column2]==1&&row[part1row][column2-1]==0)
	{																												//0,1
		if (column2!=0)																								//0,0
		{
			column1--;
			column2--;
			row[part1row][column2]=upl;
			row[part2row][column2]=lwl;
			row[part1row][column1]=upr;
			row[part2row][column1]=lwr;
			row[part2row][column1+1]=0;
			row[part1row][column1+1]=0;
		}
	}
	else if (row[part2row][column2]==0&&row[part2row][column2-1]==0&&row[part1row][column2]==1&&row[part1row][column2-1]==0&&row[part1row][column1]==1)
	{																												//0,1,1
		if (column2!=0)																								//0,0,0
		{
			column1--;
			column2--;
			row[part1row][column2]=upl;
			row[part2row][column2]=lwl;
			row[part1row][column1]=upr;
			row[part2row][column1+1]=0;
			row[part1row][column1+1]=0;
		}
	}
	else if (row[part2row][column2]==1&&row[part2row][column2-1]==0&&row[part1row][column2]==0&&row[part1row][column2-1]==0)
	{																												//0,0
		if (column2!=0)																								//0,1
		{
			column1--;
			column2--;
			row[part1row][column2]=upl;
			row[part2row][column2]=lwl;
			row[part1row][column1]=upr;
			row[part2row][column1]=lwr;
			row[part2row][column1+1]=0;
			row[part1row][column1+1]=0;
		}
	}
	else if (row[part2row][column2]==0&&row[part2row][column2-1]==0&&row[part1row][column2]==0&&row[part1row][column2-1]==0&&row[part1row][column1]==1&&row[part2row][column1]==1)
		{																									       //0,0,1
			if (column2!=0)																						   //0,0,1
			{
				column1--;
				column2--;
				
				row[part1row][column1]=block.part2[0];
				row[part2row][column1]=block.part2[1];
				row[part2row][column1+1]=0;
				row[part1row][column1+1]=0;
				
			}
			else 
			{
				row[part1row][column2]=block.part2[0];row[part1row][column1]=block.part3[0];
				row[part2row][column2]=block.part2[1];row[part2row][column1]=block.part3[1];
			}
		}
	else if (row[part2row][column2]==0&&row[part2row][column2-1]==0&&row[part1row][column2]==0&&row[part1row][column2-1]==0)
	{																											//0,0
		if (column2!=0)																							//0,0
		{
			column1--;
			column2--;
			row[part1row][column2]=block.part2[0];
			row[part2row][column2]=block.part3[1];
			row[part1row][column1]=block.part2[0];
			row[part2row][column1]=block.part2[0];
			row[part2row][column1+1]=0;
			row[part1row][column1+1]=0;
		}
	}
	else if (row[part2row][column2]==1&&row[part2row][column2-1]==0&&row[part1row][column2]==1&&row[part1row][column2-1]==0)
	{																									       //0,1			
		if (column2!=0)																						   //0,1
		{
			column1--;
			column2--;
			row[part1row][column2]=block.part2[0];
			row[part2row][column2]=block.part3[1];
			row[part1row][column1]=block.part2[0];
			row[part2row][column1]=block.part2[1];
			row[part2row][column1+1]=0;
			row[part1row][column1+1]=0;
		}
	}

	break;
	
	case right:
	if (row[part2row][column1]==1&&row[part2row][column1+1]==0&&row[part1row][column1]==1&&row[part1row][column1+1]==0)
	{																										//1,0
	if (column1!=7)																							//1,0
	{
	column1++;
	column2++;
	row[part2row][column2-1]=0;
	row[part1row][column2-1]=0;
	row[part1row][column1]=block.part2[0];
	row[part2row][column1]=block.part2[0];
	row[part1row][column2]=block.part2[0];
	row[part2row][column2]=block.part3[1];
	
	}
	}
	else if (row[part2row][column1]==0&&row[part2row][column1+1]==0&&row[part1row][column1]==0&&row[part1row][column1+1]==0)
	{																										//0,0
		if (column1!=7)																					    //0,0
		{
			column1++;
			column2++;
			row[part2row][column2-1]=0;
			row[part1row][column2-1]=0;
			row[part1row][column1]=block.part2[0];
			row[part2row][column1]=block.part2[0];
			row[part1row][column2]=block.part2[0];
			row[part2row][column2]=block.part3[1];
			
		}
	}
	else if (row[part2row][column1]==1&&row[part2row][column1+1]==0&&row[part1row][column1]==0&&row[part1row][column1+1]==1)
	{																										//0,1
		if (column1!=7)																						//1,0
		{
			column1++;
			column2++;
			row[part2row][column2-1]=0;
			row[part1row][column2-1]=0;
			row[part1row][column1]=block.part2[0];
			row[part2row][column1]=block.part2[0];
			row[part1row][column2]=block.part2[0];
			row[part2row][column2]=block.part3[1];
			
		}
	}
	else if (row[part2row][column1]==0&&row[part2row][column1+1]==1&&row[part1row][column1]==1&&row[part1row][column1+1]==0)
	{																										//1,0
		if (column1!=7)																						//0,1
		{
			column1++;
			column2++;
			row[part2row][column2-1]=0;
			row[part1row][column2-1]=0;
			row[part1row][column1]=block.part2[0];
			row[part1row][column2]=block.part3[0];
			row[part2row][column2]=block.part3[1];
		}
	}
	else if (row[part2row][column1]==0&&row[part2row][column1+1]==0&&row[part1row][column1]==1&&row[part1row][column1+1]==0)
	{																										//1,0
		if (column1!=7)																						//0,0
		{
			column1++;
			column2++;
			row[part2row][column2-1]=0;
			row[part1row][column2-1]=0;
			row[part1row][column1]=block.part2[0];
			row[part2row][column1]=block.part2[1];
			row[part1row][column2]=block.part3[0];
			row[part2row][column2]=block.part3[1];
		}
	}
	break;
	
	case rotate:
	upr=block.part2[1];
	upl=block.part2[0];
	lwl=block.part3[0];
	lwr=block.part3[1];
	LCD_DisplayString(1, "Rotate State");
	rotbutton=1;
	break;
	
	case  gameov:
	  if (USART_HasReceived(1)) {
		receive = USART_Receive(1);
		
		if (GetBit(receive,0)!=1)
		{
			message[0]='W';
			message[1]='i';
			message[2]='n';
			message[3]='n';
			message[4]='e';
			message[5]='r';
			message[6]='/0';
		}
		USART_Flush(1);
	}
	else
	{
		if (USART_IsSendReady(1))
		{
			USART_Send(0x02,1);
		}
		message[0]='L';
		message[1]='o';
		message[2]='s';
		message[3]='e';
		message[4]='r';
		message[5]='!';
		message[6]='0/';
	}
	count=0;
	endg=0;
	LCD_DisplayString(1,message);
	break;
	
	default:		
	state = SM_Start; // default: Initial state
	break;
	}

	return state;
}

//Enumeration of states.
enum colup_States { SM2_Start,colup1,colup2};

//check if a row is full lit then mark for deletion
int SMTick2(int state) {

	//State machine transitions
	switch (state) {
	case SM2_Start:	
    state = colup1;
	break;

	case colup1:	
	state=colup1;
	break;
	
	case colup2:
	break;
	
	break;
	

	default:		state = SM2_Start;
	break;
	}

	//State machine actions
	switch(state) {
	case SM2_Start:	break;

	case colup1:
	for (int i=0;i<8;i++)
	{
		if (row[1][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=1;
	    rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[2][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=2;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[3][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=3;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[4][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=4;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[5][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=5;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[6][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=6;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[7][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=7;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	for (int i=0;i<8;i++)
	{
		if (row[8][i]==1)
		{
			rowcnt++;
		}
	}
	if (rowcnt==8)
	{
		delr=8;
		rowcnt=0;
		rowfull=1;
		break;
	}
	rowcnt=0;
	break;
	
	case colup2:
	 
	break;
	
	
	
	default:		break;
	}

	return state;
}

//Enumeration of states.
enum update_States { SM3_Start, UPdate1,UPdate2,UPdate3,UPdate4,UPdate5,UPdate6,UPdate7,UPdate8,UPdate9};

// update patterns
int SMTick3(int state) {
	//State machine transitions
	switch (state) {
	case SM3_Start:	                                                   
	state = UPdate1;
	break;

	case UPdate1:
	state = UPdate2;
	break;

	case UPdate2:
	state = UPdate3;
	break;
	
	case UPdate3:
	state = UPdate4;
	break;
	
	case UPdate4:
	state = UPdate5;
	break;
	
	case UPdate5:
	state = UPdate6;
	break;
	
	case UPdate6:
	state = UPdate7;
	break;
	
	case UPdate7:
	state = UPdate8;
	break;
	
	case UPdate8:
	state = UPdate1;
	break;
	
	
	default:		state = SM3_Start;
				break;
	}

	//State machine actions
	switch(state) {
	case SM3_Start:	break;

	case UPdate1:
	for (int i=0;i<8;i++)
	{
		data[i]=row[0][i];
	}
	upc[0]=0;
	upc[1]=1;
	upc[2]=1;
	upc[3]=1;
	upc[4]=1;
	upc[5]=1;
	upc[6]=1;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	
	break;
	
	case UPdate2:
	for (int i=0;i<8;i++)
	{
		data[i]=row[1][i];
	}
	upc[0]=1;
	upc[1]=0;
	upc[2]=1;
	upc[3]=1;
	upc[4]=1;
	upc[5]=1;
	upc[6]=1;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	case UPdate3:
	for (int i=0;i<8;i++)
	{
		data[i]=row[2][i];
	}
	upc[0]=1;
	upc[1]=1;
	upc[2]=0;
	upc[3]=1;
	upc[4]=1;
	upc[5]=1;
	upc[6]=1;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	case UPdate4:
	for (int i=0;i<8;i++)
	{
		data[i]=row[3][i];
	}
	upc[0]=1;
	upc[1]=1;
	upc[2]=1;
	upc[3]=0;
	upc[4]=1;
	upc[5]=1;
	upc[6]=1;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	case UPdate5:
	for (int i=0;i<8;i++)
	{
		data[i]=row[4][i];
	}
	upc[0]=1;
	upc[1]=1;
	upc[2]=1;
	upc[3]=1;
	upc[4]=0;
	upc[5]=1;
	upc[6]=1;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	case UPdate6:
	for (int i=0;i<8;i++)
	{
		data[i]=row[5][i];
	}
	upc[0]=1;
	upc[1]=1;
	upc[2]=1;
	upc[3]=1;
	upc[4]=1;
	upc[5]=0;
	upc[6]=1;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	case UPdate7:
	for (int i=0;i<8;i++)
	{
		data[i]=row[6][i];
	}
	upc[0]=1;
	upc[1]=1;
	upc[2]=1;
	upc[3]=1;
	upc[4]=1;
	upc[5]=1;
	upc[6]=0;
	upc[7]=1;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	case UPdate8:
	for (int i=0;i<8;i++)
	{
		data[i]=row[7][i];
	}
	upc[0]=1;
	upc[1]=1;
	upc[2]=1;
	upc[3]=1;
	upc[4]=1;
	upc[5]=1;
	upc[6]=1;
	upc[7]=0;
	for (int i=0;i<8;i++)
	{
		data2[i]=upc[i];
	}
	break;
	
	default:		break;
	}

	return state;
}


//Enumeration of states.
enum SM4_States { SM4_display };

// Combine blinking LED outputs from SM2 and SM3, and output on PORTB
int SMTick4(int state) {
	// Local Variables

	unsigned short output;

	//State machine transitions
	switch (state) {
	case SM4_display:	
	break;

	default:		
	state = SM4_display;
				break;
	}

	//State machine actions
	switch(state) {
	case SM4_display:	
	
	// Sets SRCLR to 1 allowing data to be set
	for (int i = 0; i < 8 ; ++i) {
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x02;
		// set SER = next bit of data to be sent.
		//PORTB |= ((data >> i) & 0x01);
		output= data[i]& 0x0001;
		PORTB |= output <<5;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x80;
	}
	
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x01;
	// clears all lines in preparation of a new transmission
	PORTB	 = 0x00;
	
	for (int i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x02;
		// set SER = next bit of data to be sent.
		//PORTB |= ((data >> i) & 0x01);
		output= data2[i]& 0x0001;
		//temp= array[i] & 0x0001;
		PORTB |= output <<4;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x08;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
	break;

	default:		
	break;
	}

	

	return state;
}

//check of top array has any  value in it for two cycles of movement if true then send a flag telling game to end
enum  go_states{checktop};
int gameover(int state)
{
	//State machine transitions
	switch (state) {
		case checktop:
		break;

		default:
		state = checktop;
		break;
	}

	//State machine actions
	switch(state) {
		static char c0=0;
		static char c1=0;
		static char c2=0;
		static char c3=0;
		static char c4=0;
		static char c5=0;
		static char c6=0;
		static char c7=0;
		
		case checktop:
		if (c0==2||c1==2||c2==2||c3==2||c4==2||c5==2||c6==2||c7==2)
		{
			endg=1;
			toprowcnt=0;
			c0=0;
			c1=0;
			c2=0;
			c3=0;
			c4=0;
			c5=0;
			c6=0;
			c7=0;
			for (int i=0;i<9;i++)
			{
				for (int j=0;j<8;j++)
				{
					row[i][j]=output[i][j];
				}
			}
		}
		else
		{
		if (row[8][0]==1)
		{
			c0++;
		}
		else
		{
			c0=0;
		}
		if (row[8][1]==1)
		{
			c1++;
		}
		else
		{
			c1=0;
		}
		if (row[8][2]==1)
		{
			c2++;
		}
		else
		{
			c2=0;;
		}
		
		}
		if (row[8][3]==1)
		{
			c3++;
		}
		else
		{
			c3=0;
		}
		if (row[8][4]==1)
		{
			c4++;
		}
		else
		{
			c4=0;
		}
		if (row[8][5]==1)
		{
			c5++;
		}
		else
		{
			c5=0;
		}
		if (row[8][6]==1)
		{
			c6++;
		}
		else
		{
			c6=0;
		}
		if (row[8][7]==1)
		{
			c7++;
		}
		else
		{
			c7=0;
		}
	
		break;

		default:
		break;
	}

	return state;
}


// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRA = 0x00; PORTA = 0xFF;  // Configure port A  pins as inputs
DDRB = 0xFF; PORTB = 0x00;  // Configure port B pins as outputs
DDRC = 0xFF; PORTC = 0x00; // Configure portc for output on LCD
DDRD = 0xCA; PORTD = 0x35;  // Configures port D as outputs for lcd and setups portd for USART
// . . . etc

unsigned long int SMTick1_period = 250; //movement
unsigned long int SMTick2_period = 2;  // row delete
unsigned long int SMTick3_period = 2;  // update data for shifters
unsigned long int SMTick4_period = 2;  //shift
unsigned long int SMTick5_period = 700;  // lose
//Declare an array of tasks 
static task task1, task2, task3, task4,task5;
task *tasks[] = { &task1, &task2, &task3, &task4,&task5 };
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SMTick1;//Function pointer for the tick.

// Task 2
task2.state = -1;//Task initial state.
task2.period = SMTick2_period;//Task Period.
task2.elapsedTime = SMTick2_period;//Task current elapsed time.
task2.TickFct = &SMTick2;//Function pointer for the tick.

// Task 3
task3.state = -1;//Task initial state.
task3.period = SMTick3_period;//Task Period.
task3.elapsedTime = SMTick3_period; // Task current elasped time.
task3.TickFct = &SMTick3; // Function pointer for the tick.

// Task 4
task4.state = -1;//Task initial state.
task4.period = SMTick4_period;//Task Period.
task4.elapsedTime = SMTick4_period; // Task current elasped time.
task4.TickFct = &SMTick4; // Function pointer for the tick.

// Task 5
task5.state = -1;//Task initial state.
task5.period = SMTick5_period;//Task Period.
task5.elapsedTime = SMTick5_period; // Task current elasped time.
task5.TickFct = &gameover; // Function pointer for the tick.

// Set the timer and turn it on
TimerSet(1);
TimerOn();
ADC_init();
//initUSART();
initUSART(0);
initUSART(1);
LCD_init();
unsigned short i; // Scheduler for-loop iterator

while(1) {
	// Scheduler code
	 adcvalue=ADC;
	 button=PIND&0x20;
	 button=button>>5;
	 rotbutton=PIND&0x10;
	 rotbutton=rotbutton>>4;
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

