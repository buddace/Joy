/*********************************************************************
 * FileName:        buttons.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         sprut
 * Copyright:       2007 Joerg Bredendiek (sprut)
 *
 *
 ********************************************************************/

#ifndef BUTTONS_H
#define BUTTONS_H

/** I N C L U D E S **********************************************************/
#include "HardwareProfile.h"     

/** D E F I N I T I O N S ****************************************************/

typedef enum ADC_AnalogCh_e
{
	ADC_X_CH,
	ADC_Y_CH,
	ADC_Z_CH,
	ADC_Rz_CH,
	ADC_X_MAX,	
}ADC_AnalogCh_t;


typedef enum GAMEPORT_byte_e
{
	GAMEPORT_X = ADC_X_CH,
	GAMEPORT_Y = ADC_Y_CH,
	GAMEPORT_Z = ADC_Z_CH,
	GAMEPORT_Rz = ADC_Rz_CH,
	GAMEPORT_B1,
	GAMEPORT_MAX	
}GAMEPORT_byte_t;

/** E X T E R N     **********************************************************/
extern GAMEPORT_byte_t GAMEPORT_byte[GAMEPORT_MAX];

/** P U B L I C  P R O T O T Y P E S *****************************************/
void GAMEPORT_Init(void);
void GAMEPORT_Task(void);

/** P R I V A T E  P R O T O T Y P E S ***************************************/

#endif	// BUTTONS_H

/****  E N D E  *********************************************/
