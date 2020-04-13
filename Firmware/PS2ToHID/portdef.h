#ifndef __PORTDEF__
#define __PORTDEF__

/************************ HEADERS **********************************/
//#include "main.h"
/************************ DEFINES **********************************/

//************************PSX***************************************/
#define PSX_SDOPort	PORTCbits.RC7 
#define PSX_SDOTris TRISCbits.TRISC7 

#define PSX_SDIPort	PORTBbits.RB0 
#define PSX_SDITris TRISBbits.TRISB0 

#define PSX_SCKPort	PORTBbits.RB1 
#define PSX_SCKTris TRISBbits.TRISB1 

#if defined(__18F4550)
#define PSX_ATT0Port	LATBbits.LATB2 
#define PSX_ATT0Tris TRISBbits.TRISB2

#define PSX_ATT1Port	LATCbits.LATC0 
#define PSX_ATT1Tris 	TRISCbits.TRISC0

#define PSX_ACKPort		PORTCbits.RC6 
#define PSX_ACKPTris 	TRISCbits.TRISC6

#else

#define PSX_ATT0Port	LATCbits.LATC0 
#define PSX_ATT0Tris 	TRISCbits.TRISC0

#define PSX_ATT1Port	LATCbits.LATC1 
#define PSX_ATT1Tris 	TRISCbits.TRISC1

#define PSX_ACKPort		PORTCbits.RC6 
#define PSX_ACKPTris 	TRISCbits.TRISC6
#endif



//************************GAMEPORT**********************************/
#if defined(__18F4550)

#define GAMEPAD_B3Port	PORTBbits.RB4
#define GAMEPAD_B3Tris	TRISBbits.TRISB4

#define GAMEPAD_B4Port	PORTBbits.RB5
#define GAMEPAD_B4Tris	TRISBbits.TRISB5

#define GAMEPAD_B1Port	PORTDbits.RD6
#define GAMEPAD_B1Tris	TRISDbits.TRISD6

#define GAMEPAD_B2Port	PORTDbits.RD7
#define GAMEPAD_B2Tris	TRISDbits.TRISD7

#define GAMEPAD_XPort	PORTEbits.RE0
#define GAMEPAD_XTris	TRISEbits.TRISE0
#define GAMEPAD_X_ADCCh	0x05

#define GAMEPAD_YPort	PORTEbits.RE1
#define GAMEPAD_YTris	TRISEbits.TRISE1
#define GAMEPAD_Y_ADCCh	0x06

#define GAMEPAD_ZPort	PORTAbits.RA2
#define GAMEPAD_ZTris	TRISAbits.TRISA2
#define GAMEPAD_Z_ADCCh	0x00

#define GAMEPAD_RzPort	PORTAbits.RA3
#define GAMEPAD_RzTris	TRISAbits.TRISA3
#define GAMEPAD_Rz_ADCCh	0x01


#else


#define GAMEPAD_B1Port	PORTBbits.RB2
#define GAMEPAD_B1Tris	TRISBbits.TRISB2

#define GAMEPAD_B2Port	PORTBbits.RB5
#define GAMEPAD_B2Tris	TRISBbits.TRISB5

#define GAMEPAD_B3Port	PORTBbits.RB4
#define GAMEPAD_B3Tris	TRISBbits.TRISB4

#define GAMEPAD_B4Port	PORTBbits.RB3
#define GAMEPAD_B4Tris	TRISBbits.TRISB3

#define GAMEPAD_XPort	PORTAbits.RA0
#define GAMEPAD_XTris	TRISAbits.TRISA0
#define GAMEPAD_X_ADCCh	0x03

#define GAMEPAD_YPort	PORTAbits.RA1
#define GAMEPAD_YTris	TRISAbits.TRISA1
#define GAMEPAD_Y_ADCCh	0x02

#define GAMEPAD_ZPort	PORTAbits.RA2
#define GAMEPAD_ZTris	TRISAbits.TRISA2
#define GAMEPAD_Z_ADCCh	0x01

#define GAMEPAD_RzPort	PORTAbits.RA3
#define GAMEPAD_RzTris	TRISAbits.TRISA3
#define GAMEPAD_Rz_ADCCh	0x00
#endif

#endif