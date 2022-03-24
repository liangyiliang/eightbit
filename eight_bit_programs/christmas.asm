#include "rules"

    ldpi.i 0b00000001 ; the following lines configure the display
    ldpi.i 0b00111000
    ldpi.i 0b00001110
    ldpi.i 0b00000110
loop:
    tas
    subf.i 16 ; Compute A - 16, sets flags, does not store result
    jmz line_break ; If A = 16, do a line break
    jmp no_line_break ; Otherwise, don't.
line_break:
    ldpi.i 0b11000000 ; Writes a instruction to the second line
no_line_break:
    lda.rel data_addr ; Loads Reg A <-- MEM[Reg A + data_addr]
    subf.i 0 ; Sets flags to detect \0
    jmz end
    tapd
    tsa
    add.i 1
    jmp loop
end:
    hlt


data_addr:
    #d "Happy New Year!     Happy 2022!\0"

