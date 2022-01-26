# IPT.py
import math;
import statistics;
from collections import Counter;
import os;

#os.system('clear');


print("IPT");

print("Q1 ----------");



soubor_hodnot_relativni_kumulativni_cetnosti_x = [3, 5, 6, 8, 10];
soubor_hodnot_relativni_kumulativni_cetnosti_rkc = [0.2, 0.4, 0.65, 0.7, 1];
pocet_hodnot = 20;

print("POCET HODNOT: "+str(pocet_hodnot));
print("DATA [xi]: "+str(soubor_hodnot_relativni_kumulativni_cetnosti_x));
print("DATA [rel kum cet]: "+str(soubor_hodnot_relativni_kumulativni_cetnosti_rkc));

rel_cetn = [];
for index, val in enumerate(soubor_hodnot_relativni_kumulativni_cetnosti_rkc):
    if index == 0:
        rel_cetn.append(val);
    else:
        rel_cetn.append(val - soubor_hodnot_relativni_kumulativni_cetnosti_rkc[index-1]);


print(rel_cetn);

print([float("%.2f" % v) for v in rel_cetn]);

rel_cetn2 = [0.2, 0.3, 0.15, 0.25, 0.1];
print(rel_cetn2);

rel_cetn2 = [float("%.2f" % v) for v in rel_cetn];




abs_cetn = [pocet_hodnot*val for val in rel_cetn2];



print(abs_cetn);

print([float("%.2f" % v) for v in abs_cetn]);

abs_cetn2 = [4, 6, 3, 5, 2];
print(abs_cetn2);

abs_cetn2 = [int(float("%.2f" % v)) for v in abs_cetn];


print(abs_cetn2);


print();
print("Hodnoty:      "+str(soubor_hodnot_relativni_kumulativni_cetnosti_x));
print("Pocet hodnot: "+str(abs_cetn2));

xi_hodnoty = [];
for index, val in enumerate(abs_cetn2):
    for inx in range(0, val):
        xi_hodnoty.append(soubor_hodnot_relativni_kumulativni_cetnosti_x[index]);


print();
print("Hodnoty: "+str(xi_hodnoty));
print();

arm_prum = sum(xi_hodnoty)/pocet_hodnot;
vyb_rozptyl = (sum([x*x for x in xi_hodnoty]) - len(xi_hodnoty)*(arm_prum*arm_prum))/(len(xi_hodnoty)-1)

print("Aritmeticky prumer: "+str(arm_prum));
print("Vyberovy rozptyl: "+str(vyb_rozptyl));
print("Modus: "+str(statistics.mode(xi_hodnoty)));
print("Median: "+str(statistics.median(xi_hodnoty)));



print("Q2 ----------");

soubor_hodnot = [4, 2, 2, 4, 1];

aritmeticky_prumer = sum(soubor_hodnot) / len(soubor_hodnot);
vyberovy_rozptyl = (sum([x*x for x in soubor_hodnot]) - len(soubor_hodnot)*(aritmeticky_prumer*aritmeticky_prumer))/(len(soubor_hodnot)-1);
vyberova_smerodatna_odchylka = math.sqrt(vyberovy_rozptyl);

print("DATA: "+str(soubor_hodnot));
print("Prumer: "+str(aritmeticky_prumer));
print("Rozptyl: "+str(vyberovy_rozptyl));
print("Odchylka: "+str(vyberova_smerodatna_odchylka));



print("Q3 ----------");


soubor_hodnot2 = [8, 5, 6, 4, 7, 4, 7, 8, 5, 7];

modus = statistics.mode(soubor_hodnot2);
median = statistics.median(soubor_hodnot2);

print("DATA: "+str(soubor_hodnot2));
print("Modus: "+str(modus));
print("Median: "+str(median));



print("Q4 ----------");

soubor_hodnot3 = [10, 7, 10, 7, 8, 9, 9, 10, 8, 6];
hodnota = 8;

absolutni_cetnost = len([x for x in soubor_hodnot3 if x == hodnota]);
relativni_cetnost = absolutni_cetnost / len(soubor_hodnot3);
absolutni_kumulativni_cetnost = len([x for x in soubor_hodnot3 if x <= hodnota]);
relativni_kumulativni_cetnost = absolutni_kumulativni_cetnost / len(soubor_hodnot3);


print("DATA: "+str(soubor_hodnot3));
print("HODNOTA: "+str(hodnota));
print("Absolutni cetnost: "+str(absolutni_cetnost));
print("Relativni cetnost: "+str(relativni_cetnost));
print("Absolutni kumulativni cetnost: "+str(absolutni_kumulativni_cetnost));
print("Relativni kumulativni cetnost: "+str(relativni_kumulativni_cetnost));

print("Q5 ----------");


soubor_hodnot4 = [3, 3, 1, 4, 2, 1, 1, 2, 4, 4];
hodnoty = [x for x in Counter(soubor_hodnot4)];
hodnoty.sort();
print("DATA: "+str(soubor_hodnot4));
print("HODNOTY: "+str(hodnoty));


absolutni_cetnosti = [];
relativni_cetnosti = [];
absolutni_kumulativni_cetnosti = [];
relativni_kumulativni_cetnosti = [];


for hodnota3 in hodnoty:

    absolutni_cetnost = len([x for x in soubor_hodnot4 if x == hodnota3]);
    relativni_cetnost = absolutni_cetnost / len(soubor_hodnot4);
    absolutni_kumulativni_cetnost = len([x for x in soubor_hodnot4 if x <= hodnota3]);
    relativni_kumulativni_cetnost = absolutni_kumulativni_cetnost / len(soubor_hodnot4);

    absolutni_cetnosti.append(absolutni_cetnost);
    relativni_cetnosti.append(relativni_cetnost);
    absolutni_kumulativni_cetnosti.append(absolutni_kumulativni_cetnost);
    relativni_kumulativni_cetnosti.append(relativni_kumulativni_cetnost);


print("Absolutni cetnosti: "+str(absolutni_cetnosti));
print("Relativni cetnosti: "+str(relativni_cetnosti));
print("Absolutni kumulativni cetnosti: "+str(absolutni_kumulativni_cetnosti));
print("Relativni kumulativni cetnosti: "+str(relativni_kumulativni_cetnosti));
print();
print("Vysledek: \nCetnosti(G2);\nRelativni cetnosti(G3);\nAbsolutni kumulativni cetnosti(G4);\nRelativni kumulativni cetnosti(G1)");


print("----------");
