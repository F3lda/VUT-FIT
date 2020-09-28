;%include "rw32-2019.inc"
%include "rw32-2020.inc"
; long strtol( const char *str, char **str_end, int base );
CEXTERN strtol

; int printf( const char *format, ... );
; CEXTERN printf

section .data

    strKBInput db " 15str56A  10 sFF  g",0
;    strKBInput db " 23hhhhB1a ",0    
;    strKBInput db " B1 1",0
;    strKBInput db "B1",0    
;    strKBInput db "1",0
;    strKBInput db 0    
    strFormat  db "0x%08X",EOL,0

section .text

; char *strStart = strKBInput
; char *strPos = NULL
; do {
;   long EAX = strtol(strStart, &strPos, 16)
;   if (EAX != 0) printf("0x%08X", EAX)
;   strStart = strPos + 1;
; } while(*(strStart-1) != 0)

CMAIN:
    push ebp
    mov ebp, esp
    sub esp,4

%define _local_strPos   ebp-4    

; char *strStart = strKBInput
    mov esi,strKBInput ; strStart ~ ESI
; char *strPos = NULL    
    mov dword [_local_strPos],0

.while:   
;   long EAX = strtol(strStart, &strEnd, 16)
    push dword 16
    lea edi,[_local_strPos]
    push edi
    push esi
    call strtol
    add esp,12
    
;   if (EAX != 0)
    test eax,eax
    jz .skip_printf

;       printf("0x%08X", EAX)
    push eax
    push strFormat
    call printf
    add esp,8
    
.skip_printf:
;   strStart = strEnd + 1;
    mov esi,[_local_strPos]
    inc esi    
; } while(*(strStart-1) != 0)
    cmp byte [esi - 1],0
    jnz .while
    
.exit:
    xor eax,eax
    mov esp,ebp
    pop ebp
    ret