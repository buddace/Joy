/******************************************************************************
 *
 *               Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        SD-SPI.c
 * Dependencies:    SD-SPI.h
 *                  string.h
 *                  FSIO.h
 *                  FSDefs.h
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 * Version:         1.1.2
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

#include "MDD File System\FSIO.h"
#include "MDD File System\FSDefs.h"
#include "MDD File System\SD-SPI.h"
#include "string.h"
#include "FSConfig.h"
#include "HardwareProfile.h"

/******************************************************************************
 * Global Variables
 *****************************************************************************/

DWORD finalLBA;


/*********************************************************************
 * sdmmc_cmdtable
 * - Provides information for all the sdmmc commands that we support
 * 
 * Notes: We turn off the CRC as soon as possible, so the commands with
 *        0xFF don't need to be calculated in runtime 
 *********************************************************************/
#ifdef USE_PIC18
    const rom typMMC_CMD sdmmc_cmdtable[] =
#else
    const typMMC_CMD sdmmc_cmdtable[] =
#endif
{
    // cmd                      crc     response
    {cmdGO_IDLE_STATE,          0x95,   R1,     NODATA},
    {cmdSEND_OP_COND,           0xF9,   R1,     NODATA},
    {cmdSEND_CSD,               0xAF,   R1,     MOREDATA},
    {cmdSEND_CID,               0x1B,   R1,     MOREDATA},
    {cmdSTOP_TRANSMISSION,      0xC3,   R1,     NODATA},
    {cmdSEND_STATUS,            0xAF,   R2,     NODATA},
    {cmdSET_BLOCKLEN,           0xFF,   R1,     NODATA},
    {cmdREAD_SINGLE_BLOCK,      0xFF,   R1,     MOREDATA},
    {cmdREAD_MULTI_BLOCK,       0xFF,   R1,     MOREDATA},
    {cmdWRITE_SINGLE_BLOCK,     0xFF,   R1,     MOREDATA},
    {cmdWRITE_MULTI_BLOCK,      0xFF,   R1,     MOREDATA}, 
    {cmdTAG_SECTOR_START,       0xFF,   R1,     NODATA},
    {cmdTAG_SECTOR_END,         0xFF,   R1,     NODATA},
    {cmdUNTAG_SECTOR,           0xFF,   R1,     NODATA},
    {cmdTAG_ERASE_GRP_START,    0xFF,   R1,     NODATA},
    {cmdTAG_ERASE_GRP_END,      0xFF,   R1,     NODATA},
    {cmdUNTAG_ERASE_GRP,        0xFF,   R1,     NODATA},
    {cmdERASE,                  0xDF,   R1b,    NODATA},
    {cmdLOCK_UNLOCK,            0x89,   R1b,    NODATA},  
    {cmdSD_APP_OP_COND,         0xE5,   R1,     NODATA},
    {cmdAPP_CMD,                0x73,   R1,     NODATA},
    {cmdREAD_OCR,               0x25,   R3,     NODATA},
    {cmdCRC_ON_OFF,             0x25,   R1,     NODATA}
};




/******************************************************************************
 * Prototypes
 *****************************************************************************/

extern void Delayms(BYTE milliseconds);
BYTE MDD_SDSPI_ReadMedia(void);
BYTE MDD_SDSPI_MediaInitialize(void);
MMC_RESPONSE SendMMCCmd(BYTE cmd, DWORD address);

#if defined USE_16BIT || defined USE_32BIT
    void OpenSPIM ( unsigned int sync_mode, unsigned char bus_mode, unsigned char smp_phase);
    void CloseSPIM( void );
    unsigned char WriteSPIM( unsigned char data_out );
#elif defined USE_PIC18
    void OpenSPIM ( unsigned char sync_mode, unsigned char bus_mode, unsigned char smp_phase);
    void CloseSPIM( void );
    unsigned char WriteSPIM( unsigned char data_out );

    unsigned char WriteSPIManual(unsigned char data_out);
    BYTE ReadMediaManual (void);
    MMC_RESPONSE SendMMCCmdManual(BYTE cmd, DWORD address);
#endif

#ifdef USE_PIC32
static inline __attribute__((always_inline)) unsigned char SPICalutateBRG(unsigned int pb_clk, unsigned int spi_clk)
{
    unsigned int brg;

    brg = pb_clk / (2 * spi_clk);

    if(pb_clk % (2 * spi_clk))
        brg++;

    if(brg > 0x100)
        brg = 0x100;

    if(brg)
        brg--;

    return (unsigned char) brg;
}
#endif
/******************************************************************************
 * Function:        BYTE MDD_SDSPI_MediaDetect(void)
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
BYTE MDD_SDSPI_MediaDetect()
{
    return(!SD_CD);
}//end MediaDetect

/******************************************************************************
 * Function:        WORD ReadSectorSize(void)
 *
 * PreCondition:    MDD_MediaInitialize() is complete
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
WORD MDD_SDSPI_ReadSectorSize(void)
{
    return MEDIA_SECTOR_SIZE;
}

/******************************************************************************
 * Function:        DWORD ReadCapacity(void)
 *
 * PreCondition:    MDD_MediaInitialize() is complete
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
DWORD MDD_SDSPI_ReadCapacity()
{
    return (finalLBA);
}

/******************************************************************************
 * Function:        void MDD_SDSPI_InitIO(void)
 *
 * PreCondition:    None
 *
 * Input:           void
 *
 * Output:          void
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/

void MDD_SDSPI_InitIO (void)
{
    // Turn off the card
    MMC_OFF;
    SD_CD_TRIS = INPUT;            //Card Detect - input
    SD_CS = 1;                     //Initialize Chip Select line
    SD_CS_TRIS = OUTPUT;            //Card Select - output
    SD_WE_TRIS = INPUT;            //Write Protect - input
}
      

/******************************************************************************
 * Function:        BYTE MDD_SDSPI_MediaInitialize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE                    - Everything is fine
 *                  FALSE      - Communication was not established
 *
 * Overview:        MediaInitialize initializes the media card and supporting variables.
 *
 * Note:            
 *****************************************************************************/
BYTE MDD_SDSPI_MediaInitialize(void)
{
    WORD timeout;
    BYTE       status = TRUE;
    MMC_RESPONSE    response; 
#if defined USE_16BIT || defined USE_32BIT
    WORD spiconvalue = 0x0003;
#endif
    
    SD_CS = 1;               //Initialize Chip Select line
    
    MMC_ON;
    
    //Media powers up in the open-drain mode and cannot handle a clock faster
    //than 400kHz. Initialize SPI port to slower than 400kHz
#if defined USE_16BIT || defined USE_32BIT
#ifdef USE_PIC32
    SPIBRG = SPICalutateBRG(GetPeripheralClock(), 400000);
    SPICON1bits.MSTEN = 1;
    OpenSPIM (MASTER_ENABLE_ON, BUS_MODE, SMP_PHASE);
    
#else
    // Calculate the prescaler needed for the clock
    timeout = GetSystemClock() / 400000;
    // if timeout is less than 400k and greater than 100k use a 1:1 prescaler
    if (timeout == 0)
    {
        OpenSPIM ((MASTER_ENABLE_ON | PRI_PRESCAL_1_1 | SEC_PRESCAL_1_1), BUS_MODE, SMP_PHASE);
    }
    while (timeout != 0)
    {
        if (timeout > 8)
        {
            spiconvalue--;
            // round up
            if ((timeout % 4) != 0)
                timeout += 4;
            timeout /= 4;
        }
        else
        {
            timeout = 0;
        }
    }
    
    OpenSPIM (MASTER_ENABLE_ON | spiconvalue | ((~(timeout << 2)) & 0x1C), BUS_MODE, SMP_PHASE);
#endif    


    // let the card power on and initialize
    Delayms(1);
    
    //Media requires 80 clock cycles to startup [8 clocks/BYTE * 10 us]
    for(timeout=0; timeout<10; timeout++)
        mSend8ClkCycles();

    SD_CS = 0;
    
    Delayms(1);
    
    // Send CMD0 to reset the media
    response = SendMMCCmd(GO_IDLE_STATE,0x0);
    
    if((response.r1._byte == MMC_BAD_RESPONSE) || ((response.r1._byte & 0xF7) != 0x01))
    {
        status = FALSE;      // we have not got anything back from the card
        SD_CS = 1;                               // deselect the devices
        
        // Turn it off
        MMC_OFF;
        return status;
    }

    // According to spec cmd1 must be repeated until the card is fully initialized
    timeout = 0xFFF;
    
    do
    {
        response = SendMMCCmd(SEND_OP_COND,0x0);
        timeout--;
    }while(response.r1._byte != 0x00 && timeout != 0);

    // see if it failed
    if(timeout == 0)
    {
        status = FALSE;      // we have not got anything back from the card
        
        SD_CS = 1;                               // deselect the devices
        
        // Turn it off
        MMC_OFF;
    }
    else      
    {

#else
    
    // let the card power on and initialize
    Delayms(1);
    
    #if (GetSystemClock() < 25600000)

        #if (GetSystemClock() < 1600000)
            OpenSPIM (SYNC_MODE_FAST, BUS_MODE, SMP_PHASE);
        #elif (GetSystemClock() < 6400000)
            OpenSPIM (SYNC_MODE_MED, BUS_MODE, SMP_PHASE);
        #else
            OpenSPIM (SYNC_MODE_SLOW, BUS_MODE, SMP_PHASE);
        #endif
        
        // let the card power on and initialize
        Delayms(1);
        
        //Media requires 80 clock cycles to startup [8 clocks/BYTE * 10 us]
        for(timeout=0; timeout<10; timeout++)
            mSend8ClkCycles();
    
        SD_CS = 0;
        
        Delayms(1);
        
        // Send CMD0 to reset the media
        response = SendMMCCmd(GO_IDLE_STATE,0x0);
        
        if((response.r1._byte == MMC_BAD_RESPONSE) || ((response.r1._byte & 0xF7) != 0x01))
        {
            status = FALSE;      // we have not got anything back from the card
            SD_CS = 1;                               // deselect the devices
            
            // Turn it off
            MMC_OFF;
            return status;
        }
    
        // According to spec cmd1 must be repeated until the card is fully initialized
        timeout = 0xFFF;
        
        do
        {
            response = SendMMCCmd(SEND_OP_COND,0x0);
            timeout--;
        }while(response.r1._byte != 0x00 && timeout != 0);

    #else

        // Make sure the SPI module doesn't control the bus
        SPICON1 = 0x00;

        //Media requires 80 clock cycles to startup [8 clocks/BYTE * 10 us]
        for(timeout=0; timeout<10; timeout++)
            WriteSPIManual(0xFF);
    
        SD_CS = 0;
        
        Delayms(1);
    
        // Send CMD0 to reset the media
        response = SendMMCCmdManual (GO_IDLE_STATE, 0x0);

        if ((response.r1._byte == MMC_BAD_RESPONSE) || ((response.r1._byte & 0xF7) != 0x01))
        {
            status = FALSE;     // we have not got anything back from the card
            SD_CS = 1;                              // deselect the devices
    
            // Turn it off
            MMC_OFF;
            return status;
        }

        // According to the spec cmd1 must be repeated until the card is fully initialized
        timeout = 0xFFF;
    
        do
        {
            response = SendMMCCmdManual (SEND_OP_COND, 0x0);
            timeout--;
        }while(response.r1._byte != 0x00 && timeout != 0);
    #endif    

    // see if it failed
    if (timeout == 0)
    {
        status = FALSE;     // we have not got anything back from the card

        SD_CS = 1;                              // deselect the devices

        // Turn it off
        MMC_OFF;
    }
    else
    {
#endif

        Delayms (2);

        #ifdef USE_PIC32
            #if (GetSystemClock() <= 20000000)
                SPIBRG = SPICalutateBRG(GetPeripheralClock(), 10000);
            #else
                SPIBRG = SPICalutateBRG(GetPeripheralClock(), 20000000); // SPI Speed is 20MHz 
            #endif
            SPICON1 = 0x0000C060;
            SPICON1bits.MSTEN = 1;
        #else
            OpenSPIM(SYNC_MODE_FAST, BUS_MODE, SMP_PHASE);
        #endif

        // Turn off CRC7 if we can, might be an invalid cmd on some cards (CMD59)
        response = SendMMCCmd(CRC_ON_OFF,0x0);

        // Now set the block length to media sector size. It should be already
        response = SendMMCCmd(SET_BLOCKLEN,MEDIA_SECTOR_SIZE);
        
        for(timeout = 0xFF; timeout > 0 && MDD_SDSPI_SectorRead(0x0,NULL) != TRUE; timeout--)
        {;}

        // see if we had an issue
        if(timeout == 0)
        {
            status = FALSE;
            SD_CS = 1;                               // deselect the devices
            
            // Turn it off
            MMC_OFF;
        }
    }

    return(status);
}//end MediaInitialize

/******************************************************************************
 * Function:        void MDD_SDSPI_ShutdownMedia(void)
 *
 * PreCondition:    None
 *
 * Input:           void
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Turn off the card
 *
 * Note:            None
 *****************************************************************************/

void MDD_SDSPI_ShutdownMedia(void)
{
    // close the spi bus
    CloseSPIM();
    
    // deselect the device
    SD_CS = 1;
    
    // Turn off the card
    MMC_OFF;
}

/******************************************************************************
 * Function:        void SendMMCCmd(BYTE cmd, DWORD address)
 *
 *
 * Input:           None
 *
 * Output:          response            - Response from the card
 *                                      - 0x00 or 0x01 Command received 
 *                                        successfully, else, command failed
 *                  -Bit 0              - In idle state if 1
 *                  -Bit 1              - Erase Reset if 1
 *                  -Bit 2              - Illgal Command if 1
 *                  -Bit 3              - Com CRC Error if 1
 *                  -Bit 4              - Erase Sequence Error if 1
 *                  -Bit 5              - Address Error if 1
 *                  -Bit 6              - Parameter Error if 1
 *                  -Bit 7              - Not used, always '0'
 *
 * Side Effects:    None
 *
 * Overview:        SendMMCCmd prepares the command packet and sends it out
 *                  over the SPI interface. Response data of type 'R1' (see
 *                  SD or MMC product manuals) is returned.
 *
 * Note:            MMC_CS is not set at the end of this function.
 *                  if the command has no data stage, call macro
 *                  mSendMediaCmd_NoData(), it reasserts MMC_CS to 1.
 *                  If the command has a data stage, MMC_CS must be
 *                  reasserted after the data transfer stage is complete.
 *                  See SectorRead and SectorWrite for examples.
 *****************************************************************************/
MMC_RESPONSE SendMMCCmd(BYTE cmd, DWORD address)
{
    WORD timeout = 0x8;
    BYTE index;
    MMC_RESPONSE    response;
    CMD_PACKET  CmdPacket;
    
    SD_CS = 0;                           //Card Select
    
    // Copy over data
    CmdPacket.cmd        = sdmmc_cmdtable[cmd].CmdCode;
    CmdPacket.address    = address;
    CmdPacket.crc        = sdmmc_cmdtable[cmd].CRC;       // Calc CRC here
    
    CmdPacket.TRANSMIT_BIT = 1;             //Set Tranmission bit
    
    WriteSPIM(CmdPacket.cmd);                //Send Command
    WriteSPIM(CmdPacket.addr3);              //Most Significant Byte
    WriteSPIM(CmdPacket.addr2);
    WriteSPIM(CmdPacket.addr1);
    WriteSPIM(CmdPacket.addr0);              //Least Significant Byte
    WriteSPIM(CmdPacket.crc);                //Send CRC
    
    // see if we are going to get a response
    if(sdmmc_cmdtable[cmd].responsetype == R1 || sdmmc_cmdtable[cmd].responsetype == R1b)
    {
        do
        {
            response.r1._byte = MDD_SDSPI_ReadMedia();
            timeout--;
        }while(response.r1._byte == MMC_FLOATING_BUS && timeout != 0);
    }
    else if(sdmmc_cmdtable[cmd].responsetype == R2)
    {
        MDD_SDSPI_ReadMedia();
        
        response.r2._byte1 = MDD_SDSPI_ReadMedia();
        response.r2._byte0 = MDD_SDSPI_ReadMedia();
    }

    if(sdmmc_cmdtable[cmd].responsetype == R1b)
    {
        response.r1._byte = 0x00;
        
        for(index =0; index < 0xFF && response.r1._byte == 0x00; index++)
        {
            timeout = 0xFFFF;
            
            do
            {
                response.r1._byte = MDD_SDSPI_ReadMedia();
                timeout--;
            }while(response.r1._byte == 0x00 && timeout != 0);
        }
    }

    mSend8ClkCycles();                      //Required clocking (see spec)

    // see if we are expecting data or not
    if(!(sdmmc_cmdtable[cmd].moredataexpected))
        SD_CS = 1;

    return(response);
}

#ifdef USE_PIC18
#if (GetSystemClock() >= 25600000)
/******************************************************************************
 * Function:        void SendMMCCmdManual (BYTE cmd, DWORD address)
 *
 *
 * Input:           None
 *
 * Output:          response            - Response from the card
 *                                      - 0x00 or 0x01 Command received 
 *                                        successfully, else, command failed
 *                  -Bit 0              - In idle state if 1
 *                  -Bit 1              - Erase Reset if 1
 *                  -Bit 2              - Illgal Command if 1
 *                  -Bit 3              - Com CRC Error if 1
 *                  -Bit 4              - Erase Sequence Error if 1
 *                  -Bit 5              - Address Error if 1
 *                  -Bit 6              - Parameter Error if 1
 *                  -Bit 7              - Not used, always '0'
 *
 * Side Effects:    None
 *
 * Overview:        SendMMCCmdManual prepares the command packet and sends it 
 *                  out over the SPI interface. Response data of type 'R1' (see
 *                  SD or MMC product manuals) is returned.
 *
 * Note:            None
 *****************************************************************************/
MMC_RESPONSE SendMMCCmdManual(BYTE cmd, DWORD address)
{
    WORD timeout = 0x8;
    BYTE index;
    MMC_RESPONSE    response;
    CMD_PACKET  CmdPacket;
    
    SD_CS = 0;                           //Card Select
    
    // Copy over data
    CmdPacket.cmd        = sdmmc_cmdtable[cmd].CmdCode;
    CmdPacket.address    = address;
    CmdPacket.crc        = sdmmc_cmdtable[cmd].CRC;       // Calc CRC here
    
    CmdPacket.TRANSMIT_BIT = 1;             //Set Tranmission bit
    
    WriteSPIManual(CmdPacket.cmd);                //Send Command
    WriteSPIManual(CmdPacket.addr3);              //Most Significant Byte
    WriteSPIManual(CmdPacket.addr2);
    WriteSPIManual(CmdPacket.addr1);
    WriteSPIManual(CmdPacket.addr0);              //Least Significant Byte
    WriteSPIManual(CmdPacket.crc);                //Send CRC
    
    // see if we are going to get a response
    if(sdmmc_cmdtable[cmd].responsetype == R1 || sdmmc_cmdtable[cmd].responsetype == R1b)
    {
        do
        {
            response.r1._byte = ReadMediaManual();
            timeout--;
        }while(response.r1._byte == MMC_FLOATING_BUS && timeout != 0);
    }
    else if(sdmmc_cmdtable[cmd].responsetype == R2)
    {
        ReadMediaManual();
        
        response.r2._byte1 = ReadMediaManual();
        response.r2._byte0 = ReadMediaManual();
    }

    if(sdmmc_cmdtable[cmd].responsetype == R1b)
    {
        response.r1._byte = 0x00;
        
        for(index =0; index < 0xFF && response.r1._byte == 0x00; index++)
        {
            timeout = 0xFFFF;
            
            do
            {
                response.r1._byte = ReadMediaManual();
                timeout--;
            }while(response.r1._byte == 0x00 && timeout != 0);
        }
    }

    WriteSPIManual(0xFF);                      //Required clocking (see spec)

    // see if we are expecting data or not
    if(!(sdmmc_cmdtable[cmd].moredataexpected))
        SD_CS = 1;

    return(response);
}
#endif
#endif

/******************************************************************************
 * Function:        BYTE MDD_SDSPI_SectorRead(DWORD sector_addr, BYTE *buffer)
 *
 * PreCondition:    None
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be stored, if NULL do
 *                                not store the data anywhere
 *
 * Output:          TRUE      - Sector read
 *               FALSE      - Sector could not be read
 *
 * Side Effects:    None
 *
 * Overview:        SectorRead reads 512 bytes of data from the card starting
 *                  at the sector address specified by sector_addr and stores
 *                  them in the location pointed to by 'buffer'.
 *
 * Note:            The card expects the address field in the command packet
 *                  to be a byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
BYTE MDD_SDSPI_SectorRead(DWORD sector_addr, BYTE* buffer)
{
    WORD index;
    WORD delay;
    MMC_RESPONSE    response;
    BYTE data_token;
    BYTE status = TRUE;
    DWORD   new_addr;
   
#ifdef USB_USE_MSD
    DWORD firstSector;
    DWORD numSectors;
#endif

    // send the cmd
    new_addr = sector_addr << 9;
    response = SendMMCCmd(READ_SINGLE_BLOCK,new_addr);

    // Make sure the command was accepted
    if(response.r1._byte != 0x00)
    {
        response = SendMMCCmd (READ_SINGLE_BLOCK,new_addr);
        if(response.r1._byte != 0x00)
        {
            return FALSE;
        }
    }

    index = 0x2FF;
    
    // Timing delay- at least 8 clock cycles
    delay = 0x40;
    while (delay)
        delay--;
  
    //Now, must wait for the start token of data block
    do
    {
        data_token = MDD_SDSPI_ReadMedia();
        index--;   
        
        delay = 0x40;
        while (delay)
            delay--;

    }while((data_token == MMC_FLOATING_BUS) && (index != 0));

    // Hopefully that zero is the datatoken
    if((index == 0) || (data_token != DATA_START_TOKEN))
    {
        status = FALSE;
    }
    else
    {
#ifdef USB_USE_MSD
        if ((sector_addr == 0) && (buffer == NULL))
            finalLBA = 0x00000000;
#endif

        for(index = 0; index < MEDIA_SECTOR_SIZE; index++)      //Reads in 512-byte of data
        {
            if(buffer != NULL)
            {
#ifdef USE_PIC18
                data_token = SPIBUF;
                SPI_INTERRUPT_FLAG = 0;
                SPIBUF = 0xFF;
                while(!SPI_INTERRUPT_FLAG);
                buffer[index] = SPIBUF;
#else
                SPIBUF = 0xFF;
                while (!SPISTAT_RBF);
                buffer[index] = SPIBUF;
#endif
            }
            else
            {
#ifdef USB_USE_MSD
                if (sector_addr == 0)
                {
                    if ((index == 0x1C6) || (index == 0x1D6) || (index == 0x1E6) || (index == 0x1F6))
                    {
                        firstSector = MDD_SDSPI_ReadMedia();
                        firstSector |= (DWORD)MDD_SDSPI_ReadMedia() << 8;
                        firstSector |= (DWORD)MDD_SDSPI_ReadMedia() << 16;
                        firstSector |= (DWORD)MDD_SDSPI_ReadMedia() << 24;
                        numSectors = MDD_SDSPI_ReadMedia();
                        numSectors |= (DWORD)MDD_SDSPI_ReadMedia() << 8;
                        numSectors |= (DWORD)MDD_SDSPI_ReadMedia() << 16;
                        numSectors |= (DWORD)MDD_SDSPI_ReadMedia() << 24;
                        index += 8;
                            if ((firstSector + numSectors) > finalLBA)
                        {
                            finalLBA = firstSector + numSectors - 1;
                        }
                    }
                    else
                    {
                        MDD_SDSPI_ReadMedia();
                    }
                }
                else
                    MDD_SDSPI_ReadMedia();
#else
                MDD_SDSPI_ReadMedia();
#endif
            }
        }
        // Now ensure CRC
        mReadCRC();               //Read 2 bytes of CRC
        //status = mmcCardCRCError;
    }

    mSend8ClkCycles();            //Required clocking (see spec)

    SD_CS = 1;

    return(status);
}//end SectorRead

/******************************************************************************
 * Function:        BYTE MDD_SDSPI_SectorWrite(DWORD sector_addr, BYTE *buffer, BYTE allowWriteToZero)
 *
 * PreCondition:    None
 *
 * Input:           sector_addr - Sector address, each sector contains 512 bytes
 *                  buffer      - Buffer that data will be read from
 *               allowWriteToZero   - allows write to the MBR sector
 *
 * Output:          TRUE      - Sector written
 *               FALSE      - Sector could not be written
 *
 * Side Effects:    None
 *
 * Overview:        SectorWrite sends 512 bytes of data from the location
 *                  pointed to by 'buffer' to the card starting
 *                  at the sector address specified by sector_addr.
 *
 * Note:            The card expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
BYTE MDD_SDSPI_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero)
{
    WORD            index;
    BYTE            data_response;
#ifdef USE_PIC18
    BYTE            clear;
#endif
    MMC_RESPONSE    response; 
    BYTE            status = TRUE;

    if (sector_addr == 0 && allowWriteToZero == FALSE)
        status = FALSE;
    else
    {
        // send the cmd
        response = SendMMCCmd(WRITE_SINGLE_BLOCK,(sector_addr << 9));
        
        // see if it was accepted
        if(response.r1._byte != 0x00)
            status = FALSE;
        else
        {
            WriteSPIM(DATA_START_TOKEN);                 //Send data start token

            for(index = 0; index < MEDIA_SECTOR_SIZE; index++)      //Send 512 bytes of data
            {
#ifdef USE_PIC18
                clear = SPIBUF;
                SPI_INTERRUPT_FLAG = 0;
                SPIBUF = buffer[index];         // write byte to SSP1BUF register
                while( !SPI_INTERRUPT_FLAG );   // wait until bus cycle complete
                data_response = SPIBUF;         // Clear the SPIBUF
#else
                SPIBUF = buffer[index];
                while (!SPISTAT_RBF);
                data_response = SPIBUF;
#endif
            }

            // calc crc
            mSendCRC();                                 //Send 2 bytes of CRC
            
            data_response = MDD_SDSPI_ReadMedia();                //Read response
            
            if((data_response & 0x0F) != DATA_ACCEPTED)
            {
                status = FALSE;
            }
            else
            {
                index = 0;            //using i as a timeout counter
                
                do                  //Wait for write completion
                {
#ifdef USE_PIC18
                    clear = SPIBUF;
                    SPI_INTERRUPT_FLAG = 0;
                    SPIBUF = 0xFF;
                    while(!SPI_INTERRUPT_FLAG);
                    data_response = SPIBUF;
#else
                    SPIBUF = 0xFF;
                    while(!SPISTAT_RBF);
                    data_response = SPIBUF;
#endif
                    index++;
                }while((data_response == 0x00) && (index != 0));

                if(index == 0)                                  //if timeout first
                    status = FALSE;
            }

            mSend8ClkCycles();
        }

        SD_CS = 1;

    } // Not writing to 0 sector

    return(status);
} //end SectorWrite


/******************************************************************************
 * Function:        BYTE MDD_SDSPI_WriteProtectState(void)
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
BYTE MDD_SDSPI_WriteProtectState(void)
{
    return(SD_WE);
}


/******************************************************************************
 * Function:        BYTE Delayms(void)
 *
 * PreCondition:    None
 *
 * Input:           BYTE millisecons   - Number of ms to delay
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Delays- used for SPI timing
 *
 * Note:            None.
 *****************************************************************************/

void Delayms(BYTE milliseconds)
{
    BYTE    ms;
    DWORD   count;
    
    ms = milliseconds;
    while (ms--)
    {
        count = MILLISECDELAY;
        while (count--);
    }
    Nop();
    return;
}



#if defined USE_16BIT || defined USE_32BIT


/******************************************************************************
 * Function:        BYTE MDD_SDSPI_ReadMedia(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          BYTE    - One byte of data read in from SPI port
 *
 * Side Effects:    None
 *
 * Overview:        ReadMedia reads in one byte of data while sending out 0xFF
 *
 * Note:            Could not use ReadSPI because it initializes SSPBUF to
 *                  0x00. The card expects 0xFF (see spec).
 *****************************************************************************/
#if defined USE_PIC32
BYTE MDD_SDSPI_ReadMedia(void)
{
    putcSPI1((BYTE)0xFF);
    return (BYTE)getcSPI1();
}//end ReadMedia
#else
BYTE MDD_SDSPI_ReadMedia(void)
{
    SPIBUF = 0xFF;                              //Data Out - Logic ones
    while(!SPISTAT_RBF);                     //Wait until cycle complete
    return(SPIBUF);                             //Return with byte read
}//end ReadMedia
#endif

/******************************************************************************
 * Function:        void CloseSPIM (void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Disables the SPI module
 *
 * Note:            None
 *****************************************************************************/

void CloseSPIM( void )
{
    SPISTAT &= 0x7FFF;               // disable synchronous serial port
}


/******************************************************************************
 * Function:        void OpenSPIM( unsigned char sync_mode, unsigned char bus_mode, unsigned char smp_phase)
 *
 * PreCondition:    None
 *
 * Input:           sync_mode   - Sets synchronization
 *               bus_mode   - Sets bus operating mode
 *               smp_phase   - Set sampling phase
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Enables SPI module
 *
 * Note:            None
 *****************************************************************************/

void OpenSPIM( unsigned int sync_mode, unsigned char bus_mode, unsigned char smp_phase)
{
	SPISTAT = 0x0000;               // power on state 

#ifndef USE_PIC32	
	SPICON1 = 0x0000;                // power on state
	SPICON1 |= sync_mode;          // select serial mode 
#endif
	SPICON1 |= smp_phase;          // select data input sample phase

    switch( bus_mode )
    {
        case 0:                       // SPI1 bus mode 0,0
            SPICON1bits.CKE = 1;       // data transmitted on rising edge
            break;
        case 2:                       // SPI1 bus mode 1,0
            SPICON1bits.CKP = 1;       // clock idle state high
            break;
        case 3:                       // SPI1 bus mode 1,1
            SPICON1bits.CKP = 1;       // clock idle state high
            SPICON1bits.CKE = 0;
            break;
        default:                      // default SPI1 bus mode 0,1
            break;
    }

    switch( sync_mode )
    {
        case 4:                       // slave mode w /SS1 enable
            SD_CS_TRIS = 1;       // define /SS1 pin as input
        case 5:                       // slave mode w/o /SS1 enable
            SPICLOCK = 1;       // define clock pin as input
            SPICON1bits.SMP = 0;       // must be cleared in slave SPI mode
            break;
        default:                      // master mode, define clock pin as output
            SPICLOCK = 0;       // define clock pin as output
            break;
    }
    
    SPIOUT = 0;                  // define SDO1 as output (master or slave)
    SPIIN = 1;                  // define SDI1 as input (master or slave)
    SPIENABLE = 1;             // enable synchronous serial port
}

/******************************************************************************
 * Function:        unsigned char WriteSPIM ( unsigned char data_out)
 *
 * PreCondition:    None
 *
 * Input:           data_out   - data to transmit
 *
 * Output:          0 if successful, -1 otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Writes a byte on the SPI
 *
 * Note:            None
 *****************************************************************************/
#if defined USE_PIC32
unsigned char WriteSPIM( unsigned char data_out )
{
    BYTE   clear;
    putcSPI1((BYTE)data_out);
    clear = getcSPI1();
    return ( 0 );                // return non-negative#
}

#else
unsigned char WriteSPIM( unsigned char data_out )
{
    BYTE   clear;
    SPIBUF = data_out;          // write byte to SSP1BUF register
    while( !SPISTAT_RBF ); // wait until bus cycle complete
    clear = SPIBUF;
    return ( 0 );                // return non-negative#
}
#endif
#endif


#ifdef USE_PIC18


/******************************************************************************
 * Function:        BYTE MDD_SDSPI_ReadMedia(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          BYTE    - One byte of data read in from SPI port
 *
 * Side Effects:    None
 *
 * Overview:        ReadMedia reads in one byte of data while sending out 0xFF
 *
 * Note:            Could not use ReadSPI because it initializes SSPBUF to
 *                  0x00. The card expects 0xFF (see spec).
 *****************************************************************************/
BYTE MDD_SDSPI_ReadMedia(void)
{
    unsigned char clear;
    
    clear = SPIBUF;
    SPI_INTERRUPT_FLAG = 0;
    SPIBUF = 0xFF;                              //Data Out - Logic ones
    while(!SPI_INTERRUPT_FLAG);                     //Wait until cycle complete
    return(SPIBUF);                             //Return with byte read
}//end ReadMedia


/******************************************************************************
 * Function:        void OpenSPIM( unsigned char sync_mode, unsigned char bus_mode, unsigned char smp_phase)
 *
 * PreCondition:    None
 *
 * Input:           sync_mode   - Sets synchronization
 *               bus_mode   - Sets bus operating mode
 *               smp_phase   - Set sampling phase
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Enables SPI module
 *
 * Note:            None
 *****************************************************************************/

void OpenSPIM( unsigned char sync_mode, unsigned char bus_mode, unsigned char smp_phase)
{
    SPISTAT &= 0x3F;               // power on state
    SPICON1 = 0x00;                // power on state
    SPICON1 |= sync_mode;          // select serial mode 
    SPISTAT |= smp_phase;          // select data input sample phase
    
    switch( bus_mode )
    {
        case 0:                       // SPI1 bus mode 0,0
            SPISTATbits.CKE = 1;       // data transmitted on rising edge
            break;    
        case 2:                       // SPI1 bus mode 1,0
            SPISTATbits.CKE = 1;       // data transmitted on falling edge
            SPICON1bits.CKP = 1;       // clock idle state high
            break;
        case 3:                       // SPI1 bus mode 1,1
            SPICON1bits.CKP = 1;       // clock idle state high
            break;
        default:                      // default SPI1 bus mode 0,1
            break;
    }

    switch( sync_mode )
    {
        case 4:                       // slave mode w /SS1 enable
            SD_CS_TRIS = 1;       // define /SS1 pin as input
        case 5:                       // slave mode w/o /SS1 enable
            SPICLOCK = 1;       // define clock pin as input
            SPISTATbits.SMP = 0;       // must be cleared in slave SPI mode
            break;
        default:                      // master mode, define clock pin as output
            SPICLOCK = 0;       // define clock pin as output
            break;
    }

    SPIOUT = 0;                  // define SDO1 as output (master or slave)
    SPIIN = 1;                  // define SDI1 as input (master or slave)
    SPICON1bits.SSPEN = 1;             // enable synchronous serial port 
}


/******************************************************************************
 * Function:        unsigned char WriteSPIM ( unsigned char data_out)
 *
 * PreCondition:    None
 *
 * Input:           data_out   - data to transmit
 *
 * Output:          0 if successful, -1 otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Writes a byte on the SPI
 *
 * Note:            None
 *****************************************************************************/

unsigned char WriteSPIM( unsigned char data_out )
{
    unsigned char clear;

    clear = SPIBUF;
    SPI_INTERRUPT_FLAG = 0;
    SPIBUF = data_out;          // write byte to SSP1BUF register
    if ( SPICON1 & 0x80 )       // test if write collision occurred
        return ( -1 );              // if WCOL bit is set return negative #
    else
    {
        while( !SPI_INTERRUPT_FLAG ); // wait until bus cycle complete 
    }
    return ( 0 );                // if WCOL bit is not set return non-negative#
}


#if (GetSystemClock() >= 25600000)
/******************************************************************************
 * Function:        unsigned char WriteSPIManual ( unsigned char data_out)
 *
 * PreCondition:    None
 *
 * Input:           data_out   - data to transmit
 *
 * Output:          0 if successful, -1 otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Writes a byte on the SPI
 *
 * Note:            None
 *
*******************************************************************************/

#define MANUAL_SPI_CLOCK_VALUE             1

unsigned char WriteSPIManual(unsigned char data_out)
{
    char i = data_out;
    unsigned char clock;

    ADCON1 = 0xFF;
    SPICLOCKLAT = 0;
    SPIOUTLAT = 1;
    SPICLOCK = OUTPUT_PIN;
    SPIOUT = OUTPUT_PIN;
    
    if ((SPIOUTPORT != SPIOUTLAT) || (SPICLOCKPORT != SPICLOCKLAT))
        return (-1);

    // Perform loop operation iteratively to reduce discrepancy
    // Bit 7
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x80)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 6
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x40)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;

    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 5
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x20)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 4
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x10)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 3
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x08)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 2
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x04)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 1
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x02)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    // Bit 0
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    if (i & 0x01)
        SPIOUTLAT = 1;
    else
        SPIOUTLAT = 0;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);

    SPICLOCKLAT = 0;

    return 0; 
}


/******************************************************************************
 * Function:        BYTE ReadMediaManual (void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          BYTE    - One byte of data read in from SPI port
 *
 * Side Effects:    None
 *
 * Overview:        ReadMedia reads in one byte of data while sending out 0xFF
 *
 * Note:            Could not use ReadSPI because it initializes SSPBUF to
 *                  0x00. The card expects 0xFF (see spec).
 *****************************************************************************/
BYTE ReadMediaManual (void)
{
    char i, result = 0x00;
    unsigned char clock;

    SPICLOCKLAT = 0;
    SPIOUTLAT = 1;
    SPICLOCK = OUTPUT_PIN;
    SPIOUT = OUTPUT_PIN;
    SPIIN = INPUT_PIN;
    
    if ((SPIOUTPORT != SPIOUTLAT) || (SPICLOCKPORT != SPICLOCKLAT))
        return (-1);

    // Perform loop operation iteratively to reduce discrepancy
    // Bit 7
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x80;

    // Bit 6
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x40;

    // Bit 5
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x20;

    // Bit 4
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);    
    if (SPIINPORT)
        result |= 0x10;

    // Bit 3
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x08;

    // Bit 2
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x04;

    // Bit 1
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x02;

    // Bit 0
    SPICLOCKLAT = 0;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    SPICLOCKLAT = 1;
    clock = MANUAL_SPI_CLOCK_VALUE;
    while (clock--);
    if (SPIINPORT)
        result |= 0x01;

    SPICLOCKLAT = 0;

    return result;
}//end ReadMedia

#endif

/******************************************************************************
 * Function:        void CloseSPIM (void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Disables the SPI module
 *
 * Note:            None
 *****************************************************************************/

void CloseSPIM( void )
{
    SPICON1 &= 0xDF;               // disable synchronous serial port
}



#endif

