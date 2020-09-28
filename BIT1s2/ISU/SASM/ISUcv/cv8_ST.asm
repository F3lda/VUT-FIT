%include "rw32-2018.inc"

section .data

    str1 db "10A?20",0

section .text

; pascal int isdigit(char c) { return c >= '0' && c <= '9'; }

; c ... ebp+8
; c ... esp+4

; ESP+4 -> 'a' ~ c
; ESP+0 -> EIP

; PUSH EBP
; MOV EBP,ESP
; ...
; POP EBP

; ESP+8 -> 'a' ~ c   <- EBP+8
; ESP+4 -> EIP       <- EBP+4
; ESP+0 -> stare EBP <- EBP+0

; EBP+8 -> c

isdigit:
%define c ebp+8
    push ebp    ; enter 0,0
    mov ebp,esp
;    mov al,[esp+8]
;    mov al,[ebp+8]
    mov al,[c]
; { return c >= '0' && c <= '9'; }
    cmp al,'0'
    jb .false
    cmp al,'9'
    ja .false
    mov eax,1
    jmp .exit
.false:
    xor eax,eax
.exit:            
    pop ebp
    ret 4

; pascal int isdigit_base(char c, int base) { 
;   if (base >= 1 && base < 35) {
;       if (c >= '0' && c <= '9') 
;         c -= '0';
;       else if (c >= 'A' && c <= 'Z') 
;         c -= 'A' + 10;    
;       else if (c >= 'a' && c <= 'z') 
;         c -= 'a' + 10;    
;       else return 0;
;     return c < base && c >= 0;
;   }
;   return 0;
; }
isdigit_base:
%define c    ebp+12
%define base ebp+8
    push ebp    ; enter 0,0
    mov ebp,esp
    cmp [base],dword 1
    jle .false
    cmp [base],dword 35
    jge .false
    
    mov al, byte[c]
    cmp al,'0'
    jb .false
    cmp al,'9'
    ja .if1
    sub al,'0'
    jmp .test
.if1:    
    cmp al,'A'
    jb .false
    cmp al,'Z'
    ja .if2
    sub al,'A'
    add al,10
    jmp .test
.if2:
    cmp al,'a'
    jb .false
    cmp al,'z'
    ja .false
    sub al,'a'
    add al,10
    jmp .test
.test:    
    cmp al,[base]
    jae .false
.true:    
    mov eax,1
    jmp .exit    
.false:
    xor eax,eax
.exit:            
    pop ebp
    ret 8

; pascal int get_digit_value(char c, int base) { 
;   if (base >= 1 && base < 35) {
;       if (c >= '0' && c <= '9') 
;         c -= '0';
;       else if (c >= 'A' && c <= 'Z') 
;         c -= 'A' + 10;    
;       else if (c >= 'a' && c <= 'z') 
;         c -= 'a' + 10;    
;       else return -1;
;     if (c < base && c >= 0) return (int)c;
;   }
;   return -1;
; }
get_digit_value:
%define c    ebp+12
%define base ebp+8
    push ebp    ; enter 0,0
    mov ebp,esp
    xor eax,eax
    cmp [base],dword 1
    jle .false
    cmp [base],dword 35
    jge .false
    
    mov al, byte[c]
    cmp al,'0'
    jb .false
    cmp al,'9'
    ja .if1
    sub al,'0'
    jmp .test
.if1:    
    cmp al,'A'
    jb .false
    cmp al,'Z'
    ja .if2
    sub al,'A'
    add al,10
    jmp .test
.if2:
    cmp al,'a'
    jb .false
    cmp al,'z'
    ja .false
    sub al,'a'
    add al,10
    jmp .test
.test:    
    cmp al,[base]
    jae .false
.true:
    jmp .exit 
.false:
    mov eax,-1
.exit:            
    pop ebp
    ret 8

; cdecl int atoi(const char *pStr, int base) {
;   if (base <= 1 && base >= 35) return 0;
;   if (pStr == NULL) return 0;
;   int EAX = 0;
;   int ECX = 0;
;   while(pStr[ECX] != 0) {
;     EAX = EAX*base;
;     int EDX = get_digit_value(pStr[ECX], base);
;     if (EDX > -1) EAX += EDX;
;     ECX++;
;   }
;   return EAX;
; }

; pStr .... EBP+8
; base .... EBP+12
atoi:
%define pStr EBP+8
%define base EBP+12
    enter 0,0

;   int EAX = 0;
    xor eax,eax        
;   if (base <= 1 && base >= 35) return 0; 
    mov edi,[base]   
    cmp edi,1
    jle .exit 
    cmp edi,35
    jge .exit 
;   if (pStr == NULL) return 0;
    mov esi,[pStr]
    test esi,esi
    jz .exit
;   int ECX = 0;
    xor ecx,ecx    
;   while(pStr[ECX] != 0) {    
.while:
    mov bl,[esi + ecx]
    test bl,bl
    jz .endwhile
;     EAX = EAX*base;
    imul edi  ; EDX:EAX = EAX*[base]
;     int EDX = get_digit_value(pStr[ECX], base);
    push eax
    push ebx
    push edi
    call get_digit_value
    mov edx,eax
    pop eax
;     if (EDX > -1) EAX += EDX;    
    cmp edx,-1
    jle .skip
    add eax,edx
.skip:
;     ECX++;
    inc ecx
    jmp .while
.endwhile:    
.exit:
    leave
    ret


; cdecl int strtol(const char *pStr, char **ptrEnd, int base)
; *ptrEnd = 

CMAIN:
    push ebp
    mov ebp, esp
    
    push dword 16
    push str1
    call atoi
    add esp,8
        
    pop ebp
    ret