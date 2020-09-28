%include "rw32-2018.inc"

CEXTERN  malloc

section .data
    task21A dw 192,-32,-48,96,-80,0,-960,-944
    task21B dw -5231,-2606,19068,27330,2702,-29579,9661,-3339
    task22A dd 6144,2560,2304,4608,1792,3072,-12288,-12032
    task22B dd 109,2,4,54,1
    task22Bcount dd 5
    task23A db 96,40,36,72,28,48,-192,-188

section .text
CMAIN:
    push ebp
    mov ebp,esp
    
    mov eax,task21A
    mov ebx,task21B
    mov ecx,8
    call task21

    
    
    ; zde bude volani funkce: task22(task22A,8)
    ; musite ulozit parametry ve spravnem poradi
    ; a uklidit zasobnik, pokud je to treba
    mov ebx, task22B
    mov eax, [task22Bcount]
    push eax
    push ebx
    call task22
    ;;call WriteInt32
    
    
    
    mov ecx,9
    call task23
    
    ;;mov ebx, eax
    ;;mov eax, ecx
    ;;call WriteInt32
    ;;call WriteNewLine
    ;;call WriteNewLine
    ;;xor edx, edx; edx = 0
    ;;loop4:;LOOP START
    ;;    mov eax, [ebx+edx]
    ;;    call WriteInt8
    ;;    call WriteNewLine
    ;;inc edx;LOOP ENCREASE
    ;;cmp edx,ECX;LOOP COMPARE
    ;;jl loop4;LOOP END

    


    pop ebp
    ret    
;           
;--- Ukol 1 --- 
;
; Naprogramujte funkci, ktera zkopiruje pole 16bitovych hodnot z pole, na ktere
; ukazuje registr EAX do pole, na ktery ukazuje registr EBX. Pocet prvku obou poli je v registru ECX.
;
; Vstup:
;   EAX = ukazatel na pole 16bitovych hodnot, ktere chci zkopirovat (32bitova hodnota, vzdy ruzna od 0)
;   EBX = ukazatel na misto, kam chci pole EAX zkopirovat (32bitova hodnota, vzdy ruzna od 0)
;   ECX = pocet prvku poli (32bitova hodnota bez znamenka, vzdy vetsi nez 0)
;
; Vystup:
;   Kopie pole z adresy EAX na adresu EBX.
;
; Dulezite:
;   Funkce NEmusi zachovat obsah registru.
;
; Hodnoceni: az 2 b.
;
task21:
    ; zde zadejte kod funkce task21
    mov ESI, EAX
    
    xor edx, edx; edx = 0
    loop1:;LOOP START
        mov eax, [ESI+(edx*2)]
        mov [EBX+(edx*2)],eax
        ;;cwde
        ;;call WriteInt32
        ;;call WriteNewLine
    inc edx;LOOP ENCREASE
    cmp edx,ECX;LOOP COMPARE
    jb loop1;LOOP END

    ret
;
;--- Ukol 2 ---
;
; Naprogramujte funkci, ktera v poli 32bitovych cisel bez znamenka pA nalezne maximum a vrati jeho hodnotu
; v registru EAX. Delka pole je dana parametrem N.
;
; Funkci jsou predavany parametry na zasobniku v poradi od prvniho k poslednimu (zleva doprava), parametry ze 
; zasobniku uklizi volany a vysledek funkce se vraci v registru EAX (konvence jazyka Pascal).
;
; unsigned int task22(const unsigned int *pA, int N)
;
; Vstup:
;   pA: ukazatel na zacatek pole 32bitovych prvku bez znamenka
;    N: pocet prvku pole pA (32bitove cislo se znamenkem)
;
; Vystup:
;   EAX = 0, kdyz je pA neplatny ukazatel (tj. pA == 0) nebo N <= 0
;   EAX = hodnota 32bitoveho maxima bez znamenka.
;
; Dulezite:
;   Funkce musi zachovat obsah vsech registru, krome registru EAX a priznakoveho registru.
;
; Hodnoceni: az 4 b.
;
task22:
    push ebp     
    mov ebp,esp
    ; STACK FRAME
    push ebx
    push ecx
    push edx
    ;;;;;;;;
    
    ; count check
    cmp dword[ebp+12], 0
    jle error
    mov ebx, [ebp+12]
    
    ; pointer check
    cmp dword[ebp+8], 0
    je error
    mov ecx, [ebp+8]
    
    ; EBX = array size
    ; ECX = array pointer
    xor edx, edx; edx = 0
    inc edx; start from 1
    mov eax, [ECX]; set first value
    cmp edx, EBX; if array has just 1 item
    je end; finish
    ; else continue
    loop2:;LOOP START
        cmp [ECX+(edx*4)], eax
        jbe continue; is bigger
        mov eax, [ECX+(edx*4)]
        ;;call WriteInt32
        ;;call WriteNewLine
        ; else
        continue:
    inc edx;LOOP ENCREASE
    cmp edx,EBX;LOOP COMPARE
    jl loop2;LOOP END
    
    
    ;;;;;;;;
    jmp end
    error:
        mov eax, 0
end:
    ;;;;;;;;
    pop edx
    pop ecx
    pop ebx
    ; STACK FRAME
    mov esp, ebp
    pop ebp
    ; úklid parametrù
    ret 8;(2*4)
;
;--- Ukol 3 ---
;
; Naprogramujte funkci 'unsigned char* task23(int N)', ktera vypocita Fibonacciho
; posloupnost N 8bitovych cisel bez znamenka. Funkce ma jako vstupni parametr
; pocet prvku posloupnosti v ECX (32bitova hodnota se znamenkem) a vraci v EAX
; ukazatel na pole, ktere alokujete funkci 'malloc', a do ktereho ulozite
; jednotlive prvky posloupnosti.
;
; Fibonacciho posloupnost je definovana takto:
;
;   F(0) = 0, F(1) = 1, F(n) = F(n-1) + F(n-2)
;
; Funkci 'malloc' importujte makrem CEXTERN:
;
;   CEXTERN  malloc
;
; Funkce 'malloc' je definovana takto: 
;
;   void* malloc(size_t N)
;     * N je pocet bytu, ktere maji byt alokovany (32bitove cislo bez znamenka)
;     * funkce vraci v EAX ukazatel (32bitove cislo bez znamenka)
;     * funkce muze zmenit obsah registru ECX a EDX
;
; Vstup:
;   * ECX = pocet prvku pole (32bitove cislo se znamenkem)
;
; Vystup:
;   * EAX = 0, pokud bude N <= 0 (tj. nic nealokujete a vratite hodnotu EAX = NULL = 0),
;   * jinak v EAX vratite ukazatel na pole 8bitovych prvku bez znamenka reprezentujicich Fibonacciho posloupnost
;
; Dulezite:
;   * Funkce musi zachovat obsah vsech registru, krome registru EAX a priznakoveho registru.
;   * Funkce 'malloc' muze zmenit obsah registru ECX a EDX => pokud je pouzivate, schovejte si je.
;
; Hodnoceni: az 6 b.
;
task23:
    push ebp     
    mov ebp,esp
    ; STACK FRAME
    push ebx
    push ecx
    push edx
    push esi
    push edi
    
    ;;;;;;;;
    
    cmp ECX, 0
    jle error2
    
    push ECX
    call malloc
    cmp EAX, 0
    je error2
    
    mov ESI, EAX
    pop ECX
    ; ESI = array pointer
    ; ECX = count
    
    xor edi, edi; edi = 0
    
    mov [ESI+edi], byte 0; first value
    inc edi
    cmp ECX, 1; if array has just 1 item
    je end2; finish
    ; else continue
    
    mov [ESI+edi], byte 1; first value
    inc edi
    cmp ECX, 2; if array has just 2 items
    je end2; finish
    ; else continue
    
    loop3:;LOOP START
        mov al, [ESI+edi-1]
        mov [ESI+edi], al
        mov al, [ESI+edi-2]
        add [ESI+edi], al
    inc edi;LOOP ENCREASE
    cmp edi,ECX;LOOP COMPARE
    jl loop3;LOOP END
    
    
    ;;;;;;;;
    jmp end2
    error2:
        mov eax, 0
end2:
    mov eax, ESI
    ;;;;;;;;
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ; STACK FRAME
    mov esp, ebp
    pop ebp
    ret 
