1. the example is GLCD use for SOM-STM32F429IG.

2. instructions for use:
   (1) Project file path is: \KEIL-MDK\Project.uvproj.
   (2)	\EWARM: includes EWARM (IAR) project and configuration files, not finished.
   (3)	\Keil:	includes RVMDK (Keil)project and configuration files.
   (2) The MDK 4.0 or later to open, The MDK version is too low will lead to not recognize the project.
   (3) Download and debugging tools is Onboard JLINK emulator.
   (4) Use the STM32F4xx_StdPeriph_Driver version is V1.4.
   (5) you can choose LCD Type in GLCD.h file, define USED_LCD, 
   (6) Run example, watch the LCD display.