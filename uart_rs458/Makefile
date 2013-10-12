
default: main.hex

main.hex: main.c
	msp430-gcc -Os -mmcu=msp430g2553 -o main.elf main.c
	msp430-objdump -DS main.elf > main.lst
	msp430-objcopy -O ihex main.elf main.hex

load: main.hex
	mspdebug rf2500 "prog main.hex"

