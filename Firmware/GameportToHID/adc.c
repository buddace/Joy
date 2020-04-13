/*********************************************************************
 * FileName:        adc.c
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
#include "adc.h"


typedef enum ADC_AnalogCh_e
{
	ADC_X_CH,
	ADC_Y_CH,
	ADC_X_MAX,	
}ADC_AnalogCh_t;


/** V A R I A B L E S ********************************************************/
#pragma udata
ADC_AnalogCh_t		ADC_Kanal;
BYTE		ADC_X;	//ADC_Out_0;
BYTE		ADC_Y;  //ADC_Out_1;
//BYTE		ADC_Out_2;
//BYTE		ADC_Out_3;
//BYTE		ADC_Out_4;


/** D E C L A R A T I O N S **************************************************/
#pragma code

void Init_ADC(void)
{
	ADC_Kanal = 0;
	ADC_X = 80;		// Mittelstellung
	ADC_Y = 80;
//	ADC_Out_2 = 80;
//	ADC_Out_3 = 80;
//	ADC_Out_4 = 80;
//	mInitADC()			// AN0,1,2,3,4

	ADCON1 = 0x07;
	ADCON2 = 0x38;//'00111000';

	//bit 7 ADFM: A/D Result Format Select bit
	//1 = Right justified
	//0 = Left justified
	//bit 6 Unimplemented: Read as ‘0’
	//bit 5-3 ACQT2:ACQT0: A/D Acquisition Time Select bits
	//111 = 20 TAD
	//110 = 16 TAD
	//101 = 12 TAD
	//100 = 8 TAD
	//011 = 6 TAD
	//010 = 4 TAD
	//001 = 2 TAD
	//000 = 0 TAD(1)
	//bit 2-0 ADCS2:ADCS0: A/D Conversion Clock Select bits
	//111 = FRC (clock derived from A/D RC oscillator)(1)
	//110 = FOSC/64
	//101 = FOSC/16
	//100 = FOSC/4
	//011 = FRC (clock derived from A/D RC oscillator)(1)
	//010 = FOSC/32
	//001 = FOSC/8
	//000 = FOSC/2

	TRISEbits.TRISE0=1;
	TRISEbits.TRISE1=1;
	ADCON0bits.GO = 1;
}


//** A D C *******************************************************************************//

// ADC-Ergebnis auslesen
// neuen Kanal einstellen
// ADC starten
void UP_ADC(void)
{
	if (ADCON0bits.NOT_DONE)
	{
 		return;     // Wait for conversion
	}

	switch (ADC_Kanal)
	{
		case ADC_X_CH: ADC_X = 0xFF - ADRESH; break;
		case ADC_Y_CH: ADC_Y = 0xFF - ADRESH; break;
//		case 2: ADC_Out_2 = ADRESH; break;
//		case 3: ADC_Out_3 = ADRESH; break;
//		case 4: ADC_Out_4 = ADRESH; break;
	}

	ADC_Kanal++;
	if (ADC_Kanal > ADC_X_MAX)
	{ 
		ADC_Kanal = 0;
	}

	switch (ADC_Kanal)
	{
		case ADC_X_CH:	ADCON0bits.CHS = 0x05; 	break;	//AN6
		case ADC_Y_CH:	ADCON0bits.CHS = 0x06; 	break;	//AN7
//		case 2:	ADCON0 = 0x09; 	break;	//AN2
//		case 3:	ADCON0 = 0x0D; 	break;	//AN3
//		case 4:	ADCON0 = 0x11; 	break;	//AN4
	}
	ADCON0bits.ADON = 1; 
	ADCON0bits.GO = 1;   
} // UP_ADC

