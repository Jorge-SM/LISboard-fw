# LISboard-fw
This is the Firmware for the LISboard pcb.

LISboard is custom tiny board for use with ‘2-position applications’  in robot combat – weapons such as hammers, lifters, flippers. It sits between your receiver and weapon ESC allowing you to add limit switches – stopping your motor stalling and essentially turning your motor into a 2 position servo!

The firmware is saved as arduino file, but uses low level interrupts and register access for better performance.

# What do I need to flash the firmware?
1. First, you need to download and install the Arduino MegaTiny core (https://github.com/SpenceKonde/megaTinyCore)
2. Also, you will need an arduino nano or arduino UNO with the UPDI flashing program installed (https://github.com/SpenceKonde/megaTinyCore/blob/master/MakeUPDIProgrammer.md)
3. Connect the UPDI output of your arduino board to the RESET pin of the LISboard. 
4. Download code
