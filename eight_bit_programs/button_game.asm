#include "rules"

    jsr clear_display
    ldpi.i 0b00111000  ; initializes the display
    ldpi.i 0b00001100
    ldpi.i 0b00000110
    lda.i 0b11000000
    sta curr_loc_character
    ldpi.i 0b11000000
    ldpd.i "A"
    lda.i 0b11010000
    sta curr_loc_obstacle
loop:
    tia
    subf prev
    jmz no_change_person
    jsr clear_display
    sta prev
    subf.i 1
    jmz first_line
    lda.i 0b11000000
    tapi
    sta curr_loc_character
    jmp end_check
first_line:
    ldpi.i 0b10000000
    sta curr_loc_character
end_check:
    ldpd.i "A"
no_change_person:
    jsr change_location
    lda curr_loc_obstacle
    subf curr_loc_character
    jmz game_over
    jmp loop

change_location:         ; moves barricade location to the left, wraps around
    lda curr_loc_obstacle
    tapi
    ldpd.i " "            ; erase previous marking
    subf.i 0b11000000    ; ZR means currently at zero
    jmz wrap_around      ; If ZR, wrap around
    sub.i 1              ; Otherwise, subtract 1
    tapi
    jmp no_wrap_around
wrap_around:
    lda.i 0b11010000
    tapi
no_wrap_around:
    sta curr_loc_obstacle
    lda.i 0xff           ; draw new marking
    tapd
    rts

game_over:               ; shows the game-over display
    lda.i 0
    jsr clear_display
show:
    tas
    lda.rel game_over_text_addr
    subf.i 0
    jmz end
    tapd
    tsa
    add.i 1
    jmp show
end:
    hlt

clear_display:
    ldpi.i 0b00000001
    rts

prev:
    #d 0x00
curr_loc_obstacle:
    #d 0b11010000
curr_loc_character:
    #d 0b11000000
game_over_text_addr:
    #d "GAME OVER!\0"