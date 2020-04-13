#include "psx.h"


//------------ DEFINES ------------------------------- //

// this is a macro shortcut (this code will replace bitrev(c) at compile time)
// for reversing the bit order 
#define bitrev(c)	c = (c & 0x0F) << 4 | (c & 0xF0) >> 4; \
					c = (c & 0x33) << 2 | (c & 0xCC) >> 2; \
					c = (c & 0x55) << 1 | (c & 0xAA) >> 1;



//------------ Global Variables ------------------------------- //
/*R	L	U	D*/
const BYTE PS2_HatMap[16] = {
PS2_HAT_SWITCH_NULL			,	/*0000 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NORTH		,   /*0001 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_EAST			,   /*0010 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NORTH_EAST  	,   /*0011 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_SOUTH 		,   /*0100 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL			,   /*0101 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_SOUTH_EAST	,   /*0110 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL 	    ,   /*0111 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_WEST	    	,   /*1000 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NORTH_WEST  	,   /*1001 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL 		,   /*1010 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL   		,   /*1011 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_SOUTH_WEST	,   /*1100 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL   		,   /*1101 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL   		,   /*1110 UP   RGHT DOWN LEFT*/
PS2_HAT_SWITCH_NULL   		,   /*1111 UP   RGHT DOWN LEFT*/
};


BYTE PS2_byte0[E_PS2_MAX];
BYTE PS2_byte1[E_PS2_MAX];
BYTE PS2_pressure[12];

// ---------------  utility functions ---------------- //

void PS2_Init(void)
{

//	ADCON0 = 0b00110000;	
//	TRISCbits.TRISC7 = 0;	// SDO (SPI Output)	
//	LATBbits.LATB2 = 1;  //ATT for SPI)
//	TRISB = 0b11111001;
//	TRISCbits.TRISC6 = 0; // makes TX an output

	BYTE i;  

	PSX_SDOTris = FALSE;
	PSX_SDITris = TRUE;
	PSX_SCKTris = FALSE;
	PSX_ATT0Tris = FALSE;
	PSX_ATT1Tris = FALSE;
	PSX_ACKPTris = TRUE;

	PSX_ATT0Port = TRUE;
	PSX_ATT1Port = TRUE;

	// Fosc = 16 MHz, so the SPI data rate is 250kHz.  The playstation 2 usually
	// talks to controllers at 500kHZ, except the Guitar Hero controller.  Playstation 1 
	// talked at 250kHz.  Going at 500kHZ gave us some noise issues on some controllers.

	OpenSPI(SPI_FOSC_64, MODE_11, SMPMID);  //MODE_11 = (CKP, CKE) = (1,0) 
										//CKP=1 (idle clock state is high, active low)  
										//CKE=0	data is read when the clock transitions
										//from active to idle.  This causes the clock
										//to start out going low, and then high mid-cycle.

	for(i=0;i<E_PS2_MAX;i++)
	{
		PS2_byte0[i]=0;
		PS2_byte1[i]=0;
	}

}


// ---------------  utility functions ---------------- //

void PS2_TxRx(BYTE data_out,BYTE *reg_in)
{
	bitrev(data_out);
	SSPBUF = (data_out);
	while(!SSPSTATbits.BF);
	*reg_in = SSPBUF;
	bitrev(*reg_in);
	
}

void PS2_Task(void)
{


//    Analogue Controller in Red Mode
//    BYTE    CMND    DATA
//
//     01     0x01    idle
//     02     0x42    0x73
//     03     idle    0x5A    Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7
//     04     idle    data    SLCT JOYR JOYL STRT UP   RGHT DOWN LEFT
//     05     idle    data    L2   R2   L1   R1   /\   O    X    []
//     06     idle    data    Right Joy 0x00 = Left  0xFF = Right
//     07     idle    data    Right Joy 0x00 = Up    0xFF = Down
//     08     idle    data    Left Joy  0x00 = Left  0xFF = Right
//     09     idle    data    Left Joy  0x00 = Up    0xFF = Down
//
//    All Buttons active low.

//    Standard Digital Pad
//    BYTE    CMND    DATA
//
//     01     0x01    idle
//     02     0x42    0x41
//     03     idle    0x5A    Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7
//     04     idle    data    SLCT           STRT UP   RGHT DOWN LEFT
//     05     idle    data    L2   R2    L1  R1   /\   O    X    []
//   
//    All Buttons active low.
static _PS2_Joy_t PS2_JoyCh = PS2_JOY_0;

switch(PS2_JoyCh)
	{
	case PS2_JOY_0:
		// poll and command
		PSX_ATT0Port = FALSE;					// pull ATT low to signal gamepad
		Delay1TCY();
		PS2_TxRx(0x01, &PS2_byte0[E_PS2_RESPONSE]);					// send: 	Start (0x01 LSBF (order flipped in ps2_tx)				
		PS2_TxRx(0x42, &PS2_byte0[E_PS2_ID]);	// send: 	Request Data (0x42 LSBF)						
		PS2_TxRx(0x00, &PS2_byte0[E_PS2_MSG]);// receive gamepad ID:	should be '0x5A' for 'ready to send'
			
		/*comando motore e bottoni*/		
		PS2_TxRx(0x00, &PS2_byte0[E_PS2_BUTONS1]);			// receive:	1st byte of button states and set big (left) motor to Left arrow pressure
		PS2_TxRx(0x00, &PS2_byte0[E_PS2_BUTONS2]);			// receive:	2nd byte of button states and turn on small (right) motor if Right arrow pressure = 0xFF
		/*joistyck R*/
		/*asse x*/
		PS2_TxRx(0x00,&PS2_byte0[E_PS2_RX]);			// receive:	3rd byte of button states	
		/*asse y*/
		PS2_TxRx(0x00,&PS2_byte0[E_PS2_RY]);			// receive:	4th byte of button states
		/*joistick L*/
		/*asse x*/
		PS2_TxRx(0x00,&PS2_byte0[E_PS2_LX]);			// receive:	5th byte of button states
		/*asse y*/
		PS2_TxRx(0x00,&PS2_byte0[E_PS2_LY]);			// receive:	6th byte of button states
	
		PSX_ATT0Port = TRUE;
		Delay1TCY();
	break;

	case PS2_JOY_1:
		// poll and command
		PSX_ATT1Port = FALSE;					// pull ATT low to signal gamepad
		Delay1TCY();
		PS2_TxRx(0x01, &PS2_byte1[E_PS2_RESPONSE]);					// send: 	Start (0x01 LSBF (order flipped in ps2_tx)				
		PS2_TxRx(0x42, &PS2_byte1[E_PS2_ID]);	// send: 	Request Data (0x42 LSBF)						
		PS2_TxRx(0x00, &PS2_byte1[E_PS2_MSG]);// receive gamepad ID:	should be '0x5A' for 'ready to send'
			
		/*comando motore e bottoni*/		
		PS2_TxRx(0x00, &PS2_byte1[E_PS2_BUTONS1]);			// receive:	1st byte of button states and set big (left) motor to Left arrow pressure
		PS2_TxRx(0x00, &PS2_byte1[E_PS2_BUTONS2]);			// receive:	2nd byte of button states and turn on small (right) motor if Right arrow pressure = 0xFF
		/*joistyck R*/
		/*asse x*/
		PS2_TxRx(0x00,&PS2_byte1[E_PS2_RX]);			// receive:	3rd byte of button states	
		/*asse y*/
		PS2_TxRx(0x00,&PS2_byte1[E_PS2_RY]);			// receive:	4th byte of button states
		/*joistick L*/
		/*asse x*/
		PS2_TxRx(0x00,&PS2_byte1[E_PS2_LX]);			// receive:	5th byte of button states
		/*asse y*/
		PS2_TxRx(0x00,&PS2_byte1[E_PS2_LY]);			// receive:	6th byte of button states
	
		PSX_ATT1Port = TRUE;
		Delay1TCY();
	break;

	default:
		PS2_JoyCh = PS2_JOY_0;
	break;
	}
	
	if(++PS2_JoyCh>=PS2_JOY_MAX)
	{
		PS2_JoyCh = PS2_JOY_0;
	}

}