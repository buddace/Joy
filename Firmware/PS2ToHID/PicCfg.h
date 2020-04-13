
#ifndef PIC_CFG_H
#define PIC_CFG_H

/** CONFIGURATION **************************************************/
        #pragma config PLLDIV   = 5         // (20 MHz crystal on PICDEM FS USB board)
        #pragma config CPUDIV   = OSC2_PLL3 //OSC1_PLL2   
        #pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
        #pragma config FOSC     = HSPLL_HS
//        #pragma config FCMEM    = OFF
        #pragma config IESO     = OFF
        #pragma config PWRT     = OFF
//        #pragma config BOR      = ON
//        #pragma config BORV     = 3
        #pragma config VREGEN   = ON      //USB Voltage Regulator
        #pragma config WDT      = OFF
        #pragma config WDTPS    = 32768
#ifdef __DEBUG
        #pragma config MCLRE    = ON
#else
        #pragma config MCLRE    = OFF
#endif
        #pragma config LPT1OSC  = OFF
        #pragma config PBADEN   = OFF	//By programming the Configuration bit,PBADEN (CONFIG3H<1>), RB4:RB0 will alternatively be configured as digital inputs on POR.
//      #pragma config CCP2MX   = ON
        #pragma config STVREN   = ON
        #pragma config LVP      = OFF
//      #pragma config ICPRT    = OFF       // Dedicated In-Circuit Debug/Programming
        #pragma config XINST    = OFF       // Extended Instruction Set
        #pragma config CP0      = OFF
        #pragma config CP1      = OFF
//      #pragma config CP2      = OFF
//      #pragma config CP3      = OFF
        #pragma config CPB      = OFF
//      #pragma config CPD      = OFF
        #pragma config WRT0     = OFF
        #pragma config WRT1     = OFF
//      #pragma config WRT2     = OFF
//      #pragma config WRT3     = OFF
        #pragma config WRTB     = OFF       // Boot Block Write Protection
        #pragma config WRTC     = OFF
//      #pragma config WRTD     = OFF
        #pragma config EBTR0    = OFF
        #pragma config EBTR1    = OFF
//      #pragma config EBTR2    = OFF
//      #pragma config EBTR3    = OFF
        #pragma config EBTRB    = OFF
   

#endif

