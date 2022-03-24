#include "rules"

    lda.i 0
    add.i 0b11111111
    out
    sta val
    lda.i 20
    out
    lda val
    out
    hlt
val:
    #d 0x00