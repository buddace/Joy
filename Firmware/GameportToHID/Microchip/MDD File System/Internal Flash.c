/******************************************************************************
 *
 *               Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        FS Phys Interface Template.c
 * Dependencies:    TEMPLATEFILE.h
 *					string.h
 *                  FSIO.h
 *                  FSDefs.h
 * Processor:       None
 * Compiler:        None
 * Company:         Microchip Technology, Inc.
 * Version:         1.0.0
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
*****************************************************************************/

#include "Compiler.h"
#include "MDD File System\FSIO.h"
#include "MDD File System\FSDefs.h"
#include "string.h"
#include "MDD File System\internal flash.h"
#include "HardwareProfile.h"


/*************************************************************************/
/*  Note:  This file is included as a template of a C file for           */
/*         a new physical layer. It is designed to go with               */
/*         "TEMPLATEFILE.h"                                              */
/*************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/

#ifdef USE_PIC18
	#pragma udata
	#pragma code
#endif


#ifdef USE_PIC18
	#pragma udata
	#pragma code
#endif

/******************************************************************************
 * Prototypes
 *****************************************************************************/

BYTE DISKmount( DISK *dsk);
BYTE LoadMBR(DISK *dsk);
BYTE LoadBootSector(DISK *dsk);
extern void Delayms(BYTE milliseconds);
BYTE MediaInitialize(void);

#ifdef USE_PIC24
	BYTE ReadByte( BYTE* pBuffer, WORD index );
	WORD ReadWord( BYTE* pBuffer, WORD index );
	DWORD ReadDWord( BYTE* pBuffer, WORD index );
#endif

/******************************************************************************
 * Function:        BYTE MediaDetect(void)
 *
 * PreCondition:    InitIO() function has been executed.
 *
 * Input:           void
 *
 * Output:          TRUE   - Card detected
 *                  FALSE   - No card detected
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
BYTE MediaDetect()
{
	return TRUE;
}//end MediaDetect

/******************************************************************************
 * Function:        WORD ReadSectorSize(void)
 *
 * PreCondition:    MediaInitialize() is complete
 *
 * Input:           void
 *
 * Output:          WORD - size of the sectors for this physical media.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
WORD ReadSectorSize(void)
{
    return MEDIA_SECTOR_SIZE;
}

/******************************************************************************
 * Function:        DWORD ReadCapacity(void)
 *
 * PreCondition:    MediaInitialize() is complete
 *
 * Input:           void
 *
 * Output:          DWORD - size of the "disk"
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
DWORD ReadCapacity(void)
{
    //TODO: fix this to a variable that is acceptable
    return 0xA;
}

/******************************************************************************
 * Function:        BYTE InitIO(void)
 *
 * PreCondition:    None
 *
 * Input:           void
 *
 * Output:          TRUE   - Card initialized
 *                  FALSE   - Card not initialized
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
#if 0
BYTE InitIO (void)
{
	return  (DISKmount (&gDiskData) == CE_GOOD);
}
#endif


/******************************************************************************
 * Function:        BYTE MediaInitialize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Returns TRUE if media is initialized, FALSE otherwise
 *
 * Overview:        MediaInitialize initializes the media card and supporting variables.
 *
 * Note:            None
 *****************************************************************************/
BYTE MediaInitialize(void)
{
	return TRUE;
}//end MediaInitialize


/******************************************************************************
 * Function:        BYTE SectorRead(DWORD sector_addr, BYTE *buffer)
 *
 * PreCondition:    None
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be stored, see
 *                                'ram_acs.h' for 'block' definition.
 *                                'Block' is dependent on whether internal or
 *                                external memory is used
 *
 * Output:          Returns TRUE if read successful, false otherwise
 *
 * Side Effects:    None
 *
 * Overview:        SectorRead reads 512 bytes of data from the card starting
 *                  at the sector address specified by sector_addr and stores
 *                  them in the location pointed to by 'buffer'.
 *
 * Note:            The device expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
BYTE SectorRead(DWORD sector_addr, BYTE* buffer)
{
    #if defined(__C30__)
        WORD PSVPageSave;

        PSVPageSave = PSVPAG;
        //TODO: fix this so it is not static but rather based on the requested sector
        PSVPAG = 0x01;
    #endif

    memcpypgm2ram
    (
        (void*)buffer,
        (ROM void*)(MASTER_BOOT_RECORD_ADDRESS + (sector_addr * MEDIA_SECTOR_SIZE)),
        MEDIA_SECTOR_SIZE
    );

    #if defined(__C30__)
        PSVPAG = PSVPageSave;
    #endif

	return TRUE;
}//end SectorRead

/******************************************************************************
 * Function:        BYTE SectorWrite(DWORD sector_addr, BYTE *buffer, BYTE allowWriteToZero)
 *
 * PreCondition:    None
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be read
 *                  allowWriteToZero - If true, writes to the MBR will be valid
 *
 * Output:          Returns TRUE if write successful, FALSE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        SectorWrite sends 512 bytes of data from the location
 *                  pointed to by 'buffer' to the card starting
 *                  at the sector address specified by sector_addr.
 *
 * Note:            The sample device expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
#if defined(__18CXX)
//#pragma udata myFileBuffer=0x600
#endif
volatile unsigned char file_buffer[ERASE_BLOCK_SIZE] __attribute__((far));
#if defined(__18CXX)
//#pragma udata
#endif

#if defined(__C30__)
    extern int  ReadPMHigh(int);
    extern int  ReadPMLow(int);
    extern void UnlockPM(void);
    extern int  WritePMHigh(int, int);
    extern int  WritePMHighB(int, int);
    extern int  WritePMLow(int, int);
    extern int  WritePMLowB(int, int);
#endif

void EraseBlock(BYTE* dest);
void WriteRow(void);
void WriteByte(unsigned char);

#define PROGRAM_WORD        0x4003
#define ERASE               0x4042
#define PROGRAM_PAGE        0x4001

BYTE *FileAddress = 0;

BYTE SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero)
{
    ROM BYTE* dest;
    BOOL foundDifference;
    WORD blockCounter;
    WORD sectorCounter;


    #if defined(__C30__)
    WORD PSVPageSave;
    WORD_VAL data;

    PSVPageSave = PSVPAG;
    //TODO: make this not a hardcoded value
    PSVPAG = 0x01;
    #endif

    if(sector_addr == 0x04)
    {
        Nop();
    }

    dest = (ROM BYTE*)(MASTER_BOOT_RECORD_ADDRESS + (sector_addr * MEDIA_SECTOR_SIZE));

    sectorCounter = 0;

    while(sectorCounter < MEDIA_SECTOR_SIZE)
    {
        foundDifference = FALSE;
        for(blockCounter = 0; blockCounter < ERASE_BLOCK_SIZE; blockCounter++)
        {
            if(dest[sectorCounter] != buffer[sectorCounter])
            {
                foundDifference = TRUE;
                sectorCounter -= blockCounter;
                break;
            }
            sectorCounter++;
        }
        if(foundDifference == TRUE)
        {
            BYTE i,j;
            WORD address;

            #if (ERASE_BLOCK_SIZE >= MEDIA_SECTOR_SIZE)
                address = ((WORD)(dest + sectorCounter) & ~(ERASE_BLOCK_SIZE - 1));

                memcpypgm2ram
                (
                    (void*)file_buffer,
                    (ROM void*)address,
                    ERASE_BLOCK_SIZE
                );

                EraseBlock((BYTE*)address);

                address = ((WORD)(dest + sectorCounter) & (ERASE_BLOCK_SIZE - 1));

                memcpy
                (
                    (void*)(&file_buffer[address]),
                    (void*)buffer,
                    MEDIA_SECTOR_SIZE
                );

            #else

                address = ((WORD)(&dest[sectorCounter]) & ~(ERASE_BLOCK_SIZE - 1));

                EraseBlock((BYTE*)address);

                sectorCounter = sectorCounter & ~(ERASE_BLOCK_SIZE - 1);

                memcpy
                (
                    (void*)file_buffer,
                    (void*)buffer+sectorCounter,
                    ERASE_BLOCK_SIZE
                );
            #endif

            //sectorCounter = sectorCounter & ~(ERASE_BLOCK_SIZE - 1);

            i=ERASE_BLOCK_SIZE/WRITE_BLOCK_SIZE;
            j=0;
            while(i-->0)
            {
                //Write the new data
                for(blockCounter = 0; blockCounter < WRITE_BLOCK_SIZE; blockCounter++)
                {
                    //Write the data
                    #if defined(__18CXX)
                        TABLAT = file_buffer[(j * WRITE_BLOCK_SIZE) + blockCounter];
                        _asm tblwtpostinc _endasm
                    #endif

                    #if defined(__C30__)

                        if(((unsigned int)FileAddress & 0x01) == 0x01)
                        {
                            FileAddress--;
                            data.v[1] = file_buffer[sectorCounter];
                            WritePMLow(data.Val, (int)FileAddress);
                            WritePMHigh(0,(int)FileAddress);
                            FileAddress += 2;
                        }
                        else
                        {
                            data.v[0] = file_buffer[sectorCounter];
                            FileAddress++;
                        }
                    #endif

                    sectorCounter++;
                }

                j++;

                //write the row
                #if defined(__18CXX)
                    // Start the write process: reposition tblptr back into memory block that we want to write to.
                     _asm tblrdpostdec _endasm

                    // Write flash memory, enable write control.
                    EECON1 = 0x84;

                    EECON2 = 0x55;
                    EECON2 = 0xaa;
                    EECON1bits.WR = 1;
                    Nop();
                    EECON1bits.WREN = 0;

                    TBLPTR++;
                #endif
                #if defined(__C30__)
                    NVMCON = PROGRAM_PAGE;
                    UnlockPM();
                #endif

                Nop();
                Nop();
            }
        }
    }

    #if defined(__C30__)
    PSVPAG = PSVPageSave;
    #endif

	return TRUE;
} //end SectorWrite

void EraseBlock(BYTE* dest)
{
    #if defined(__18CXX)
        TBLPTR = (unsigned short long)dest;

        //Erase the current block
        EECON1 = 0x94;

        EECON2 = 0x55;
        EECON2 = 0xaa;
        EECON1bits.WR = 1;

        EECON1bits.WREN = 0;
    #endif

    #if defined(__C30__)
        NVMCON = ERASE;
        WritePMLow(0, (int)dest);
        UnlockPM();
        FileAddress = dest;
        NVMCON = PROGRAM_PAGE;
    #endif
}
void WriteRow(void)
{

}

/******************************************************************************
 * Function:        BYTE WriteProtectState(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          BYTE    - Returns the status of the "write enabled" pin
 *
 * Side Effects:    None
 *
 * Overview:        Determines if the card is write-protected
 *
 * Note:            None
 *****************************************************************************/

BYTE WriteProtectState(void)
{
	return FALSE;
}

#ifdef USE_PIC24

/******************************************************************************
 * Function:        BYTE ReadByte(BYTE * pBuffer, WORD index)
 *
 * PreCondition:    None
 *
 * Input:           pBuffer	- pointer to a buffer to read from
 *					index	- index in the buffer to read to
 *
 * Output:          BYTE	- the byte read
 *
 * Side Effects:    None
 *
 * Overview:        Reads a byte from a buffer
 *
 * Note:            None
 *****************************************************************************/


	BYTE ReadByte( BYTE* pBuffer, WORD index )
	{
		return( pBuffer[index] );
	}


/******************************************************************************
 * Function:        BYTE ReadWord(BYTE * pBuffer, WORD index)
 *
 * PreCondition:    None
 *
 * Input:           pBuffer	- pointer to a buffer to read from
 *					index	- index in the buffer to read to
 *
 * Output:          WORD	- the word read
 *
 * Side Effects:    None
 *
 * Overview:        Reads a 16-bit word from a buffer
 *
 * Note:            None
 *****************************************************************************/


	WORD ReadWord( BYTE* pBuffer, WORD index )
	{
		BYTE loByte, hiByte;
		WORD res;

		loByte = pBuffer[index];
		hiByte = pBuffer[index+1];
		res = hiByte;
		res *= 0x100;
		res |= loByte;
		return( res );
	}

/******************************************************************************
 * Function:        BYTE ReadDWord(BYTE * pBuffer, WORD index)
 *
 * PreCondition:    None
 *
 * Input:           pBuffer	- pointer to a buffer to read from
 *					index	- index in the buffer to read to
 *
 * Output:          DWORD	- the double word read
 *
 * Side Effects:    None
 *
 * Overview:        Reads a 32-bit double word from a buffer
 *
 * Note:            None
 *****************************************************************************/


	DWORD ReadDWord( BYTE* pBuffer, WORD index )
	{
		WORD loWord, hiWord;
		DWORD result;

		loWord = ReadWord( pBuffer, index );
		hiWord = ReadWord( pBuffer, index+2 );

		result = hiWord;
		result *= 0x10000;
		result |= loWord;
		return result;
	}
#endif

