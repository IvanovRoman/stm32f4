echo off

REM -------- This bat file is used to merge multiple BIN files into one files

echo on

REM -------- generate 16M byte generated file
LinkBin NorFlash(16M).bin =0x01000000 @0x0 ��������ֿ�.bin @0x00200000 480x272_��̲.bin @0x0023FC00 480x272_��Ҷ.bin @0x0027F800 480x272_ˮ��.bin @0x002BF400 800x480_��̲.bin @0x0037AC00 800x480_��Ҷ.bin @0x00436400 800x480_ˮ��.bin

REM -------- generate variable length file
LinkBin NorFlash.bin =VAR @0x0 ��������ֿ�.bin @0x00200000 480x272_��̲.bin @0x0023FC00 480x272_��Ҷ.bin @0x0027F800 480x272_ˮ��.bin @0x002BF400 800x480_��̲.bin @0x0037AC00 800x480_��Ҷ.bin @0x00436400 800x480_ˮ��.bin

pause