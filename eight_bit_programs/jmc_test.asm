#include "rules"
    lda.i 255
    add.i 1
    jmc test
    lda.i 2
    out
    hlt
test:
    lda.i 1
    out
    hlt