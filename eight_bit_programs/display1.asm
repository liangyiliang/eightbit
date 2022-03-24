#include "rules"

    ldpi.i 0b00000001
    ldpi.i 0b00111000
    ldpi.i 0b00001110
    ldpi.i 0b00000110

    lda.i data ; loads the address of "data" to A
loop:
    tas ; Reg A --> Stack Register
    lda.rel 0 ; Reg A <-- MEM[RegA + 0]
    add.i 0 ; Adds 0 to A, sets flags
    jmz end ; Ends if string content = 0
    tapd
    tsa ; Stack Register <-- Reg A
    add.i 1 ; Reg A += 1
    jmp loop
end:
    hlt
data:
    #d "hello world\0"
