###!/bin/bash

# testuje spravnou sekvenci cislovani akci a format vystupu (pouze syntax)
# bez zaruky, muze obsahovat chyby

outf=$1

# odstraneni bilych znaku
tr -d " \t" < ${outf} > $$

# test cislovani akci
# tiskne radky s chybnym cislovanim
cat $$ | awk -F":" ' { c++; if (c != $1) print NR, " => ", $1, " : chyba v cislovani akci"; } '

declare -a lines
lines[0]='^[1-9][0-9]*:IMM[1-9][0-9]*:starts$'
lines[1]='^[1-9][0-9]*:IMM[1-9][0-9]*:enters:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[2]='^[1-9][0-9]*:IMM[1-9][0-9]*:checks:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[3]='^[1-9][0-9]*:IMM[1-9][0-9]*:wantscertificate:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[4]='^[1-9][0-9]*:IMM[1-9][0-9]*:gotcertificate:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[5]='^[1-9][0-9]*:IMM[1-9][0-9]*:leaves:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'

lines[6]='^[1-9][0-9]*:JUDGE:wantstoenter$'
lines[7]='^[1-9][0-9]*:JUDGE:enters:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[8]='^[1-9][0-9]*:JUDGE:waitsforimm:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[9]='^[1-9][0-9]*:JUDGE:startsconfirmation:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[10]='^[1-9][0-9]*:JUDGE:endsconfirmation:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[11]='^[1-9][0-9]*:JUDGE:leaves:[0-9][0-9]*:[0-9][0-9]*:[0-9][0-9]*$'
lines[12]='^[1-9][0-9]*:JUDGE:finishes$'

# kontrola sytaxe vystupu
# vytiskne radky, ktere neodpovidaji formatu vytupu
echo "=== radky, ktere neodpovidaji formatu vystupu"

for i in `seq 0 13`
do
    echo "/${lines[$i]}/d" >> $$-sed
done

sed -f $$-sed $$

# kontrola chybejicich vystupu
# tiskne informaci, ktery text ve vystupu chybi
echo "=== chybejici vystupy"
echo ">> chybejici vystup 'waits for imm'"
echo ">>>> nemusi byt chyba, k situaci, kdy soudce ceka na registraci, dochazi mene casto, prip."
echo ">>>> k ni nemusi dojit vubec"

for i in `seq 0 13`
do
    cat $$ | grep "${lines[$i]}" >/dev/null || echo "${lines[$i]}"
done


rm $$*
