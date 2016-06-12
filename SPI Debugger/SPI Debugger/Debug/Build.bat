avrdude -F -v -cusbtiny -b19200 -pm328p -U flash:w:"SPI Debugger.hex":i -U lfuse:w:0xE2:m -U hfuse:w:0xDA:m -U efuse:w:0x07:m -U lock:w:0x3F:m
pause