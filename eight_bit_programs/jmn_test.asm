#include "rules"
    lda.i -10
    subf.i 0
    jmn test
    lda.i 2
    out
    hlt
test:
    lda.i 1
    out
    hlt