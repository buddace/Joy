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
#define mZeile_0            LATCbits.LATC0
#define mZeile_1            LATCbits.LATC1
#define mZeile_2            LATCbits.LATC2



/** E X T E R N     **********************************************************/
extern	BYTE		RC_Buttons0;
extern	BYTE		RC_Buttons1;
extern	BYTE		RC_Buttons2;


/** P U B L I C  P R O T O T Y P E S *****************************************/
void	Button_Init(void);
void	UP_Button(void);

/** P R I V A T E  P R O T O T Y P E S ***************************************/

#endif	// BUTTONS_H

/****  E N D E  *********************************************/
