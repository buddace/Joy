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
#include "gameport.h"
#include "portdef.h"



/** V A R I A B L E S ********************************************************/
#pragma udata
BYTE	ADC_Ch[GAMEPORT_MAX]={
GAMEPAD_X_ADCCh,
GAMEPAD_Y_ADCCh,
GAMEPAD_Z_ADCCh,
GAMEPAD_Rz_ADCCh
};

GAMEPORT_byte_t GAMEPORT_byte[GAMEPORT_MAX];
ADC_AnalogCh_t		ADC_Kanal;

/** D E C L A R A T I O N S **************************************************/
#pragma code

void GAMEPORT_Init(void)
{
	BYTE i;
//	INTCON2bits.RBPU = 0;	// pull up

    GAMEPAD_B1Tris = TRUE;
	GAMEPAD_B2Tris = TRUE;
	GAMEPAD_B3Tris = TRUE;
	GAMEPAD_B4Tris = TRUE;


	ADC_Kanal = ADC_X_CH;
	ADCON1 = 0x0F;//0x07;
#if defined(__18F4550)
	ADCON1bits.PCFG = 0x07;
#else
	ADCON1bits.PCFG = 0x0B;
#endif
	//bit 7-6 Unimplemented: Read as ‘0’
	//bit 5 VCFG1: Voltage Reference Configuration bit (VREF- source)
	//1 = VREF- (AN2)
	//0 = VSS
	//bit 4 VCFG0: Voltage Reference Configuration bit (VREF+ source)
	//1 = VREF+ (AN3)
	//0 = VDD
	//bit 3-0 PCFG3:PCFG0: A/D Port Configuration Control bits:



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

	GAMEPAD_XTris=TRUE;
	GAMEPAD_YTris=TRUE;
	GAMEPAD_ZTris=TRUE;
	GAMEPAD_RzTris=TRUE;
	ADCON0bits.GO = 1;


	for(i=0;i<GAMEPORT_MAX;i++)
	{
		GAMEPORT_byte[i]=0;
	}
}	


//** A D C *******************************************************************************//

// ADC-Ergebnis auslesen
// neuen Kanal einstellen
// ADC starten
void GAMEPORT_Task(void)
{
//Buttons
	GAMEPORT_byte[GAMEPORT_B1] = 0;

	if(GAMEPAD_B1Port == FALSE)
	{
		GAMEPORT_byte[GAMEPORT_B1] |= 0x01;
	}

	if(GAMEPAD_B2Port == FALSE)
	{
		GAMEPORT_byte[GAMEPORT_B1] |= 0x02;
	}

	if(GAMEPAD_B3Port == FALSE)
	{
		GAMEPORT_byte[GAMEPORT_B1] |= 0x04;
	}

	if(GAMEPAD_B4Port == FALSE)
	{
		GAMEPORT_byte[GAMEPORT_B1] |= 0x08;
	}
//Analogs
	if (ADCON0bits.NOT_DONE)
	{

	}
	else
	{

		GAMEPORT_byte[ADC_Kanal] = 0xFF - ADRESH;
		ADC_Kanal++;
		if (ADC_Kanal >= ADC_X_MAX)
		{ 
			ADC_Kanal = ADC_X_CH;
		}
//		switch (ADC_Kanal)
//		{
//			case ADC_X_CH:	ADCON0bits.CHS = 0x03; 	break;	//AN0
//			case ADC_Y_CH:	ADCON0bits.CHS = 0x02; 	break;	//AN1
//			case ADC_Z_CH:	ADCON0bits.CHS = 0x01; 	break;	//AN2
//			case ADC_Rz_CH:	ADCON0bits.CHS = 0x00; 	break;	//AN3
//		}
		ADCON0bits.CHS = ADC_Ch[ADC_Kanal];
	
		ADCON0bits.ADON = 1; 
		ADCON0bits.GO = 1; 
	}  

	return;
} // UP_ADC



