#include "rules"

    ldpi.i 0b00000001 ; the following lines configure the display
    ldpi.i 0b00111000 ; 111 vs 110
    ldpi.i 0b00001110
    ldpi.i 0b00000110

    lda.i data1 ; loads the address of "data" to A
loop1:
    tas ; Reg A --> Stack Register
    lda.rel 0 ; Reg A <-- MEM[RegA + 0]
    add.i 0 ; Adds 0 to A, sets flags
    jmz end_loop1 ; Ends if string content = 0
    tapd
    tsa ; Stack Register <-- Reg A
    add.i 1 ; Reg A += 1
    jmp loop1
end_loop1:
    ldpi.i 0b11000000
    lda.i data2
loop2:
    tas ; Reg A --> Stack Register
    lda.rel 0 ; Reg A <-- MEM[RegA + 0]
    add.i 0 ; Adds 0 to A, sets flags
    jmz end_loop2 ; Ends if string content = 0
    tapd
    tsa ; Stack Register <-- Reg A
    add.i 1 ; Reg A += 1
    jmp loop2
end_loop2:
    hlt
data1:
    #d "hello !!!!! It i\0"
data2:
    #d "s working!\0"

