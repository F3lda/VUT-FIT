%include "rw32-2018.inc"

section .data
    task21A dw 192,-32,-48,96,-80,0,-960,-944

section .text
CMAIN:
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; STACK FRAME
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    push ebp     ; save the value of base pointer
    mov ebp, esp ; base pointer now points to the top of the stack
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    push 10
    push 11
    push 12    
    
    pop eax
    call WriteInt32
    call WriteNewLine

    pop eax
    call WriteInt32
    call WriteNewLine
    
    pop eax
    call WriteInt32
    call WriteNewLine
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    sub esp, 12  ; space allocated on the stack for the local variables
    
    mov [ebp -  4], dword 10  ; location of variable a
    mov [ebp -  8], dword 5   ; location of b
    mov [ebp - 12], dword 2   ; location of c
    
    mov eax, [ebp -  4];
    call WriteInt32
    call WriteNewLine

    mov eax, [ebp -  8];
    call WriteInt32
    call WriteNewLine
    
    mov eax, [ebp -  12];
    call WriteInt32
    call WriteNewLine
    
    
    pop eax
    call WriteInt32
    call WriteNewLine

    pop eax
    call WriteInt32
    call WriteNewLine
    
    pop eax
    call WriteInt32
    call WriteNewLine
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; STACK FRAME - END
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov esp, ebp ; set the top of the stack to old value (current base pointer)
    pop ebp      ; set the base pointer to old value (saved in stack)
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ret
;
;:    : 
;|  2 | [ebp + 16] (3rd function argument)
;|  5 | [ebp + 12] (2nd argument)
;| 10 | [ebp + 8]  (1st argument)
;| RA | [ebp + 4]  (return address)
;| FP | [ebp]      (old ebp value)
;|    | [ebp - 4]  (1st local variable)
;|    | [ebp - 8]  (2nd local variable)
;:    :
;:    :
;|    | [ebp - X]  (esp - the current stack pointer. The use of push / pop is valid now)
;
; https://en.m.wikibooks.org/wiki/X86_Disassembly/Functions_and_Stack_Frames