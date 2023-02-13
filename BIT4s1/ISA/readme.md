# Atom & RSS 2.0 feed reader
Čtečka zdrojů ve formátu [Atom ](https://www.rfc-editor.org/rfc/rfc4287)nebo [RSS 2.0](https://www.rssboard.org/rss-specification) ([RSS](https://www.cs.mcgill.ca/~rwest/wikispeedia/wpcd/wp/r/RSS_%2528file_format%2529.htm)). Zdroj URL lze zadat přímo jako parametr nebo přes přiložený soubor obsahující seznam zdrojů.

## Instalace
V kořenové složce projektu pomocí příkazu `make`. Testováno na serveru `merlin.fit.vutbr.cz`.
Spuštění testů: `make test`

## Použití
`feedreader <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]`

- první argument musí být URL adresa (s http(s)) zdroje ke čtení nebo značka `-f`
-- značku `-f` musí ihned následovat cesta k souboru (každá adresa zdroje se nachází zvlášť na řádku; řádky začínající `#` jsou považovány za komentáře a jsou ignorovány)
- u dalších argumentů nezáleží na pořadí
-- značka `-T` - zobrazení času aktualizace nebo času publikování článku ve výpisu, pokud je nalezeno
-- značka `-a` - zobrazení jména autora článku nebo jeho emailové adresy ve výpisu, pokud je nalezena
-- značka `-u` - zobrazení URL adresy článku ve výpisu, pokud je nalezena
-- argument `-c <certfile>` - použití konkrétního SSL certifikátu při stahování zdrojů
-- argument `-C <certaddr>` - použití SSL certifikátů z určitého adresáře při stahování zdrojů

Příklady:
`feedreader https://what-if.xkcd.com/feed.atom -T -a -u`
`feedreader -f feeds.txt -T -u -a`
`feedreader -f feeds.txt -u -C path/to/certs/`

## Poznámky
- Implementace je většinově podle zadání. 
- Nebylo otestováno použití argumentů`-c <certfile>` a `-C <certaddr>`.
- Parsování dokumentu nekontroluje zdroj přesně podle standardů. Program vrátí chybu v případě špatného TOP elementu zdroje (`<rss>` nebo `<feed>`), při chybějícím titulku feedu (`<title>`) nebo při nenalezení titulku žádného záznamu/článku. Toto je kontrolováno podle úrovně zanoření daného elementu zdroje.

V tuto chvíli nejsou známé žádné bugy v projektu.
Dokumentace chybí.
