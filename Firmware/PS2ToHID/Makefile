# MPLAB IDE generated this makefile for use with GNU make.
# Project: USB Device - HID - joystick_8_24.mcp
# Date: Mon Mar 16 00:29:12 2020

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

USB\ Device\ -\ HID\ -\ joystick_8_24.cof : usb_descriptors.o usb_device.o usb_function_hid.o main.o psx.o gameport.o
	$(LD) /p18F4550 /l"C:\Program Files (x86)\Microchip\mplabc18\v3.46\lib" "usb_descriptors.o" "usb_device.o" "usb_function_hid.o" "main.o" "psx.o" "gameport.o" /u_CRUNTIME /u_DEBUG /u_DEBUGCODESTART=0x7dc0 /u_DEBUGCODELEN=0x240 /u_DEBUGDATASTART=0x3f6 /u_DEBUGDATALEN=0xa /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /z__MPLAB_DEBUGGER_ICD3=1 /z__ICD2RAM=1 /m"USB Device - HID - joystick_8_24.map" /w /o"USB Device - HID - joystick_8_24.cof"

usb_descriptors.o : usb_descriptors.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdlib.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/string.h Microchip/Include/USB/usb_device.h usb_descriptors.c Microchip/Include/GenericTypeDefs.h Microchip/Include/Compiler.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f4550.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stddef.h usb_config.h Microchip/Include/USB/usb_ch9.h Microchip/Include/USB/usb_hal.h Microchip/Include/USB/usb_hal_pic18.h Microchip/Include/USB/usb_function_hid.h
	$(CC) -p=18F4550 /i".\Microchip\Include" -I".\Microchip\Usb" "usb_descriptors.c" -fo="usb_descriptors.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -pa=4

usb_device.o : Microchip/Usb/usb_device.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdlib.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/string.h Microchip/Include/usb/usb_common.h Microchip/Include/usb/usb_device.h Microchip/Usb/usb_device.c Microchip/Include/GenericTypeDefs.h Microchip/Include/Compiler.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f4550.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stddef.h Microchip/Include/USB/usb_ch9.h Microchip/Include/USB/USB.h usb_config.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/limits.h Microchip/Include/usb/usb_ch9.h Microchip/Include/usb/usb_hal.h Microchip/Include/USB/usb_hal_pic18.h Microchip/Include/USB/usb_hal.h Microchip/Include/USB/usb_device.h HardwareProfile.h
	$(CC) -p=18F4550 /i".\Microchip\Include" -I".\Microchip\Usb" "Microchip\Usb\usb_device.c" -fo="usb_device.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -pa=4

usb_function_hid.o : Microchip/Usb/HID\ Device\ Driver/usb_function_hid.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdlib.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/string.h Microchip/Include/USB/usb_device.h Microchip/Usb/HID\ Device\ Driver/usb_function_hid.c Microchip/Include/GenericTypeDefs.h Microchip/Include/Compiler.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f4550.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stddef.h usb_config.h Microchip/Include/USB/usb_ch9.h Microchip/Include/USB/usb_hal.h Microchip/Include/USB/usb_hal_pic18.h Microchip/Include/USB/usb_function_hid.h
	$(CC) -p=18F4550 /i".\Microchip\Include" -I".\Microchip\Usb" "Microchip\Usb\HID Device Driver\usb_function_hid.c" -fo="usb_function_hid.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -pa=4

main.o : main.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdio.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdlib.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/string.h Microchip/Include/USB/usb_device.h Microchip/Include/usb/usb_common.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/spi.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/timers.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/delays.h psx.h main.c portdef.h Microchip/Include/GenericTypeDefs.h Microchip/Include/Compiler.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f4550.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stdarg.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/stddef.h usb_config.h Microchip/Include/USB/usb_ch9.h Microchip/Include/USB/usb_hal.h Microchip/Include/USB/usb_hal_pic18.h Microchip/Include/USB/usb.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/limits.h Microchip/Include/usb/usb_ch9.h Microchip/Include/usb/usb_hal.h Microchip/Include/usb/usb_device.h HardwareProfile.h gameport.h Microchip/Include/USB/usb_function_hid.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/pconfig.h PicCfg.h
	$(CC) -p=18F4550 /i".\Microchip\Include" -I".\Microchip\Usb" "main.c" -fo="main.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -pa=4

psx.o : psx.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/spi.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/timers.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/delays.h psx.h psx.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f4550.h Microchip/Include/GenericTypeDefs.h portdef.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/pconfig.h
	$(CC) -p=18F4550 /i".\Microchip\Include" -I".\Microchip\Usb" "psx.c" -fo="psx.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -pa=4

gameport.o : gameport.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/delays.h gameport.c C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h C:/Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f4550.h Microchip/Include/GenericTypeDefs.h HardwareProfile.h gameport.h portdef.h
	$(CC) -p=18F4550 /i".\Microchip\Include" -I".\Microchip\Usb" "gameport.c" -fo="gameport.o" -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -pa=4

clean : 
	$(RM) "usb_descriptors.o" "usb_device.o" "usb_function_hid.o" "main.o" "psx.o" "gameport.o" "USB Device - HID - joystick_8_24.cof" "USB Device - HID - joystick_8_24.hex" "USB Device - HID - joystick_8_24.map"

