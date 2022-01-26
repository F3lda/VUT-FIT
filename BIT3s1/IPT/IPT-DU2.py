# IPT.py
import math;
import statistics;
from collections import Counter;
import os;

#os.system('clear');


print("IPT");
print("----------");

soubor_hodnot = [73, 46, 38, 61, 53, 37, 49, 44];

aritmeticky_prumer = sum(soubor_hodnot) / len(soubor_hodnot);
vyberovy_rozptyl = (sum([x*x for x in soubor_hodnot]) - len(soubor_hodnot)*(aritmeticky_prumer*aritmeticky_prumer))/(len(soubor_hodnot)-1);
vyberova_smerodatna_odchylka = math.sqrt(vyberovy_rozptyl);

print("DATA: "+str(soubor_hodnot));
print("Prumer: "+str(aritmeticky_prumer));
print("Rozptyl: "+str(vyberovy_rozptyl));
print("Odchylka: "+str(vyberova_smerodatna_odchylka));



print("Q1 ----------");
print("Interval stredni hodnoty - Normalni rozdeleni");


soubor_hodnot2 = [51,51,49,51,52,59,55,52,59,65];

stred_hodnota = sum(soubor_hodnot2) / len(soubor_hodnot2);
rozptyl = (sum([x*x for x in soubor_hodnot2]) - len(soubor_hodnot2)*(stred_hodnota*stred_hodnota))/(len(soubor_hodnot2)-1);
smerodatna_odchylka = math.sqrt(rozptyl);

print("DATA: "+str(soubor_hodnot2));
print("Bodovy odhad stredni hodnoty: "+str(stred_hodnota));
print("Rozptyl: "+str(rozptyl));
print("Odchylka: "+str(smerodatna_odchylka));

kvantil1 = 1.833;
Lint1 = stred_hodnota-(kvantil1*(smerodatna_odchylka/math.sqrt(len(soubor_hodnot2))));
Rint1 = stred_hodnota+(kvantil1*(smerodatna_odchylka/math.sqrt(len(soubor_hodnot2))));
print();
print(0);
print(Rint1);


print("Q2 ----------");
print("Normalni rozdeleni pri znamem rozptylu");


soubor_hodnot23 = [332,323,330,336,334,332,325,335,326,324];
smer_dochylka = 5;
#smer_dochylka = math.sqrt(rozptl);

stred_hodnota2 = sum(soubor_hodnot23) / len(soubor_hodnot23);
rozptyl2 = (sum([x*x for x in soubor_hodnot23]) - len(soubor_hodnot23)*(stred_hodnota2*stred_hodnota2))/(len(soubor_hodnot23)-1);
smerodatna_odchylka2 = math.sqrt(rozptyl2);

print("DATA: "+str(soubor_hodnot23));
print("Bodovy odhad stredni hodnoty: "+str(stred_hodnota2));
print("Rozptyl: "+str(rozptyl2));
print("Odchylka: "+str(smerodatna_odchylka2));

kvantil2 = 1.960;
Lint2 = stred_hodnota2-(kvantil2*(smer_dochylka/math.sqrt(len(soubor_hodnot23))));
Rint2 = stred_hodnota2+(kvantil2*(smer_dochylka/math.sqrt(len(soubor_hodnot23))));
print();
print(Lint2);
print(Rint2);


print("----------");


print("Q3 ----------");
print("Normalni rozdeleni - interval spolehlivosti pro rozptyl");


soubor_hodnot234 = [36,25,47,61,56,48,35,57,42,43];


stred_hodnota24 = sum(soubor_hodnot234) / len(soubor_hodnot234);
rozptyl24 = (sum([x*x for x in soubor_hodnot234]) - len(soubor_hodnot234)*(stred_hodnota24*stred_hodnota24))/(len(soubor_hodnot234)-1);
smerodatna_odchylka24 = math.sqrt(rozptyl24);

print("DATA: "+str(soubor_hodnot234));
print("Bodovy odhad stredni hodnoty: "+str(stred_hodnota24));
print("Rozptyl: "+str(rozptyl24));
print("Odchylka: "+str(smerodatna_odchylka24));

kvantil24_L = 23.589;
kvantil24_R = 1.735;
Lint3 = ((len(soubor_hodnot234)-1)*rozptyl24)/kvantil24_L;
Rint3 = ((len(soubor_hodnot234)-1)*rozptyl24)/kvantil24_R;
print();
print(Lint3);
print(Rint3);


print("----------");
