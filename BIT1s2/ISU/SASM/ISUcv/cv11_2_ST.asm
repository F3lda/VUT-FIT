%include "rw32-2020.inc"

section .data

    a dd 2.0
    b dd 1.5
    S dd 0
    dva dd 2.0
    
    const dd 3.0

const2 dd 2.0

const3 dd 4.0

section .text

plocha:
; __cdecl double plocha(double a)
; S = a*a*(44 + 5*pi)/72
    enter 0,0

    finit
    fld1           ; 1
    fadd st0,st0   ; 2
    fld1           ; 1 2
    fld st1        ; 2 1 2
    fld st0        ; 2 2 1 2
    fadd st0,st0   ; 4 2 1 2
    fadd st1,st0   ; 4 6 1 2
    fadd st2,st0   ; 4 6 5 2
    fld st2        ; 5 4 6 5 2
    fadd st4,st0   ; 5 4 6 5 7
    faddp st2,st0  ; 4 11 5 7
    fmul st3,st0   ; 4 11 5 28
    fmulp          ; 44 5 28
    fadd st2,st0   ; 44 5 72
    fldpi          ; pi 44 5 72
    fmulp st2,st0  ; 44 5pi 72
    faddp          ; 44+5pi 72
    fdivrp         ; 44+5pi/72
    
    fld qword [ebp+8]
    fmul st0,st0
    fmulp

    leave
    ret

plocha_mem:
; __cdecl double plocha_mem(double a)
; S = a*a*(44 + 5*pi)/72
    enter 0,0

    finit
    fld qword [ebp+8]; a
    fmul st0,st0     ; a*a
    push dword 72
    fidiv dword [esp]; a*a/72
    fldpi            ; pi a*a/72
    push dword 5
    fimul dword [esp]; 5*pi a*a/72
    push dword 44
    fiadd dword [esp]; 44+5*pi a*a/72
    fmulp            ; (44+5*pi)*a*a/72

    leave
    ret

plocha_opt:
; __cdecl double plocha_opt(double a)
; S = a*a*(44 + 5*pi)/72 = a*a*MAGIC
; ?0,82927726761040230822657245717219??
; 64bit IEE754... 3fea8970_7af67e4e

    push 0x3fea8970    ; uložíme MAGIC
    push 0x7af67e4e
    
    finit
    fld qword [esp+12] ; a
    fmul st0,st0       ; a*a
    fmul qword [esp]   ; a*a*MAGIC

    add esp,8
    ret

plocha_stud:
ret

CMAIN:
    push ebp
    mov ebp, esp
    
    finit
    sub esp,8
    mov [esp],dword 3
    fild dword [esp]
    fstp qword [esp]
    call plocha
;    add esp,8
    call WriteDouble
    call WriteNewLine    
    
;    sub esp,8
;    mov [esp],dword 3
;    fild dword [esp]
;    fstp qword [esp]
    call plocha_mem
;    add esp,8
    call WriteDouble
    call WriteNewLine    

;    sub esp,8
;    mov [esp],dword 3
;    fild dword [esp]
;    fstp qword [esp]
    call plocha_opt
;    add esp,8
    call WriteDouble
    call WriteNewLine

    finit
;    sub esp,8
;    mov [esp],dword 3
;    fild dword [esp]
;    fstp qword [esp]
    call plocha_stud
    add esp,8
    call WriteDouble
    call WriteNewLine    

    xor eax,eax
    pop ebp
    ret