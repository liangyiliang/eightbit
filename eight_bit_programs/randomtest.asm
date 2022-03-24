#include "rules"

    sta current_random_offset
loop:
    lda current_random_offset
    add.i 1
    sta current_random_offset
    lda.rel unused
    sta current_random_value
    add current_random_value
    jmc generate_1
    lda.i 0
    out
    jmp after_generate
generate_1:
    lda.i 1
    out
after_generate:
    jmp loop


current_random_offset:
    #d 0x00
current_random_value:
    #d 0x00
unused:
    #d 0xff