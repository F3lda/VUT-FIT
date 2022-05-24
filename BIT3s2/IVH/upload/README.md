# Nahravání do FITkitu
Poznámka: z ISE vytvoříte soubor .bin. Soubory hex jsou pro MCU, které nevyužíváte. Proto jsou už přiložené v této složce.

## Varianta 1: Vlastní fitkit připojený k virtual boxu
Proveďte build přes Xilinx ISE. Dále pomocí fkflash (a změňte správně cesty)

```
fkflash --devid=0  --hexfile="testled_f1xx.hex" --hexfile2="testled_f2xx.hex" --binfile="testled.bin"
fkterm -d 0
```

## Varianta 2: Vlastní fitkit bez virtual boxu
Pokud nemáte vůbec virtual box, nebo vám k němu nejde připojit USB zařízení,
můžete využít multiplatformní nástroj pro nahrávání binárního kódu.

https://github.com/janchaloupka/fitkit-serial/releases

Pak stáhnete z VirtualBoxu přes sdílenou složku bin soubor a nahrajete jej

```
fitkit-serial-win-amd64.exe -hex1 testled_f1xx.hex -hex2 testked_f2xx.hex -bin testled.bin"
```


## Varianta 3: Vzdálený FitKit

Přihlašte se na adresu

https://ehw.fit.vutbr.cz/ivh

A nahrajte tam soubor bin. 

