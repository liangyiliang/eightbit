#include "rules"
    lda.i 0b00000001
    sta top_bottom_indicator
    jsr clear_display
    ldpi.i 0b00111000  ; initializes the display
    ; jsr lcd_wait
    ldpi.i 0b00001100
    ;jsr lcd_wait
    ldpi.i 0b00000110
    ;jsr lcd_wait

    lda.i 0b11000000
    sta curr_loc_character
    tapi
    ;jsr lcd_wait
    ldpd.i "A"
    ;jsr lcd_wait
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
    ;jsr lcd_wait
    sta curr_loc_character
    jmp end_check
first_line:
    lda.i 0b10000000
    tapi
    ;jsr lcd_wait
    sta curr_loc_character
end_check:
    ldpd.i "A"
    ;jsr lcd_wait
no_change_person:
    jsr change_location
    lda curr_loc_obstacle
    subf curr_loc_character
    jmz game_over
    jmp loop

change_location:         ; moves barricade location to the left, wraps around
    lda top_bottom_indicator
    subf.i 1
    lda curr_loc_obstacle
    sta prev_loc_obstacle
    jmz bottom_row
top_row:
    subf.i 0b10000000    ; ZR means currently at zero
    jmz wrap_around
    jmp after_check
bottom_row:
    subf.i 0b11000000    ; ZR means currently at zero
    jmz wrap_around
after_check:
    sub.i 1              ; Otherwise, subtract 1
    tapi
    ;jsr lcd_wait
    jmp no_wrap_around
wrap_around:
    jsr change_indicator
    lda top_bottom_indicator
    subf.i 1
    jmz bottom_wrap
    lda.i 0b10010000
    jmp after_change_wrap
bottom_wrap:
    lda.i 0b11010000
after_change_wrap:
    tapi
no_wrap_around:
    ldpd.i 0xff           ; draw new marking
    ;jsr lcd_wait
    sta curr_loc_obstacle
    lda prev_loc_obstacle
    tapi
    ;jsr lcd_wait
    ldpd.i " "
    ;jsr lcd_wait
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
    ;jsr lcd_wait
    tsa
    add.i 1
    jmp show
end:
    hlt

clear_display:
    ldpi.i 0b00000001
    ;jsr lcd_wait
    rts

change_indicator:
    tra
    sta top_bottom_indicator
    rts

wait:
    lda.i 0
outer_loop:
    add.i 1
    subf.i num_waits_inner
    jmz end_outer_loop
end_outer_loop:
    rts

prev:
    #d 0x00
curr_loc_obstacle:
    #d 0b11010000
prev_loc_obstacle:
    #d 0b11010001
curr_loc_character:
    #d 0b11000000
game_over_text_addr:
    #d "GAME OVER!\0"
top_bottom_indicator:
    #d 0b00000001 ; bottom = 1
num_waits_inner:
    #d 255`8

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