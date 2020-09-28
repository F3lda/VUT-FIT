%include "rw32-2018.inc"

section .data
    ; zde budou vase data

section .text
CMAIN:
    push ebp
    mov ebp, esp
    
    mov ax, 76
    ;call WriteBin16
    
    MOV AL, 76
 
    AND AX, 255; AL = 76 = 01001100
        call WriteInt8
    call WriteNewLine
    OR AL, -127; AL = -51
        call WriteInt8
    call WriteNewLine
    XCHG AL, DL	 
    ROL AX, 8	 ; AL = AH
    XOR AL, DL	 
    SAR AL, 4; AL = -4
    call WriteInt8
    call WriteNewLine

    pop ebp
    ret