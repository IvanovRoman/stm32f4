echo off

REM -------- This bat file is used to merge multiple BIN files into one files

echo on

REM -------- generate 16M byte generated file
LinkBin NorFlash(16M).bin =0x01000000 @0x0 宋体点阵字库.bin @0x00200000 480x272_海滩.bin @0x0023FC00 480x272_树叶.bin @0x0027F800 480x272_水珠.bin @0x002BF400 800x480_海滩.bin @0x0037AC00 800x480_树叶.bin @0x00436400 800x480_水珠.bin

REM -------- generate variable length file
LinkBin NorFlash.bin =VAR @0x0 宋体点阵字库.bin @0x00200000 480x272_海滩.bin @0x0023FC00 480x272_树叶.bin @0x0027F800 480x272_水珠.bin @0x002BF400 800x480_海滩.bin @0x0037AC00 800x480_树叶.bin @0x00436400 800x480_水珠.bin

pause