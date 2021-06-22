
# Implementace Simple FTP dle RFC 913

Implementace CLI aplikace klienta a serveru protokolu `Simple FTP` podle `RFC 913` v jazyce `C`.
## Klient
Klient je velmi jednoduchý a snaží se přiblížit protkol `Simple FTP`. Ovládání je zadáváním příkazů popsaných v `RFC 913` klávesou `enter`, které jsou přímo zasílány na serverovou část aplikace, kde probíhá většina režie. Klientská část, kromě zasílání příkazů, navíc provadí kontroly zadaných příkazů a je doplněna vlastními informativními hláškami.
### spuštění:
>./ipk-simpleftp-client -h <ip_adresa_serveru> {-p <port_serveru> }
> (defaultní port je 115)
>příklad: ./ipk-simpleftp-client -p 65001 -h 147.229.176.19

Stahování souborů probíhá přes standardní výstup a uložení dat je již na uživateli. Uživatel při stahování může přesměrovat standardní výstup do souboru a postažení ze souboru odstranit přebytečné řádky. (Tento způsob implementace je zvolen pro zachování, co největší jednoduchosti klienta.) 
Klienta je ale možné obsluhovat i za pomocí `"skriptů"`, neboli souborů obsahující příkazy z `RFC 913` oddělené řádky. Tento soubor pak stačí přesměrovat na standardní vstup aplikace a dojde k provedení jeho obsahu.

## Server
Z pohledu uživatele je serverová aplikace velmi jednoduchá. Po spuštění se do konzole vypisují zprávy popisující dění na serveru a zároveň je možné přes standardní vstup zadat pouze jediný příkaz `DONE` potvrzený klávesou `enter`, který server ukončí a odpojí všechny klienty.

### spuštění:
>./ipk-simpleftp-server {-i <ethernetové_rozhraní>} {-p ­­<port_serveru>} -u <cesta_k_souboru_s_uživateli>
> (defaultní port je 115)
> (při nezadání konkrétního rozhraní, jsou použita všechna dostupná)
>příklad: ./ipk-simpleftp-server -u ./users.txt -p 65001

Po připojení nového klienta je vytvořen proces, který daného klienta obsluhuje.

## Poznámky:
Z důvodu časové vytíženosti nebyly implementovány příkazy `STOR` a `SIZE` pro odesílání souborů na server. Dokumentace/manuál také chybí. 
Aplikaci by bylo potřeba ještě více odladit a dokončit.

Projekt byl implementován a testován pouze na serveru merlin (nebyla možnost využití a spuštění virtuálního zařízení), takže je možné, že projekt nebude na referenčním zařízení přeložitelný a spustitelný!

Je škoda, že se nepodařilo projekt dokončit, zadání bylo zajímavé.
