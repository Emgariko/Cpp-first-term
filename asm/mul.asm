section         .text

                global          _start
_start:

                sub             rsp, 5 * 128 * 8
                lea             rdi, [rsp + 4 * 128 * 8]
                mov             rcx, 128
                call            read_long
                lea             rdi, [rsp + 3 * 128 * 8]
                call            read_long
                lea             rsi, [rsp + 4 * 128 * 8]
                
                mov             r11, rsp ;r11 - buffer
                lea             r10, [rsp + 128 * 8] ;r10 - result
                call            mul_long_long
                
                mov             rcx, 256 ;because result - it's 256 QWORD's
                mov             rdi, r10
                call            write_long

                mov             al, 0x0a
                call            write_char

                jmp             exit
                
; rdi - first operand
; rsi - second operand
; rcx - long_long length in QWORD's
; r11 - buffer(long_long)
; r10 - result (256 QWORD's)
mul_long_long:
                push            rdi
                push            rsi
                push            rcx 
                
                xor             r14, r14; just a offset counter
                mov             r13, rcx; length of the remaining long_long
                mov             r8, rdi
                mov             r9, r11
                call            copy_long_long
                
                
.loop:
                mov             r8, r11
                mov             r9, rdi
                ;r8->r9 copy
                call            copy_long_long
                
                mov             rbx, [rsi]
                push            rsi                
                call            mul_long_short
                pop             rsi
                lea             rsi, [rsi + 8]
                mov             r8, r10
                mov             r9, rdi
                
                ; add with offset(r14)
                call            add_long_long
                
                inc             r14
                dec             r13
                jnz             .loop
                
                
                pop             rcx
                pop             rsi
                pop             rdi
                ret
                
; copy long_long(rcx QWORD's) from r8 to r9                 
copy_long_long:
                push            r8
                push            r9
                push            rcx
                
.loop:
                mov             rax,  [r8]
                mov             [r9], rax
                lea             r8, [r8 + 8]
                lea             r9, [r9 + 8]
                dec             rcx
                jnz             .loop
                
                pop             rcx
                pop             r9
                pop             r8
                ret
                
; adds two long number
;    r8 -- address of summand #1 (long number)
;    r9 -- address of summand #2 (long number)
;    r14 -- add offset of summand #2
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to r8
add_long_long:
                push            r8
                push            r9
                push            rcx
                clc
.loop:
                mov             rax, [r9]
                lea             r9, [r9 + 8]
                adc             [r8 + r14 * 8], rax
                lea             r8, [r8 + 8]
                dec             rcx
                jnz             .loop
                
                pop             rcx
                pop             r9
                pop             r8
                ret
                
; subtracts two long number
;    rdi -- address of suband #1 (long number)
;    rsi -- address of suband #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    difference is written to rdi
sub_long_long:
                push            rdi
                push            rsi
                push            rcx

                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                sbb             [rdi], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rsi
                pop             rdi
                ret                
                
; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:
                push            rax
                push            rdi
                push            rcx
                push            rsi
                
                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop

                pop             rsi
                pop             rcx
                pop             rdi
                pop             rax
                ret

; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg

