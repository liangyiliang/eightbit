#include "rules"

start:
    lda.i 0
    sta p1
    sta p2
    sta p3
    lda.i 1
comp:
    out
    sta p1
    lda p2
    sta p3
    lda p1
    sta p2
    add p3
    jmc start
    jmp comp

p1:
    #d 0x00
p2:
    #d 0x00
p3:
    #d 0x00