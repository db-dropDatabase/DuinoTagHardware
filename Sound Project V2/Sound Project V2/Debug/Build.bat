avrdude -F -v -cusbtiny -b19200 -pt85 -U flash:w:"Sound Project V2.hex":i -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m
pause