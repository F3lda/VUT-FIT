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
;--- �kol 1 ---
;
; Naprogramujte funkci "task11", kter� zam�n� po�ad� slabik (byt�) X1, X2, X3, X4 v registru EAX takto:
;
; p�vodn� hodnota:  EAX = X4 X3 X2 X1
; v�sledn� hodnota: EAX = X3 X1 X2 X4 
;
; Funkci je hodnota p�ed�na v resgistru EAX a v�sledek vrac� tak� v registru EAX. Nejni��� v�znamov�
; slabika (byte) je X1, nejvy��� v�znamov� slabika (byte) je X4.
;
; Vstup:
;    - EAX = 32bitov� hodnota
;
; V�stup:
;    - EAX = v�sledek
;    - funkce nemus� zachovat obsah registr�
;
task11:
    ; zde zadejte k�d funkce task11
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
;--- �kol 2 ---
;
; Naprogramujte funkci "task12", kter� vypo��t� n�sleduj�c� v�raz SE ZNAM�NKEM:
;
; v = (a*(b + c) - d*e)/f ... celo��seln� pod�l
; z = (a*(b + c) - d*e)%f ... zbytek po d�len� f
;
; Vstupn� hodnoty jsou ulo�eny v pam�ti a jsou definov�ny takto:
;
;    [a] ...  8bitov� ��slo se znam�nkem
;    [b] ...  8bitov� ��slo se znam�nkem
;    [c] ...  8bitov� ��slo se znam�nkem
;    [d] ... 16bitov� ��slo se znam�nkem
;    [e] ... 16bitov� ��slo se znam�nkem
;    [f] ... 32bitov� ��slo se znam�nkem
;
; V�sledek ulo�te do pam�ti na adresu v a z takto:
;
;    [v] ... v�sledek - vypo��tan� hodnota v�razu (32bitov� ��slo se znam�nkem)
;    [z] ... zbytek po d�len� hodnotou f (32bitov� ��slo se znam�nkem)
;
; D�le�it�:
;  - funkce nesm� m�nit obsah registru ESI
;  - d�len� nulou neberte v �vahu
;
; Kontroln� v�po�et:
;    a = 5, b = 7, c = 2, d = 6, e = 10, f = 7
;    v = -2, z = -1
task12:
    ; zde zadejte k�d funkce task12
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
