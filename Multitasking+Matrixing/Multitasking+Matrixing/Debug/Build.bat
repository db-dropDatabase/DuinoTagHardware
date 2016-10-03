avrdude.exe -F -v -cusbtiny -b19200 -pt85 -U flash:w:"Multitasking+Matrixing.hex":i -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
pause