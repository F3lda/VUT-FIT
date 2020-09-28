%include "rw32-2018.inc"

section .data

    str1 db "Ahoj Ahoj"
    str2 db "---------"
    str3 db "Aho      "
    
    arr1 dw 1,-5,-40,10
    arr2 dw 0,0,0,0

section .text

; void strncpy(const char* src, char* dest, int N) {
;   if (src != NULL && dest != NULL) {
;     for(;N>0;N--) dest[N - 1] = src[N - 1];
;   }
; }
;
; ESI = src
; EDI = dest
; ECX = N
;
strncpy:
    push esi
    push edi
    push ecx

    test esi,edi
    jz .exit
    
    cmp ecx,0
    jle .exit

    cld
    rep movsb ; do { *edi = *esi; esi++; edi++; ecx--; } while (ecx != 0)
    
.exit:
    pop ecx
    pop edi
    pop esi
    ret

; int strncmp(const char *s1, const char *s2, int N) {
;   if (s1 & s2 != NULL) {
;     for(;N>0;N--) if (s1[N - 1] != s2[N - 1]) return 0;
;     //for(int i=0; i<N; i++) if (s1[i] != s2[i]) return 0;
;     return 1;
;   }
;   return 0;
; }
strncmp:
    push esi
    push edi
    push ecx
    push edx

    xor eax,eax
    test esi,esi
    jz .exit
    test edi,edi
    jz .exit

    xor ebx,ebx
.while:        
    cmp ebx,ecx
    jge .same
    mov dl,[esi + ebx]
    cmp dl,[edi + ebx]
    jnz .exit
    inc ebx
    jmp .while
    
;    cld
;    repz cmpsb 
    ; do {
    ;   ZF = (*edi == *esi); esi++; edi++; ecx--; 
    ; } while (ecx != 0 && ZF == 1)
;    jnz .exit
.same:
    inc eax
    
.exit:        
    pop edx
    pop ecx
    pop edi
    pop esi
    ret

; int find_char(char a, const char* s, int N) {
;   if (s != NULL) {
;     for(int i=0;i<N;i++) if (s[i] == a) return 1;
;   }
;   return 0;
; }    
;
; EDI = s
; AL  = a
; ECX = N
;
; EAX = 0 <=> a se nevyskytuje v s
; EAX = 1 <=> a se vyskytuje v s
;
; SCASB:
;   ZF = (AL == *EDI)
;   EDI++
find_char:
    push edi
    push ecx

    and eax,0x000000FF
    test edi,edi
    jz .exit
    cmp ecx,0
    jle .exit

    cld
    repnz scasb
    setz al ; AL = ZF
    
;    jz .found
;    mov eax,0
;    jmp .exit
;.found:
;    mov eax,1
    
            
.exit:        
    pop ecx
    pop edi
    ret
            
; void copy_abs(const short *arr1, short *arr2, int N) {
;   if (arr1 != NULL && arr2 != NULL)
;     for(int i=0;i<N;i++) arr2[i] = abs(arr1[i]);
; }

; LODSW ... AX = *ESI; ESI += 2
; STOSW ... *EDI = AX; EDI += 2

; ESI = arr1
; EDI = arr2
; ECX = N

copy_abs:
;    pushad
    push ecx
    push esi
    push edi

    cmp ecx,0
    jle .exit
    test esi,esi
    je .exit
    test edi,edi
    je .exit
    
.for:
;mov ax,[esi]
;add esi,2
    lodsw ; AX = *ESI; ESI += 2
    cmp ax,0
    jge .skip
    neg ax
.skip:
    stosw ; *edi = AX; edi += 2
    loop .for
    
.exit:            
    pop edi
    pop esi
    pop ecx
;    popad
    ret
            
CMAIN:
    push ebp
    mov ebp, esp
    
; MOVS, CMPS, SCAS, LODS, STOS
    mov esi,arr1
    mov edi,arr2
    mov ecx,4
    call copy_abs

    pop ebp
    ret