avrdude.exe -F -v  -cusbtiny -pt85  -b19200 -U flash:w:WHYNOT.hex:i -U lfuse:w:0xd2:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m
pause