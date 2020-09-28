%include "rw32-2018.inc"

section .data
    ; zde budou vase data

section .text
CMAIN:
    push ebp
    mov ebp, esp
    
    MOV AL, 24
 
    MOV BL, 8
 
    MOV CL, 40
 
    MUL CL	; AX = 960
    call WriteInt16
    call WriteNewLine
    ADD AH, BL	 
    MOV DL, 100	 
    DIV DL	; AX = 2078
    call WriteInt16
    call WriteNewLine
    INC AX	 
    ADD AL, AH	; AL = 39
    call WriteInt8
    call WriteNewLine
    SUB AL, BL	 
    ;call WriteInt8
    ;call WriteNewLine
    CMP AL, -97
 
    JAE hop1	 
    ADD AL, -128
 
hop1:	 
; AL = -97
    call WriteInt8
    call WriteNewLine

    pop ebp
    ret