# Projekt IPP 2020/2021 - 1. úloha
Implementační dokumentace k 1. úloze do IPP 2020/2021
Jméno a příjmení: Karel Jirgl
Login: xjirgl01

## Implementace 

Implementace analyzátoru kódu IPPcode21 je rozdělena do tří hlavních bloků.

### 1) Kontrola vstupních parametrů
Než se začnou načítat data ze standartního vstupu, zkontrolují se vstupní parametry skriptu. U skriptu jsou pouze **dvě možné kombinace parametrů**.
- **Zachycení jednoho parametru `--help`** -> vypíše nápovědu skriptu na standardní výstup a skript je ukončen s návratovou hodnotou `0`
- **Skript bez vstupních parametrů** -> skript pokračuje dále a začíná načítat data ze standardního vstupu

Při jiných kombinacích a počtech vstupních parametrů je skript ukončen a je vrácena chybová návratová hodnota `10`.

### 2) Nalezení hlavičky zdrojového kódu
Standartní vstup je načítán a kontrolován řádek po řádku. Velikost načítaného řádku je omezena na `1024` znaků včetně znaků nového řádku. Při přesažení délky řádku je skript ukončen s návratovou hodnotou `99`.  
Z každého řádku jsou ořezány **bílé znaky** a zbylý řetězec je převeden na **velká písmena**. Je-li řetězec prázdný, pokračuje se dále čtením dalšího řádku. Pokud je řetězec neprázdný, porovná se s očekávaným řetězcem hlavičky (`".IPPCODE21"`).  V případě **nerovnosti** řetězců nebo dosažení konce souboru je skript ukončen s chybovou návratovou hodnotou `21`. Při **rovnosti** řetězců je hlavička souboru nalezena a skript pokračuje analýzou následujícího zdrojového kódu na standartním vstupu.


### 3) Lexikální a syntaktická analýza
Před analýzou zdrojového kódu ze standartního vstupu je definováno **pole s názvy instrukcí a typy jejich parametrů** a dále jsou definovány **funkce pro kontrolu typu parametrů** načtené instrukce (`is_var($arg)`, `is_label($arg)`, `is_constant($arg)`, `is_symbol($arg)`, `is_type($arg)`). 

Standartní vstup je opět parsován po jednom řádku a velikost načítaného řádku je omezena na `1024` znaků včetně znaků nového řádku. 
Načtený řádek je nejprve rozdělen na podřetězce podle znaku `#` signalizující **komentář** a dále se již pracuje pouze s prvním podřetězcem (s řetězcem před komentářem).

> **Poznámka:** tento způsob implementace uznává například tento řádek jako platný (bez mezery mezi instrukcí a komentářem):
> `WRITE string@ZdeJeKonstanta#a zde už je komentář`
> Je to z důvodu, že znak `#` je v konstantách zakázaný a v zadání nebyl tento případ detailněji popsán.

Ze zbývajícího řetězce jsou ořezány **bílé znaky** a pokud není řetězec prázdný, pokračuje se analýzou instrukce. V opačném případě se pokračuje čtením dalšího řádku.

Řetězec s instrukcí se rozdělí podle **bílých znaků** na podřetězce. První podřetězec je **název instrukce** a další podřetězce jsou **parametry** této instrukce. Zkontroluje se, že **název instrukce** existuje v poli s definovanými instrukcemi, a jelikož jsou instrukce `"case-insensitive"`, je ještě předtím název instrukce převeden na velká písmena. Pokud instrukce existuje, zkontrolují se postupně **typy** všech jejích parametrů a nakonec jejich **počet**. Pokud je všechno v pořádku, pokračuje se čtením další instrukce, jinak při nerozpoznání instrukce je skript ukončen s návratovou hodnotou `22` a při špatném typu parametru nebo při špatném počtu parametrů instrukce je skript ukončen s návratovou hodnotou `23`.

Během celého procesu analýzy se vypisuje na standartní výstup výsledná **XML interpretace** zdrojového kódu.


Pokud vše proběhlo v pořádku, je skript ukončen s návratovou hodnotou `0`.
