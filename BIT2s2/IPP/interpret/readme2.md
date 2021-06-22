# Projekt IPP 2020/2021 - 2. úloha
Implementační dokumentace k 2. úloze do IPP 2020/2021
Jméno a příjmení: Karel Jirgl
Login: xjirgl01

## Implementace interpret.py

Implementace interpretu kódu IPPcode21 ve formátu XML je rozdělena na tři hlavní části:

### 1) Parsování argumentů programu
Podle použitých nebo nepoužitých argumentů se do proměnné `source_stream` vloží proud, ze kterého bude čten zdrojový kód určený k interpretaci, a do proměnné `input_stream` se vloží vstupní proud pro interpretovaný program. 
>Např. pokud je připojen argument `--input`, vloží se do proměnné `input_stream` proud souboru zadaného argumentem. V opačném případě je použit proud ze standardního vstupu.

### 2) Parsování XML dat
Dochazí k postupnému čtení proudu zdrojového kódu (z proměnné `source_stream`) a předávání dat XML parseru, který hlásí údálosti `start` (při počátečním elementu) a `end` (při koncovém elementu). Podle aktuálního zanoření jsou očekávány elementy s určitým názvem a atributy, z nichž se získávají instrukce a jejich argumenty, které se ukládají do instancí tříd `Instruction` a `Argument` pro další použití v intepretu.
Při neočekávané XML struktuře je program ukončen s návratovou hodnotou `32` a u chybného XML formátu je vrácena hodnota `31`. V této části jsou také převedeny `Escape` sekvence řetězců na příslušné znaky.
> **Poznámka:** Při implementaci kontroly číslování instrukcí jsem narazil na nejasnost, zda instrukce v XML formátu musí být číslovány postupně (1, 2, 3, 4, 5, ...), anebo na tom nezáleží a čísla mohou být přeskakována (1, 3, 5, 10, 23, ...).
> 
> V zadání se píše: _"Každý element `instruction` obsahuje povinný atribut `order` s pořadím instrukce (počítáno od 1, souvislá posloupnost)"_ ale v příkladech testů v souboru `stack_test.src` právě tato posloupnost dodržena nebyla.
>
> Nevím jestli jsme správně pochopil větu ze zadání, ale moje řešení jsem se rozhodl implementovat tak, že očekává postupné pořadí instrukcí bez přeskakování čisel (1, 2, 3, ...).

### 3) Interpret
Pro paměťové rámce intepretu jsou vytvořeny třídy `SymTable` (pro ukládání proměnných a návěští) a `Stack` (pro ukládání dat a lokálních rámců). 
Před spuštěním interpretace dojde k naplnění tabulky symbolů návěští a poté Interpret jednoduše v cyklu prochází a provádí instrukci po instrukci podle jejího čísla a názvu. 

>K interpretu patří pomocné funkce k ověřováníní typu parametru instrukce (`is_var(arg)`, `is_label(arg)`, `is_constant(typ,arg)`, `is_symbol(typ,arg)`, `is_type(arg)`), funkce pro získání hodnoty a typu ze symbolu (`interpret_symbol_get_value_and_type(symbol_type, symbol_value)`) a funkce k uložení hodnoty do proměnné (`interpret_var_set_value(var, typ, value)`).

## Implementace test.php
Implementace testovacího rámce je rozdělena na tři části:
#### 1) načtení argumentů programu a kontrola existence adresářů a zdrojových souborů
#### 2) načtení názvů testovacích souborů rekurzivně nebo nerekurzivně
#### 3) příprava na testování a samotné testování podle testovacích souborů
Implementace testovacího rámce nebyla dokončena.

