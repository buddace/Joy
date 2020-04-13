/*********************************************************************
 * FileName:        buttons.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         sprut
 * Copyright:       2007 Joerg Bredendiek (sprut)
 *
 *
 ********************************************************************/


/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"             // I/O pin mapping
#include "delays.h"
#include "buttons.h"



/** V A R I A B L E S ********************************************************/
#pragma udata
BYTE		RC_Buttons0;
BYTE		RC_Buttons1;
BYTE		RC_Buttons2;

/** D E C L A R A T I O N S **************************************************/
#pragma code

void	Button_Init(void)
{
	// RC0,1,2  sind die Zeilen > output, aber nur einzeln wärend der Abfrage
	// RB0..7 sind die Spalten  > input  und pull up
//    TRISC = 0xFF;
//    TRISB = 0xFF;
//	LATB  = 0;
	INTCON2bits.RBPU = 0;	// pull up

	TRISBbits.TRISB4=1;
	TRISBbits.TRISB5=1;
	TRISDbits.TRISD6=1;
	TRISDbits.TRISD7=1;

	mZeile_0 = 0;
	mZeile_1 = 0;
	mZeile_2 = 0;
	RC_Buttons0 = 0;
	RC_Buttons1 = 0;
	RC_Buttons2 = 0;
}	//Button_Init


void	UP_Button(void)
{
//	RC_Buttons0 = (PORTD ^ 0xFF) >> 6;
	RC_Buttons0 = 0;
	if(PORTDbits.RD6 == FALSE)
	{
		RC_Buttons0 |= 0x01;
	}
	if(PORTDbits.RD7 == FALSE)
	{
		RC_Buttons0 |= 0x02;
	}
	if(PORTBbits.RB4 == FALSE)
	{
		RC_Buttons0 |= 0x04;
	}
	if(PORTBbits.RB5 == FALSE)
	{
		RC_Buttons0 |= 0x08;
	}

//	TRISCbits.TRISC0 = 0;
//	Delay1TCY();
//	RC_Buttons0 = PORTB ^ 0xFF;
//	TRISCbits.TRISC0 = 1;
//
//	TRISCbits.TRISC1 = 0;
//	Delay1TCY();
//	RC_Buttons1 = PORTB ^ 0xFF;
//	TRISCbits.TRISC1 = 1;
//
//	TRISCbits.TRISC2 = 0;
//	Delay1TCY();
//	RC_Buttons2 = PORTB ^ 0xFF;
//	TRISCbits.TRISC2 = 1;
}	//UP_Button





