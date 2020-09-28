%include "rw32-2018.inc"

section .data
    deset dq 10.0
    a dq 10.0
    b dq 10.0
    c dq 10.0
    x dq 10.0

quad:
%define x esp+28
%define c esp+20
%define b esp+12
%define a esp+4

    fld qword [a]  ; a
    fmul qword [x] ; ax
    fmul qword [x] ; ax^2
    fld qword [b]  ; b    ax^2
    fmul qword [x] ; bx   ax^2
    faddp          ; bx+ax^2
    fadd qword [c] ; c+bx+ax^2

    ret




section .text
CMAIN:
    push ebp     ; save the value of base pointer
    mov ebp, esp ; base pointer now points to the top of the stack
    
    
    finit
    
    sub esp,8
    mov dword [esp],4
    fild dword [esp]
    fstp qword [esp]
    
    sub esp,8
    mov dword [esp],-1
    fild dword [esp]
    fstp qword [esp]

    sub esp,8
    mov dword [esp],-5
    fild dword [esp]
    fstp qword [esp]
    
    sub esp,8
    mov dword [esp],10
    fild dword [esp]
    fstp qword [esp]
    
    call quad
    add esp,32
    
    call WriteDouble
    call WriteNewLine

    mov esp, ebp ; set the top of the stack to old value (current base pointer)
    pop ebp      ; set the base pointer to old value (saved in stack)
    ret
