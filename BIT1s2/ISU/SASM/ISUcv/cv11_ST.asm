%include "rw32-2020.inc"

section .data

    deset dq 10.0
    fstr db "%f",EOL,0

section .text

quad:
; __cdecl double quad(double a, double b, double c, double x)
; quad(a,b,c,x) = ax^2 + bx + c

%define x esp+28
%define c esp+20
%define b esp+12
%define a esp+4
    finit
    fld qword [a] ; a
    fmul qword [x] ; ax
    fmul qword [x] ; ax^2
    fld qword [b] ; b ax^2
    fmul qword [x] ;bx ax^2
    faddp          ;bx+ax^2
    fadd qword [c] ;c+bx+ax^2    
    ret

quad_2:
; __cdecl double quad(double a, double b, double c, double x)
; quad(a,b,c,x) = ax^2 + bx + c

%define x ebp+32
%define c ebp+24
%define b ebp+16
%define a ebp+8
    enter 0,0
    finit
    fld qword [x]  ; x 
    fld st0        ; x    x 
    fmul st0,st0   ; x^2  x
    fmul qword [a] ; ax^2 x
    fld qword [b]  ; b    ax^2  x
    fmulp st2,st0  ; ax^2 bx
    faddp          ; bx+ax^2
    fadd qword [c] ; c+bx+ax^2
    leave
    ret

obsah_kruhu:
; __cdecl double obsah_kruhu(float r)
    finit    
    ; S = pi*r*r
    fld dword [esp+4] ; r
calc_obsah_kruhu:
    fmul st0,st0       ; r*r
    fldpi              ; pi  r*r
    fmulp st1,st0      ; pi*r*r
    ret

obsah_kruhu_d:
; __cdecl double obsah_kruhu_d(double r)
    finit    
    ; S = pi*r*r
    fld qword [esp+4] ; r
    jmp calc_obsah_kruhu    
        
CMAIN:
    push ebp
    mov ebp, esp

    finit
        
    push dword __float32__(10.0)
    call obsah_kruhu
    add esp,4
    call WriteDouble
    call WriteNewLine
    
    sub esp,4
;    mov eax,[deset]
;    mov [esp],eax
;    mov eax,[deset+4]
;    mov [esp+4],eax
;    push dword 0x4002a000
;    push dword 0x00000000
    push dword 10
    fild dword [esp]
    fstp qword [esp]
    call obsah_kruhu_d
    add esp,8
    call WriteDouble
    call WriteNewLine    
    
    push dword 33
    fild dword [esp]
    fstp qword [esp]   
    push fstr
    call printf
    add esp,12
    
    
; __cdecl double quad(double a, double b, double c, double x)
; quad(a,b,c,x) = ax^2 + bx + c    

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
    call quad_2 ; a = 10, b = -5, c = -1, x = 4
    add esp,32

    call WriteDouble
    call WriteNewLine
    

    mov esp,ebp
    pop ebp
    ret