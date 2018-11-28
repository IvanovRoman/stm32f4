1. the example is EMWIN STemWin version 5.22D DEMO Without RTOS use for SOM-STM32F429IG.

2. instructions for use:
   (1) Project file path is: \KEIL-MDK\Project.uvproj.
   (2)	\EWARM: includes EWARM (IAR) project and configuration files, not finished.
   (3)	\Keil:	includes RVMDK (Keil)project and configuration files.
   (2) The MDK 4.0 or later to open, The MDK version is too low will lead to not recognize the project.
   (3) Download and debugging tools is Onboard JLINK emulator.
   (4) Use the STM32F4xx_StdPeriph_Driver version is V1.4.
   (5) PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
   (5) you can choose LCD Type in GLCD.h file, define USED_LCD, LCD can be support, resistance touch 4.3 inch(480*272),
       5 inch(480*272 & 800*480), 7 inch(800*480), capacitive touch 7 inch(800*480), 7 inch(1024*600), 10.1 inch(1024*600).
   (6) Run example, select your target, on the right of the "Load" button, watch the LCD display.