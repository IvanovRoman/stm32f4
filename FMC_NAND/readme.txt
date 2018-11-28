1. the example is FMC_NAND use for SOM-STM32F429IG.

2. instructions for use:
   (1) Project file path is: \KEIL-MDK\Project.uvproj.
   (2)	\EWARM: includes EWARM (IAR) project and configuration files, not finished.
   (3)	\Keil:	includes RVMDK (Keil)project and configuration files.
   (2) The MDK 4.0 or later to open, The MDK version is too low will lead to not recognize the project.
   (3) Download and debugging tools is Onboard JLINK emulator.
   (4) Use the STM32F4xx_StdPeriph_Driver version is V1.4.
   (5) Connect Micro-USB cable with computer to SOM-STM32F429IG (USB TO USART1), install the CP2102 driver (in tools files),
       open Terminal( in tools files ), and configure the baudrate of 115200 bits, 8 data bit, one stop bit, no parity,
       no flow control.
   (6) Terminal will display information.