#include "rules"

    lda.i 0b10101010
    add.i 1
    sta mem
    lda.i 0
    sub mem
    out ; should output 85 to number display
    sta mem
    jsr initialize_display
    lda.i 0
print_text:
    tas
    lda.rel text_addr
    subf.i 0
    jmz end_text
    tapd
    tsa
    add.i 1
    jmp print_text
end_text:
    ldpi.i 0b11000000 ; Writes a instruction to the second line
    lda.i 0
    tas
    psh
loop:
    pop
    subf.i 8
    jmz end
    add.i 1
    psh
    jsr print_one
    jmp loop
end:
    hlt
mem:
    #d 0x00
digit:
    #d 0x00

text_addr:
    #d "Complement:\0"

print_one:
    lda mem
    add mem
    sta mem
    jmc put_1
    ldpd.i "0"
    jmp after_put
put_1:
    ldpd.i "1"
after_put:
    rts

initialize_display:
    ldpi.i 0b00000001 ; the following lines configure the display
    ldpi.i 0b00111000
    ldpi.i 0b00001110
    ldpi.i 0b00000110
    rts