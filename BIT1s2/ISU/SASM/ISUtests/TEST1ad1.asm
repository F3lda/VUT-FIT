%include "rw32-2018.inc"

section .data


section .text
CMAIN:
    push ebp
    mov ebp,esp
    
    
    xor edx,edx
    xor eax,eax
    xor ebx,ebx
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; -8/2 = -4
    mov eax, -8
    mov edx, eax
    sar edx, 16
    
    mov bx, 2
    idiv bx
    
    call WriteInt16
    call WriteNewLine
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; -8/2 = -4
    mov ax, -8
    cwd
    
    mov bx, 2
    idiv bx
    
    call WriteInt16
    call WriteNewLine
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    xor edx,edx
    xor eax,eax
    xor ebx,ebx
    
    mov ax, -8
    mov bl, 2
    idiv bl
    
    call WriteNewLine
    call WriteInt8
    call WriteNewLine
    call WriteInt16
    call WriteNewLine
    cbw
    call WriteInt16
    call WriteNewLine
    
    call WriteInt32
    call WriteNewLine
    cwde
    call WriteInt32
    call WriteNewLine
  

    pop ebp
    ret