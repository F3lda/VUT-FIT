%include "rw32-2018.inc"

section .data
    a db 5
    b db 7
    c db 2
    d dw 6
    e dw 10
    f dd 7
    v dd 0
    z dd 0

section .text
CMAIN:
    push ebp
    mov ebp,esp
    
        mov eax,0x44332211
    call task11
    
    call task12
    
    pop ebp
    ret
    
;
;--- Úkol 1 ---
;
; Naprogramujte funkci "task11", který zamìní poøadí slabik (bytù) X1, X2, X3, X4 v registru EAX takto:
;
; pùvodní hodnota:  EAX = X4 X3 X2 X1
; výsledná hodnota: EAX = X3 X1 X2 X4 
;
; Funkci je hodnota pøedána v resgistru EAX a výsledek vrací také v registru EAX. Nejnižší významová
; slabika (byte) je X1, nejvyšší významová slabika (byte) je X4.
;
; Vstup:
;    - EAX = 32bitová hodnota
;
; Výstup:
;    - EAX = výsledek
;    - funkce nemusí zachovat obsah registrù
;
task11:
    ; zde zadejte kód funkce task11
    mov ebx, eax
    mov ecx, eax
    shr ecx, 16
    
    mov al, cl ;X3
    sal eax, 8
    
    mov al, bl ;X1
    sal eax, 8
    
    mov al, bh ;X2
    sal eax, 8
    
    mov al, ch ;X4
    ret

;
;--- Úkol 2 ---
;
; Naprogramujte funkci "task12", který vypoèítá následující výraz SE ZNAMÉNKEM:
;
; v = (a*(b + c) - d*e)/f ... celoèíselný podíl
; z = (a*(b + c) - d*e)%f ... zbytek po dìlení f
;
; Vstupní hodnoty jsou uloženy v pamìti a jsou definovány takto:
;
;    [a] ...  8bitové èíslo se znaménkem
;    [b] ...  8bitové èíslo se znaménkem
;    [c] ...  8bitové èíslo se znaménkem
;    [d] ... 16bitové èíslo se znaménkem
;    [e] ... 16bitové èíslo se znaménkem
;    [f] ... 32bitové èíslo se znaménkem
;
; Výsledek uložte do pamìti na adresu v a z takto:
;
;    [v] ... výsledek - vypoèítaná hodnota výrazu (32bitové èíslo se znaménkem)
;    [z] ... zbytek po dìlení hodnotou f (32bitové èíslo se znaménkem)
;
; Dùležité:
;  - funkce nesmí mìnit obsah registru ESI
;  - dìlení nulou neberte v úvahu
;
; Kontrolní výpoèet:
;    a = 5, b = 7, c = 2, d = 6, e = 10, f = 7
;    v = -2, z = -1
task12:
    ; zde zadejte kód funkce task12
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    
    mov al, [b]
    add al, [c]
    ;al = b+c
    
    imul byte[a]
    ;ax = c*(b+c)
    cwde
    ;eax = ax
    mov ebx, eax
    ;ebx = eax
    ;;;;;;;;;
    
    mov ax, [d]
    imul word[e]
    ;dx:ax = d*e
    
    mov cx, dx
    ;cx = dx;
    sal ecx, 16
    mov cx, ax
    ;ecx = dx:ax
    ;ecx = d*e
    ;;;;;;;;;
    
    sub ebx, ecx
    ;ebx = ebx-ecx
    ;;;;;;;;;
    
    mov eax, ebx
    ;eax = ebx    
    cdq
    ;edx:eax = eax
    idiv dword[f]
    ;eax = edx:eax/f
    ;edx = edx:eax/f (remainder)
    
    mov [v], eax
    mov [z], edx
    
    

    ;call WriteInt32
    ;call WriteNewLine        
    ;mov eax, [z]
    ;call WriteInt32
    ;call WriteNewLine

    ret
