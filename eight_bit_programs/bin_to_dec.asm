#include "rules"

    lda.i 0b01011011               ; Upper bits
    sta curr_value
    lda.i 0b11001010               ; Lower bits
    ;out
    sta curr_value + 1
    lda.i neg_divisors
    sta current_neg_divisor_address
    lda.i 0
    sta digit
    
    ldpi.i 0b00000001 ; the following lines configure the display
    ; jsr lcd_wait
    ldpi.i 0b00111000
    ; jsr lcd_wait
    ldpi.i 0b00001110
    ; jsr lcd_wait
    ldpi.i 0b00000110
    ; jsr lcd_wait

    jsr print_digit
    jsr print_digit
    jsr print_digit
    jsr print_digit
    jsr print_digit
    hlt

print_digit:
    lda.i 0
    sta digit
get_first_digit_loop:
    lda curr_value                    ; loads current value to Operand 1
    sta add_operand_1_upper
    lda curr_value + 1
    ; out
    sta add_operand_1_lower

    lda current_neg_divisor_address   ; loads negative divisor to Operand 2
    lda.rel 0
    sta add_operand_2_upper
    lda current_neg_divisor_address
    lda.rel 1
    sta add_operand_2_lower

    jsr add_with_carry                ; Computes (current value - divisor)

    lda add_result_upper              ; Check sign of result
    subf.i 0
    jmn negative_result               ; If result is negative, we are done.
    lda digit                         ; Otherwise, digit += 1 and save the new values
    add.i 1
    sta digit
    lda add_result_upper
    sta curr_value
    lda add_result_lower
    sta curr_value + 1
    jmp get_first_digit_loop
negative_result:
    lda current_neg_divisor_address
    add.i 2
    sta current_neg_divisor_address
    lda digit
    ; out
    out
    add.i "0"
    tapd
    ; jsr lcd_wait
    rts
digit:
    #d 0x00

curr_value:
    #d 0x00
    #d 0x00

neg_divisors:
    #d 0b11011000  ; -10000 upper
    #d 0b11110000  ;        lower
    #d 0b11111100  ; -1000  upper
    #d 0b00011000  ;        lower
    #d 0b11111111  ; -100   upper
    #d 0b10011100  ;        lower
    #d 0b11111111  ; -10    upper
    #d 0b11110110  ;        lower
    #d 0b11111111  ; -1     upper
    #d 0b11111111  ;        lower
current_neg_divisor_address:
    #d neg_divisors`8



add_with_carry:
    lda add_operand_1_lower
    add add_operand_2_lower
    sta add_result_lower
    jmc carry_happened
    lda add_operand_1_upper
    add add_operand_2_upper
    sta add_result_upper
    jmp after_carry
carry_happened:
    lda add_operand_1_upper
    add add_operand_2_upper
    add.i 1
    sta add_result_upper
after_carry:
    rts
add_operand_1_lower:
    #d 0x00
add_operand_1_upper:
    #d 0x00
add_operand_2_lower:
    #d 0x00
add_operand_2_upper:
    #d 0x00
add_result_lower:
    #d 0x00
add_result_upper:
    #d 0x00

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