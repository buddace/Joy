#ifndef _MAIN_H_
#define _MAIN_H_

#define Delay(x) DELAY = x; while(--DELAY){ Nop(); Nop(); }




/***** Ports *****/
// attention line for the PS2 controller
#define pin_ATT	 		LATBbits.LATB2
									
/***** Buttons *****/
// these macros can be used as meaningful shortcuts into
// the response bytes in a PS2's return packet.
#define PS2_Select		PS2_byte1.bit0
#define PS2_Start		PS2_byte1.bit3
#define PS2_R1			PS2_byte2.bit3
#define PS2_R2			PS2_byte2.bit1
#define PS2_R3			PS2_byte1.bit2
#define PS2_L1			PS2_byte2.bit2
#define PS2_L2			PS2_byte2.bit0
#define PS2_L3			PS2_byte1.bit1
#define PS2_Up			PS2_byte1.bit4
#define PS2_Down		PS2_byte1.bit6
#define PS2_Left		PS2_byte1.bit7
#define PS2_Right		PS2_byte1.bit5
#define PS2_Circle		PS2_byte2.bit5
#define PS2_Square		PS2_byte2.bit7
#define PS2_X			PS2_byte2.bit6
#define PS2_Triangle	PS2_byte2.bit4
#define PS2_RJX			PS2_byte3.byte
#define PS2_RJY			PS2_byte4.byte
#define PS2_LJX			PS2_byte5.byte
#define PS2_LJY			PS2_byte6.byte
	

/***** Types *****/
// the union lets us access the bytes coming back from a PS2
// either bit by bit or by a whole byte.  same memory location,
// just two different names.
typedef union {
  struct {
    unsigned bit7:1;
    unsigned bit6:1;
    unsigned bit5:1;
    unsigned bit4:1;
    unsigned bit3:1;
    unsigned bit2:1;
    unsigned bit1:1;
    unsigned bit0:1;
  };
  struct {
    unsigned char byte;
  };
} uint8; 

					
#endif