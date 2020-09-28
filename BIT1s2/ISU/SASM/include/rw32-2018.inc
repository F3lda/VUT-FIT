; Verze: 2018.02.09
; Autor: Filip Orsag (orsag@fit.vutbr.cz)
;
; Podpurna knihovna funkci "rw32-20XX.inc" pro predmet ISU.
; Pro svou cinnost vyuziva standardni knihovny jazyka C.
; Lze ji pouzit v OS Windows nebo Linux.
;
; Knihovnu vkladejte na zacatek vaseho programu: 
;
;	%include "rw32-20XX.inc"
;
; Preklad Windows:
;	nasm -f win32 -g -o helloworld.obj helloworld.asm
;	golink helloworld.obj /fo helloworld.exe /console /debug coff /entry:_main msvcrt.dll kernel32.dll
;
; Preklad Linux:
;	nasm -f elf32 -g -o helloworld.o helloworld.asm
;	gcc -m32 -o helloworld helloworld.o
;
;
; Dostupne funkce jsou nasledujici:
;
; Cteni z klavesnice:
;	ReadChar
;		Vstup:	nic.
;		Vystup:	AL = hodnota znaku.
;		Chovani: ceka na zadani znaku a na stisk klavesy ENTER (lze zadat i vice znaku, ale ty budou ignorovany, platny je pouze prvni znak).

;	ReadString
;		Vstup:	EBX = maximalni pocet znaku, ktere mohou byt nacteny.
;				EDI = adresa, kam se retezec nacte (musi byt rezervovano alespon EBX+1 bytu).
;		Vystup:	EAX = pocet prectenych znaku
;				retezec znaku ulozeny na adrese EDI.
;		Chovani: nacita znaky z klavesnice, dokud neni stisknuta klavesa ENTER. Na adresu v EDI ulozi maximalne EBX znaku
;				 nebo mene (vsechny dalsi znaky budou ignorovany), skutecny pocet znaku vraci v registru EAX.

;	ReadInt8
;		Vstup:	nic.
;		Vystup:	AL = cislo z intervalu <-128, 127>.
;		Chovani: cte cislo z klvesnice, umozni zadat jakykoliv znak, ale pouze znak '-','+' a cislice budou akceptovany,
;				 pokud neni zadana zadny znak a je stisknut ENTER, pak je vracena hodnota 0, pri prekroceni kterekoliv povolene
;				 meze je vraceno bud maximum nebo minimum daneho rozsahu.

;	ReadInt16
;		Vstup:	nic.
;		Vystup:	AX = cislo z intervalu <-32768, 32767>.
;		Chovani: viz ReadInt8.

;	ReadInt32
;		Vstup:	nic.
;		Vystup:	EAX = cislo z intervalu <-2^31, 2^31 - 1>.
;		Chovani: viz ReadInt8.

;	ReadUInt8
;		Vstup:	nic.
;		Vystup:	AL = cislo z intervalu <0, 255>.
;		Chovani: viz ReadInt8 s tim rozdilem, ze pri zadani znaku '-' bude cislo v doplnkovem kodu, ale interpretovano jako cislo bez znamenka.

;	ReadUInt16
;		Vstup:	nic.
;		Vystup:	AX = cislo z intervalu <0, 65535>.
;		Chovani: viz ReadUInt8.

;	ReadUInt32
;		Vstup:	nic.
;		Vystup:	EAX = cislo z intervalu <0, 2^32 - 1>.
;		Chovani: viz ReadUInt8.

;	ReadFloat
;		Vstup:	nic.
;		Vystup:	EAX = 32bitove realne cislo v plovouci radove carce (v jazyce C typ float).
;		Chovani: viz ReadInt8, navic je dovoleno zadat znak '.' (desetinna carka) a exponent (e+N nebo e-N).

;	ReadDouble
;		Vstup:	nic.
;		Vystup:	ST0 = 64bitove realne cislo v plovouci radove carce (v jazyce C typ double).
;		Chovani: viz ReadFloat.

; Vypis na obrazovku:
;	WriteChar
;		Vstup:	znak v AL.
;		Vystup:	vypise znak na obrazovku.

;	WriteNewLine
;		Vstup:	nic.
;		Vystup:	vypise konec radku ("\n").

;	WriteString
;		Vstup:	ESI = ukazatel na retezec zakonceny hodnotou 0, ktery bude vypsan.
;		Vystup:	vypise retezec na obrazovku.

;	WriteBin8
;		Vstup:	AL = cislo.
;		Vystup:	vypise cislo z AL na obrazovku v binarni podobe.

;	WriteBin16
;		Vstup:	AX = cislo
;		Vystup:	vypise cislo z AX na obrazovku v binarni podobe.

;	WriteBin32
;		Vstup:	EAX = cislo.
;		Vystup:	vypise cislo z EAX na obrazovku v binarni podobe.

;	WriteHex8
;		Vstup:	AL = cislo.
;		Vystup:	vypise cislo z AL na obrazovku v hexadecimalni podobe "0xXX".

;	WriteHex16
;		Vstup:	AX = cislo
;		Vystup:	vypise cislo z AX na obrazovku v hexadecimalni podobe "0xXXXX".

;	WriteHex32
;		Vstup:	EAX = cislo.
;		Vystup:	vypise cislo z EAX na obrazovku v hexadecimalni podobe "0xXXXXXXXX".

;	WriteInt8
;		Vstup:	AL = cislo.
;		Vystup:	vypise cislo z AL na obrazovku v dekadicke forme se znamenkem.

;	WriteInt16
;		Vstup:	AX = cislo.
;		Vystup:	vypise cislo z AX na obrazovku v dekadicke forme se znamenkem.

;	WriteInt32
;		Vstup:	EAX = cislo.
;		Vystup:	vypise cislo z EAX na obrazovku v dekadicke forme se znamenkem.

;	WriteUInt8
;		Vstup:	AL = cislo.
;		Vystup:	vypise cislo z AL na obrazovku v dekadicke forme bez znamenka.

;	WriteUInt16
;		Vstup:	AX = cislo.
;		Vystup:	vypise cislo z AX na obrazovku v dekadicke forme bez znamenka.

;	WriteUInt32
;		Vstup:	EAX = cislo.
;		Vystup:	vypise cislo z EAX na obrazovku v dekadicke forme bez znamenka.

;	WriteFlags
;		Vstup:	nic (aktualni stav priznakoveho registru).
;		Vystup:	vypise obsah registru priznaku EFLAGS na obrazovku.

;	WriteFloat
;		Vstup:	EAX = cislo (realne cislo s jednoduchou presnosti - float).
;		Vystup:	vypise cislo z EAX na obrazovku ve forme realneho cisla.

;	WriteDouble
;		Vstup:	ST0 = cislo (realne cislo v koprocesoru, vypisuje se cislo s dvojitou presnosti - double).
;		Vystup:	vypise cislo z ST0 na obrazovku ve forme realneho cisla.
;

;	WriteAllGPRegisters32
; 		Vstup: nic (aktualni stav jednotlivych registru)
;		Vystup: vypise obsah registru EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP
;

;	WriteTestGPRegisters32
; 		Vstup: nic (aktualni stav jednotlivych registru)
;		Vystup: vypise obsah registru EBX, ECX, EDX, ESI, EDI, EBP
;

;	WriteArrayInt8
; 		Vstup: ESI = ukazatel na pole 8bitovych cisel, 
;			   ECX = pocet prvku pole
;		Vystup: vypise pole 8bitovych cisel se znamenkem
;

;	WriteArrayInt16
; 		Vstup: ESI = ukazatel na pole 16bitovych cisel,
;			   ECX = pocet prvku pole
;		Vystup: vypise pole 16bitovych cisel se znamenkem
;

;	WriteArrayInt32
; 		Vstup: ESI = ukazatel na pole 32bitovych cisel,
;			   ECX = pocet prvku pole
;		Vystup: vypise pole 32bitovych cisel se znamenkem
;

bits 32

; Funkce standardni knihovny jazyka C (C Run-Time libraray)

%ifidn __OUTPUT_FORMAT__, win32
	%ifndef SASM
		; Nektere dalsi funkce Win32 API
			extern ExitProcess

		section .text

			global start
		start:
			call _main
			push dword 0
			call ExitProcess
	%endif	

	%macro CEXTERN 1.nolist
		%define %1 _%1
		extern _%1
	%endmacro

	; nase vstupni funkce se bude jmenovat _main
	%define CMAIN _main

%elifidn __OUTPUT_FORMAT__, elf32

	%macro CEXTERN 1.nolist
		extern %1
	%endmacro

	; nase vstupni funkce se bude jmenovat main
	%define CMAIN main

%endif

global CMAIN

CEXTERN printf
CEXTERN scanf
CEXTERN sscanf
CEXTERN gets
CEXTERN fgets
CEXTERN getchar
CEXTERN strlen
CEXTERN strtoul
CEXTERN strtol
CEXTERN strtod
CEXTERN fflush

%macro STRING 2+.nolist
	%1: DB %2
	endof.%1: DB 0
	lenof.%1 EQU endof.%1 - %1
	sizeof.%1 EQU endof.%1 - %1
%endmacro

%macro INVOKE 1.nolist
	call %1
%endmacro

%macro PRINTF 0-*.nolist
	%assign incStack 0
	%if %0 > 0
		%rep %0
			%rotate -1
			%ifstr %1
				section .data
					%%str: db %1, 0
				section .text
					push %%str
			%else
				push dword %1
			%endif
			%assign incStack incStack+4
		%endrep
		%rotate -1
	%endif
	call printf
	%if incStack > 0
		add esp,incStack
	%endif
	push 0
	call fflush
	add esp,4
%endmacro

%imacro MOVSTR 2+
	section .data
		%%str: db %2, 0
	section .text			
		mov %1,%%str
%endmacro

%define KB_ENTER 0x0D
%define KB_NEWLINE 0x0A
%define KB_BACKSPACE 0x08
%define EOL KB_NEWLINE

; Konstanty

%define EOF -1
%define MAX_UBYTE 0xFF
%define MAX_UWORD 0xFFFF
%define MAX_UDWORD 0xFFFFFFFF
%define MAX_SBYTE 0x0000007F
%define MAX_SWORD 0x00007FFF
%define MAX_SDWORD 0x7FFFFFFF
%define MIN_SBYTE 0xFFFFFF80
%define MIN_SWORD 0xFFFF8000
%define MIN_SDWORD 0x80000000
%define MAX_STRING_LENGTH 1024

section .data align=4

	STRING msg_EOL, `\n`
	STRING msg_EnterChar, 'Zadejte znak: '
	STRING msg_EnterUInt, 'Zadejte cele cislo bez znamenka z intervalu <%u,%u>: '
	STRING msg_EnterSInt, 'Zadejte cele cislo se znamenkem z intervalu <%d,%d>: '
	STRING msg_EnterFloat, 'Zadejte realne cislo (napr. 1.5): '
	STRING msg_EnterText, 'Zadejte text (akceptovano bude maximalne %d znaku): '
	STRING msg_NumberTooBig, `Cislo je prilis velke, vracim maximalni moznou hodnotu.\n`
	STRING msg_NumberTooLow, `Cislo je prilis male, vracim minimalni moznou hodnotu.\n`
	STRING msg_WrongNumber, 'Zadana hodnota neni rozpoznatelne cislo. Zadejte nove CISLO: '

	msg_Flags			db 'Flags: xxxx|OF|DF|IF|TF|SF|ZF|xx|AF|xx|PF|xx|CF|',EOL,'       xxxx| '
	msg_FlagValues		db              'x| x| x| x| x| x| x| x| x| x| x| x|',EOL
		endof_msg_Flags db 0
		lenof_msg_Flags EQU endof_msg_Flags - msg_Flags
	number_in			dq 0.0
	ioBuffer			times MAX_STRING_LENGTH db '!'
						db 0

section .text

;----------------
; Funkce ReadChar
;
ReadChar:
	enter 0,0
	push esi

	mov esi,msg_EnterChar
	call WriteString

	INVOKE getchar

.return:
	pop esi
	leave
	ret

;----------------
; Funkce ReadString
;
ReadString:
	enter 0,0
	pushfd
	push esi
	push edi
	push ebx
	push ecx
	push edx

	PRINTF msg_EnterText, ebx

	mov [edi], byte 0
	cmp ebx,0
	je .return

	push ioBuffer
	INVOKE gets
	add esp,4

;	push stdin
;	push ebx
;	push ioBuffer
;	INVOKE fgets
;	add esp,12

	mov [ioBuffer + ebx], byte 0
	push ioBuffer
	INVOKE strlen
	add esp,4
	mov ecx,eax
	inc ecx

	mov esi,ioBuffer
	rep movsb

.return:
	pop edx
	pop ecx
	pop ebx
	pop edi
	pop esi
	popfd
	leave
	ret

;----------------
; Funkce ReadNumber
; Cte cislo ze standardniho vstupu dle formatovaciho retezce odpovidajiciho formatu scanf.
; Ukazatel na formatovaci retezec je ulozen v registru EBX.
;
ReadNumber:
	enter 0,0
	push ebx
	push ecx
	push edx

.again:
	mov [number_in], dword 0
	mov [number_in + 4], dword 0
	push ioBuffer
	INVOKE gets
	add esp,4
;	push dword [stdin]
;	push MAX_STRING_LENGTH
;	push ioBuffer
;	INVOKE fgets
;	add esp,12

	push number_in
	push ebx
	push ioBuffer
	INVOKE sscanf
	add esp,12
	cmp eax,0
	jne .ok

	PRINTF "%s", msg_WrongNumber

	jmp .again

.ok:
	mov eax,[number_in]

	pop edx
	pop ecx
	pop ebx
	leave
	ret

;----------------
; Funkce ReadIntX (8,16,32) {ReadInt8,ReadInt16,ReadInt32}
; Precte 8-, 16-, 32-bitovou celociselnou hodnotu se znamenkem z klavesnice
; Chovani funkci je dano chovanim funkce SSCANF. Veskere namitky se netykaji autora.
;
ReadInt8:
	enter 0,0
	push esi
	push edi
	push ebx
	mov ebx,MAX_SBYTE
	mov edi,MIN_SBYTE
	jmp ReadInt
	
ReadInt16:
	enter 0,0
	push esi
	push edi
	push ebx
	mov ebx,MAX_SWORD
	mov edi,MIN_SWORD
	jmp ReadInt

ReadInt32:
	enter 0,0
	push esi
	push edi
	push ebx
	mov ebx,MAX_SDWORD
	mov edi,MIN_SDWORD

ReadInt:
	push ecx
	push edx

	PRINTF msg_EnterSInt, edi, ebx

	pop edx
	pop ecx

	push ebx
	MOVSTR ebx,"%d"
	call ReadNumber
	pop ebx

	cmp eax,ebx
	jng .test_min

	mov esi,msg_NumberTooBig
	call WriteString
	mov eax,ebx
	jmp .return

.test_min:
	cmp eax,edi
	jnl .return

	mov esi,msg_NumberTooLow
	call WriteString
	mov eax,edi

.return:
	pop ebx
	pop edi
	pop esi
	leave
	ret

;----------------
; Funkce ReadUIntX (8,16,32) {ReadUInt8,ReadUInt16,ReadUInt32}
; Precte 8-, 16-, 32-bitovou celociselnou hodnotu bez znamenka z klavesnice
; Chovani funkci je dano chovanim funkce SSCANF. Veskere namitky se netykaji autora.
;
ReadUInt8:
	enter 0,0
	push esi
	push edi
	push ebx
	mov ebx,MAX_UBYTE
	mov edi,0
	jmp ReadUInt

ReadUInt16:
	enter 0,0
	push esi
	push edi
	push ebx
	mov ebx,MAX_UWORD
	mov edi,0
	jmp ReadUInt

ReadUInt32:
	enter 0,0
	push esi
	push edi
	push ebx
	mov ebx,MAX_UDWORD
	mov edi,0

ReadUInt:
	push ecx
	push edx

	PRINTF msg_EnterUInt, edi, ebx

	pop edx
	pop ecx

	push ebx
	MOVSTR ebx,"%u"
	call ReadNumber
	pop ebx

	cmp eax,ebx
	jna .return

	mov esi,msg_NumberTooBig
	call WriteString
	mov eax,ebx

.return:
	pop ebx
	pop edi
	pop esi
	leave
	ret

;----------------
; Funkce ReadFloat
; Precte 32bitove realne cislo z klavesnice
; Chovani funkci je dano chovanim funkce SSCANF. Veskere namitky se netykaji autora.
;
ReadFloat:
	enter 0,0
	pushfd
	push esi
	push ebx

	mov esi,msg_EnterFloat
	call WriteString
	MOVSTR ebx,"%lf"
	call ReadNumber
	fld qword [number_in]
	sub esp,4
	fstp dword [esp]
	pop eax

.return:

	pop ebx
	pop esi
	popfd
	leave
	ret

;----------------
; Funkce ReadDouble
; Precte 64bitove realne cislo z klavesnice
; Chovani funkci je dano chovanim funkce SSCANF. Veskere namitky se netykaji autora.
;
ReadDouble:
	enter 0,0
	pushfd
	push esi
	push ebx
	push eax

	mov esi,msg_EnterFloat
	call WriteString
	MOVSTR ebx,"%lf"
	call ReadNumber
	fld qword [number_in]

.return:

	pop eax
	pop ebx
	pop esi
	popfd
	leave
	ret

;----------------
; WriteChar
; Vypise znak ulozeny v registru AL
;
WriteChar:
	enter 0,0
	push eax
	push ecx
	push edx	

	PRINTF "%c",eax

	pop edx
	pop ecx
	pop eax
	leave
	ret

;----------------
; WriteNewLine
; Vypise "End Of Line", tedy ASCII hodnoty CR, LF ve Windows (13, 10) a LF v Linuxu (10)
;
WriteNewLine:
	enter 0,0
	pushfd
	push eax
	push ecx
	push edx

	PRINTF "%s",msg_EOL

	pop edx
	pop ecx
	pop eax
	popfd
	leave
	ret

;----------------
; Funkce WriteBinX (8, 16, 32) {WriteBin8, WriteBin16, WriteBin32}
; Vypise 8,16,32 bitu z registru AL, AX, EAX
;
WriteBin8:
	enter 0,0
	pushfd
	pushad

	mov ecx,8
	rcl eax,24
	jmp WriteBin

WriteBin16:
	enter 0,0
	pushfd
	pushad

	mov ecx,16
	rcl eax,16
	jmp WriteBin

WriteBin32:
	enter 0,0
	pushfd
	pushad

	mov ecx,32

WriteBin:
	cld
	mov edi,ioBuffer
	mov ebx,eax

.next_bit:
	rcl ebx,1
	setc al
	add al,'0'
	stosb
	loop .next_bit

	mov [edi], byte 0
	mov esi,ioBuffer
	call WriteString

	popad
	popfd
	leave
	ret

;----------------
; WriteString
; Vypise retezce ukonceneho 0 (ASCIIZ), jehoz adresa je ulozena v registru ESI
;
WriteString:
	enter 0,0
	pushfd
	push eax
	push ecx
	push edx

	PRINTF "%s", esi

	pop edx
	pop ecx
	pop eax
	popfd
	leave
	ret

;----------------
; WriteFlags
; Vypise stav registru priznaku
;
WriteFlags:
	enter 0,0
	pushfd
	pushad

	pushfd
	pop ebx
	shl ebx,20

	mov ecx,12
	mov edi,msg_FlagValues

.cycle:
	rcl ebx,1
	setc al
	add al,'0'
	mov [edi],al
	add edi,3
	loop .cycle

	mov esi,msg_Flags
	call WriteString

	popad
	popfd
	leave
	ret

;----------------
; Funkce WriteIntX (8,16,32) {WriteInt8,WriteInt16,WriteInt32}
; Vypise 8-, 16-, 32-bitovou celociselnou hodnotu se znamenkem z registru AL, AX, EAX
;
;----------------
; Funkce WriteUIntX (8,16,32) {WriteUInt8,WriteUInt16,WriteUInt32}
; Vypise 8-, 16-, 32-bitovou celociselnou hodnotu bez znamenka z registru AL, AX, EAX
;
; Chovani funkci je dano chovanim funkce PRINTF a prislusnym formatovacim retezcem.
;

WriteInt8:
	enter 0,0
	pushfd
	push eax
	movsx eax,al
	jmp WriteInt

WriteUInt8:
	enter 0,0
	pushfd
	push eax
	movzx eax,al
	jmp WriteUInt

WriteInt16:
	enter 0,0
	pushfd
	push eax
	movsx eax,ax
	jmp WriteInt 

WriteUInt16:
	enter 0,0
	pushfd
	push eax
	movzx eax,ax
	jmp WriteUInt

WriteInt32:
	enter 0,0
	pushfd
	push eax

WriteInt:
	push ecx
	push edx

	PRINTF "%d", eax

	pop edx
	pop ecx
	pop eax
	popfd
	leave
	ret

WriteUInt32:
	enter 0,0
	pushfd
	push eax

WriteUInt:
	push ecx
	push edx

	PRINTF "%u", eax

	pop edx
	pop ecx
	pop eax
	popfd
	leave
	ret

;----------------
; Funkce WriteHexX (8,16,32) {WriteHex8,WriteHex16,WriteHex32}
; Vypise hexadecimalne 8-, 16-, 32-bitovou celociselnou hodnotu bez znamenka z registru AL, AX, EAX
;
; Chovani funkci je dano chovanim funkce PRINTF a prislusnym formatovacim retezcem.
;

WriteHex8:
	enter 0,0
	pushfd
	push eax
	push esi
	movzx eax,al
	MOVSTR esi,"0x%02X"
	jmp WriteHex

WriteHex16:
	enter 0,0
	pushfd
	push eax
	push esi
	movzx eax,ax
	MOVSTR esi,"0x%04X"
	jmp WriteHex

WriteHex32:
	enter 0,0
	pushfd
	push eax
	push esi	
	MOVSTR esi,"0x%08X"

WriteHex:
	push ecx
	push edx

	PRINTF esi, eax

	pop edx
	pop ecx
	pop esi
	pop eax
	popfd
	leave
	ret

;----------------
; Funkce WriteFloat
; Vypise 32-bitove realne cislo z registru EAX
;
WriteFloat:
	enter 0,0
	pushfd
	push eax
	push ecx
	push edx

	push eax
	fld dword [esp]
	sub esp, 4
	fstp qword [esp]	
	PRINTF "%f"			; printf vyzaduje vzdy double!
	add esp,8

	pop edx
	pop ecx
	pop eax
	popfd
	leave
	ret

;----------------
; Funkce WriteDouble
; Vypise 64-bitove realne cislo z registru koprocesoru ST0
;
WriteDouble:
	enter 0,0
	pushfd
	push eax
	push ecx
	push edx

	sub esp,8
	fst qword [esp]	
	PRINTF "%lf"
	add esp,8

	pop edx
	pop ecx
	pop eax
	popfd
	leave
	ret

;----------------
; Funkce WriteAllGPRegisters32
; Vypise pole cisel, ukazatel na pole je v ESI, pocet cisel je v ECX
;
WriteAllGPRegisters32:
	enter 0,0
	PRINTF `EAX=0x%08X,EBX=0x%08X,ECX=0x%08X,EDX=0x%08X,ESI=0x%08X,EDI=0x%08X,EBP=0x%08X,ESP=0x%08X\n`, eax, ebx, ecx, edx, esi, edi, ebp, esp
	leave
	ret

;----------------
; Funkce WriteTestGPRegisters32
; Vypise pole cisel, ukazatel na pole je v ESI, pocet cisel je v ECX
;
WriteTestGPRegisters32:
	enter 0,0
	PRINTF `EBX=0x%08X,ECX=0x%08X,EDX=0x%08X,ESI=0x%08X,EDI=0x%08X,EBP=0x%08X\n`, ebx, ecx, edx, esi, edi, ebp
	leave
	ret

%macro CREATE_WRITE_ARRAY 2.nolist
; datovy typ (b,w,d), funkce pro vypis z RW32
WriteArray%2:
	enter 0,0
	pushfd
	push esi
	push ecx
	push eax

%%writeElement:
  	lods%1
   	call Write%2
   	cmp ecx,1
   	jbe %%skipComma
   	mov al,','
   	call WriteChar
	mov al,' '
   	call WriteChar
%%skipComma:
   	loop %%writeElement

   	call WriteNewLine
   	pop eax
   	pop ecx
   	pop esi
	popfd
	leave
	ret
%endmacro

;----------------
; Funkce WriteArrayInt8
; Vypise pole cisel, ukazatel na pole je v ESI, pocet cisel je v ECX
;
CREATE_WRITE_ARRAY b, Int8

;----------------
; Funkce WriteArrayInt16
; Vypise pole cisel, ukazatel na pole je v ESI, pocet cisel je v ECX
;
CREATE_WRITE_ARRAY w, Int16

;----------------
; Funkce WriteArrayInt32
; Vypise pole cisel, ukazatel na pole je v ESI, pocet cisel je v ECX
;
CREATE_WRITE_ARRAY d, Int32

