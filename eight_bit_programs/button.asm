#include "rules"

    ldpi.i 0b00000001
    ldpi.i 0b00111000
    ldpi.i 0b00001100
    ldpi.i 0b00000110
    ldpi.i 0b11000000
    ldpd.i "A"
loop:
    tia
    subf prev
    jmz no_change
    ldpi.i 0b00000001
    sta prev
    subf.i 1
    jmz first_line
    ldpi.i 0b11000000 ; Writes a instruction to the second line
    jmp end_check
first_line:
    ldpi.i 0b10000000
end_check:
    ldpd.i "A"
no_change:
    jmp loop
prev:
    #d 0x00