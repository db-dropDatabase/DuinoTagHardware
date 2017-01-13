avrdude -F -v -cusbtiny -b19200 -pt25 -U flash:w:"WilsonPCBBitBang.hex":i -U lfuse:w:0x43:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
pause