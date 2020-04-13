
/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "GenericTypeDefs.h"
#include "portdef.h"
#include <spi.h>
#include <timers.h>
#include "delays.h"

/** Defines**********************************************************/
//http://www.microsoft.com/whdc/archive/hidgame.mspx
#define PS2_HAT_SWITCH_NORTH            0x0
#define PS2_HAT_SWITCH_NORTH_EAST       0x1
#define PS2_HAT_SWITCH_EAST             0x2
#define PS2_HAT_SWITCH_SOUTH_EAST       0x3
#define PS2_HAT_SWITCH_SOUTH            0x4
#define PS2_HAT_SWITCH_SOUTH_WEST       0x5
#define PS2_HAT_SWITCH_WEST             0x6
#define PS2_HAT_SWITCH_NORTH_WEST       0x7
#define PS2_HAT_SWITCH_NULL             0x8
/** Enums**********************************************************/

enum _PS2_byte_e
{
	E_PS2_RESPONSE,
	E_PS2_ID,
	E_PS2_MSG,
	E_PS2_BUTONS1,
	E_PS2_BUTONS2,
	E_PS2_RX,
	E_PS2_RY,
	E_PS2_LX,
	E_PS2_LY,
	E_PS2_MAX,
};

typedef enum _PS2_Joy_e
{
	PS2_JOY_0,
	PS2_JOY_1,
	PS2_JOY_MAX,
} _PS2_Joy_t;

/** Prototypes**********************************************************/
void PS2_Init(void);
void PS2_TxRx(BYTE data_out,BYTE *reg_in);
void PS2_Task(void);

/** Variables**********************************************************/
extern BYTE PS2_byte0[E_PS2_MAX];
extern BYTE PS2_byte1[E_PS2_MAX];
extern const BYTE PS2_HatMap[];
