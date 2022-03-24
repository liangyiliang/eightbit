#include "rules"

    jsr lcd_wait
    ldpi.i 0b00000001
    jsr lcd_wait
    ldpi.i 0b00111000
    jsr lcd_wait
    ldpi.i 0b00001110
    jsr lcd_wait
    ldpi.i 0b00000110
    jsr lcd_wait

    lda.i data ; loads the address of "data" to A
loop:
    tas ; Reg A --> Stack Register
    lda.rel 0 ; Reg A <-- MEM[RegA + 0]
    add.i 0 ; Adds 0 to A, sets flags
    jmz end ; Ends if string content = 0
    tapd
    jsr lcd_wait
    tsa ; Stack Register <-- Reg A
    add.i 1 ; Reg A += 1
    jmp loop
end:
    hlt
data:
    #d "hello world\0"


lcd_wait:
    sta temp
lcd_wait_loop:
    tpia
    subf.i 0b10000000
    jmn lcd_wait_done
    jmp lcd_wait_loop
lcd_wait_done:
    lda temp
    rts
temp:
    #d 0x00