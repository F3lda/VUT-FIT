/**
 * @file main.c (VUT FIT - IMS projekt zadání)
 * 
 * @brief Generátor zadání podle vstupních dat
 * @date 2022-11-29
 * @author Karel Jirgl (xjirgl01), Thanh Quang Tran (xtrant02)
 * @update 2022-12-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// VSTUPNÍ DATA
// Zásilkovna 2020
//-----------------
#define DAT_DEPA 27
#define DAT_DODAVKY 350
#define DAT_KAMIONY 110
#define DAT_OBRAT 2500000000
#define DAT_ZASILKY 40500000
#define DAT_VYDEJNY 5843
#define DAT_BOXY 107
#define DAT_ZAMESTNANCI 1500
#define DAT_REKORD_ZASILKY 506000 // Zásilek přepravených za jeden den (14. 12. 2020) 
// Zdroje:
// https://www.zasilkovna.cz/blog/packeta-v-roce-2020-zdvojnasobila-svuj-obrat
// https://www.zasilkovna.cz/o-nas


// Další vstupní údaje (odhadnuté/vymyšlené)
//-----------------
#define CONST_PRACOVNI_DNY_ROK 255 // počet pracovních dnů v roce (https://vimjakna.cz/dny/pracovni-dny/)
#define CONST_ZAMESTNANCI_DEPA_KOEFICIENT 0.666 // 2/3 ze všech zaměstnanců jsou na depech ve skladech
#define CONST_ZASILKA_NALOZENI 30 // 30 sekund trvá naložení jedné zásilky jedním zaměstnancem
#define CONST_ZASILKA_VYLOZENI 45 // 45 sekund trvá vyložení jedné zásilky jedním zaměstnancem

#define CONST_KAMION_DODAVEK 6 // 1 kamion = 6 dodávek objemově (Zdroj: https://www.denik.cz/auto/svet-ridice-kamionu-neni-to-jednoducha-prace.html)
#define CONST_KAMION_CESTA_DEPA_MAX 5 // Kamion může na trase navštívit až 5 dep
#define CONST_KAMION_CESTA_DEPO_MIN 45 // Cesta kamionu mezi 2 depy - minimum minut
#define CONST_KAMION_CESTA_DEPO_MAX 120 // Cesta kamionu mezi 2 depy - maximum minut

#define CONST_DODAVKA_REZIE 5 // 5 minut - čas na režii na výdejním místě (papírování, svačina, záchod, povinná 30 minutová přestávka na oběd)
#define CONST_DODAVKA_CESTA_MIN 10 // 10 minut - trvání cesty na další výdejní místo - minimum
#define CONST_DODAVKA_CESTA_MAX 15 // 15 minut - trvání cesty na další výdejní místo - maximum
#define CONST_DODAVKA_NAVRAT_MIN 15 // 15 minut - vynucený návrat z důvodu konce směny - minimum
#define CONST_DODAVKA_NAVRAT_MAX 45 // 45 minut - vynucený návrat z důvodu konce směny - maximum

#define CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL 10 // +- 10 minut - rozptyl pro čas na nakládání nebo vykládání zásilek z dodávky nebo kamionu





int main() // int argc, char *argv[]
{
    printf("|--------------|\n");
    printf("| VSTUPNÍ DATA |\n");
    printf("|--------------|\n");
    printf("\n");
    printf("Zásilkovna (statistika 2020)\n");
    printf("----------------------------\n");
    printf("- %d dep (skladů)\n", DAT_DEPA);
    printf("- %d kamionů mezi depy\n", DAT_KAMIONY);
    printf("- %d dodávek do výdejen\n", DAT_DODAVKY);
    printf("- %d výdejních míst\n", DAT_VYDEJNY);
    printf("- %d Z-Boxů\n", DAT_BOXY);
    printf("- %d zaměstnanců\n", DAT_ZAMESTNANCI);
    printf("- %d rozvezených zásilek\n", DAT_ZASILKY);
    printf("- %ld Kč obrat\n", DAT_OBRAT);
    printf("- REKORD: %d zásilek přepravených za jeden den (14. 12. 2020)\n", DAT_REKORD_ZASILKY);
    printf("\n");
    printf("Zdroje:\n");
    printf("https://www.zasilkovna.cz/blog/packeta-v-roce-2020-zdvojnasobila-svuj-obrat\n");
    printf("https://www.zasilkovna.cz/o-nas\n");
    printf("\n");
    printf("\n");
    printf("\n");



    printf("Odhadnutá data\n");
    printf("--------------\n");
    printf("- %d dnů v roce je pracovních (Zdroj: https://vimjakna.cz/dny/pracovni-dny/ - přibližně)\n", CONST_PRACOVNI_DNY_ROK);
    printf("- %f ze všech zaměstnanců je na depech ve skladech (koeficient)\n", CONST_ZAMESTNANCI_DEPA_KOEFICIENT);
    printf("- %d sekund trvá naložení jedné zásilky jedním zaměstnancem\n", CONST_ZASILKA_NALOZENI);
    printf("- %d sekund trvá vyložení jedné zásilky jedním zaměstnancem (vyložení trvá déle, protože je nutné zásilku správně zařadit/roztřídit)\n", CONST_ZASILKA_VYLOZENI);
    printf("- +-%d minut je rozptyl pro čas na nakládání nebo vykládání zásilek z dodávky nebo kamionu\n", CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL);
    printf("\n");
    printf("- 1 kamion = %d dodávek (objemově) (Zdroj: https://www.denik.cz/auto/svet-ridice-kamionu-neni-to-jednoducha-prace.html)\n", CONST_KAMION_DODAVEK);
    printf("- min %d minut trvá cesta kamionu na další depo\n", CONST_KAMION_CESTA_DEPO_MIN);
    printf("- max %d minut trvá cesta kamionu na další depo\n", CONST_KAMION_CESTA_DEPO_MAX);
    printf("- až %d dep může kamion navštívit na trase\n", CONST_KAMION_CESTA_DEPA_MAX);
    printf("\n");
    printf("- min %d minut trvá cesta na další výdejní místo\n", CONST_DODAVKA_CESTA_MIN);
    printf("- max %d minut trvá cesta na další výdejní místo\n", CONST_DODAVKA_CESTA_MAX);
    printf("- min %d minut trvá vynucený návrat na depo z důvodu konce směny v 16:00\n", CONST_DODAVKA_NAVRAT_MIN);
    printf("- max %d minut trvá vynucený návrat na depo z důvodu konce směny v 16:00\n", CONST_DODAVKA_NAVRAT_MAX);
    printf("- %d minut je čas na režii na výdejním místě (papírování, svačina, záchod, povinná 30 minutová přestávka na oběd)\n", CONST_DODAVKA_REZIE);
    printf("\n");
    printf("\n");
    printf("\n");



    printf("Odvozená data (zprůměrovaná a zaokrouhlená)\n");
    printf("-------------------------------------------\n");
    double VAR_KAMIONY_DEPO = (double)DAT_KAMIONY/DAT_DEPA;
    printf("- %.3f kamionů na jedno depo = %d kamionů / %d dep\n", VAR_KAMIONY_DEPO, DAT_KAMIONY, DAT_DEPA);
    double VAR_DODAVKY_DEPO = (double)(DAT_DODAVKY)/DAT_DEPA;
    printf("- %.3f dodávek na jedno depo = %d dodávek / %d dep\n", VAR_DODAVKY_DEPO, DAT_DODAVKY, DAT_DEPA);
    //printf("- %.3f výdejních míst na jedno depo = (%d výdejen + %d Z-Boxů) / %d dep\n", (double)(DAT_VYDEJNY+DAT_BOXY)/DAT_DEPA, DAT_VYDEJNY, DAT_BOXY, DAT_DEPA);
    double VAR_VYDEJNIMISTA_DODAVKA = (double)(DAT_VYDEJNY+DAT_BOXY)/DAT_DODAVKY;
    printf("- %.3f výdejních míst na jednu dodávku = (%d výdejen + %d boxů) / %d dodávek\n", VAR_VYDEJNIMISTA_DODAVKA, DAT_VYDEJNY, DAT_BOXY, DAT_DODAVKY);
    printf("- %.3f Kč obrat na zásilku = %ld Kč obrat / %d zásilek\n", (double)(DAT_OBRAT)/DAT_ZASILKY, DAT_OBRAT, DAT_ZASILKY);
    printf("\n");


    printf("Zásilky\n");
    double VAR_ZASILKY_DEN = (double)DAT_ZASILKY/CONST_PRACOVNI_DNY_ROK;
    printf("- %.3f rozvezených zásilek za den = %d zásilek za rok / 365 dní v roce\n", VAR_ZASILKY_DEN, DAT_ZASILKY);
    double VAR_ZASILKY_DODAVKA_DEN = (double)(VAR_ZASILKY_DEN)/(DAT_DODAVKY+CONST_KAMION_DODAVEK*DAT_KAMIONY);
    printf("- %.3f zásilek na dodávku na den = %.3f zásilek na den / (%d dodávek + %d dodávek na kamion * %d kamionů)\n", VAR_ZASILKY_DODAVKA_DEN, VAR_ZASILKY_DEN, DAT_DODAVKY, CONST_KAMION_DODAVEK, DAT_KAMIONY);
    double VAR_ZASILKY_KAMION_DEN = (double)(VAR_ZASILKY_DODAVKA_DEN)*CONST_KAMION_DODAVEK;
    printf("- %.3f zásilek na kamion na den = %d dodávek na kamion * %.3f zásilek na dodávku na den\n", VAR_ZASILKY_KAMION_DEN, CONST_KAMION_DODAVEK, VAR_ZASILKY_DODAVKA_DEN);
    double VAR_ZASILKY_VYDEJNIMISTO_DEN = (double)(VAR_ZASILKY_DODAVKA_DEN)/VAR_VYDEJNIMISTA_DODAVKA;
    printf("- %.3f zásilek na výdejní místo na den (vyložení nebo naložení) = %.3f zásilek na dodávku na den / %.3f výdejních míst na jednu dodávku\n", VAR_ZASILKY_VYDEJNIMISTO_DEN, VAR_ZASILKY_DODAVKA_DEN, VAR_VYDEJNIMISTA_DODAVKA);
    printf("\n");


    printf("Rozvoz/svoz na výdejní místa\n");
    double VAR_DODAVKA_SANCE_CESTA_BOX = (double)(DAT_BOXY)/DAT_DODAVKY*100;
    printf("- %.3f %% šance, že dodávka bude vykládat na boxu = %d boxů / %d dodávek * 100 %%\n", VAR_DODAVKA_SANCE_CESTA_BOX, DAT_BOXY, DAT_DODAVKY);
    double VAR_DODAVKA_SANCE_CESTA_BEZBOX = 100-VAR_DODAVKA_SANCE_CESTA_BOX;
    printf("- %.3f %% šance, že dodávka nebude vykládat na boxu = 100 %% - (%d boxů / %d dodávek * 100 %%)\n", VAR_DODAVKA_SANCE_CESTA_BEZBOX, DAT_BOXY, DAT_DODAVKY);
    
    double VAR_DODAVKA_VYDEJNA = VAR_ZASILKY_VYDEJNIMISTO_DEN * (CONST_ZASILKA_VYLOZENI + CONST_ZASILKA_NALOZENI) / 60 + CONST_DODAVKA_REZIE;
    printf("- %.3f minut stráví dodávka na výdejně = %.3f zásilek na výdejní místo * (%d sekund vyložení zásilky + %d sekund naložení zásilky) / 60 sekund + %d minut režie\n", VAR_DODAVKA_VYDEJNA, VAR_ZASILKY_VYDEJNIMISTO_DEN, CONST_ZASILKA_VYLOZENI, CONST_ZASILKA_NALOZENI, CONST_DODAVKA_REZIE);
    double VAR_DODAVKA_BOX = VAR_ZASILKY_VYDEJNIMISTO_DEN * (CONST_ZASILKA_VYLOZENI) / 60 + CONST_DODAVKA_REZIE;
    printf("- %.3f minut stráví dodávka na Z-Boxu = %.3f zásilek na výdejní místo * (%d sekund vyložení zásilky) / 60 sekund + %d minut režie\n", VAR_DODAVKA_BOX, VAR_ZASILKY_VYDEJNIMISTO_DEN, CONST_ZASILKA_VYLOZENI, CONST_DODAVKA_REZIE);
    printf("- %d - %d minut trvá cesta mezi výdejními místy\n", CONST_DODAVKA_CESTA_MIN, CONST_DODAVKA_CESTA_MAX);
    printf("- %d - %d trvá vynucený návrat na depo z důvodu konce směny v 16:00\n", CONST_DODAVKA_NAVRAT_MIN, CONST_DODAVKA_NAVRAT_MAX);
    printf("- %d - %d minut trvá dodávce trasa bez boxu\n", (int)round(VAR_VYDEJNIMISTA_DODAVKA*VAR_DODAVKA_VYDEJNA+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MIN), (int)round(VAR_VYDEJNIMISTA_DODAVKA*VAR_DODAVKA_VYDEJNA+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MAX));
    printf("- %d - %d minut trvá dodávce trasa s boxem\n", (int)round((VAR_VYDEJNIMISTA_DODAVKA-1)*VAR_DODAVKA_VYDEJNA+VAR_DODAVKA_BOX+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MIN), (int)round((VAR_VYDEJNIMISTA_DODAVKA-1)*VAR_DODAVKA_VYDEJNA+VAR_DODAVKA_BOX+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MAX));
    printf("\n");


    printf("Zaměstnanci\n");
    double VAR_ZAMESTNANCI_BEZRIDICU = (double)DAT_ZAMESTNANCI-DAT_DODAVKY-DAT_KAMIONY;
    printf("- %.3f zaměstnanců bez řidičů = %d zaměstnanců celkem - %d dodávek - %d kamionů\n", VAR_ZAMESTNANCI_BEZRIDICU, DAT_ZAMESTNANCI, DAT_DODAVKY, DAT_KAMIONY);
    double VAR_ZAMESTNANCI_DEPA = VAR_ZAMESTNANCI_BEZRIDICU*CONST_ZAMESTNANCI_DEPA_KOEFICIENT;
    printf("- %.3f zaměstnanců na depech ve skladech = %.3f zaměstnanců bez řidičů * %.3f koeficient zaměstnanců na depech ve skladech\n", VAR_ZAMESTNANCI_DEPA, VAR_ZAMESTNANCI_BEZRIDICU, CONST_ZAMESTNANCI_DEPA_KOEFICIENT);
    double VAR_ZAMESTNANCI_DEPO = VAR_ZAMESTNANCI_DEPA/DAT_DEPA; 
    printf("- %.3f zaměstnanců na jednom depu ve skladu = %.3f zaměstnanců na depech ve skladech / %d dep\n", VAR_ZAMESTNANCI_DEPO, VAR_ZAMESTNANCI_DEPA, DAT_DEPA);
    printf("\n");
    printf("\n");


    printf("Dodávka (jeden zaměstnanec)\n");
    double VAR_DODAVKA_DOBA_NALOZENI = VAR_ZASILKY_DODAVKA_DEN*CONST_ZASILKA_NALOZENI/60;
    printf("- %.3f (+-%d) minut trvá naložení jedné dodávky jedním zaměstnancem = %.3f zásilek na dodávku na den * %d sekund trvá naložení jedné zásilky jedním zaměstnancem / 60 sekund\n", VAR_DODAVKA_DOBA_NALOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, VAR_ZASILKY_DODAVKA_DEN, CONST_ZASILKA_NALOZENI);
    double VAR_DODAVKA_DOBA_VYLOZENI = VAR_ZASILKY_DODAVKA_DEN*CONST_ZASILKA_VYLOZENI/60;
    printf("- %.3f (+-%d) minut trvá vyložení jedné dodávky jedním zaměstnancem = %.3f zásilek na dodávku na den * %d sekund trvá vyložení jedné zásilky jedním zaměstnancem / 60 sekund\n", VAR_DODAVKA_DOBA_VYLOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, VAR_ZASILKY_DODAVKA_DEN, CONST_ZASILKA_VYLOZENI);
    printf("\n");

    printf("Dodávka/y (všichni zaměstnanci depa)\n");
    double VAR_ZAMESTNANCI_DODAVKA = VAR_ZAMESTNANCI_DEPO/VAR_DODAVKY_DEPO;
    printf("- %.3f zaměstnanců na jednu dodávku na depu = %.3f zaměstnanců na depu / %.3f dodávek na depu\n", VAR_ZAMESTNANCI_DODAVKA, VAR_ZAMESTNANCI_DEPO, VAR_DODAVKY_DEPO);
    
    double VAR_DODAVKY_DOBA_NALOZENI = VAR_DODAVKA_DOBA_NALOZENI/VAR_ZAMESTNANCI_DODAVKA;
    printf("- %.3f (+-%d) minut trvá naložení všech dodávek na depu = %.3f minut naložení dodávky / %.3f zaměstnanců na jednu dodávku na depu\n", VAR_DODAVKY_DOBA_NALOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, VAR_DODAVKA_DOBA_NALOZENI, VAR_ZAMESTNANCI_DODAVKA);
    double VAR_DODAVKY_DOBA_VYLOZENI = VAR_DODAVKA_DOBA_VYLOZENI/VAR_ZAMESTNANCI_DODAVKA;
    printf("- %.3f (+-%d) minut trvá vyložení všech dodávek na depu = %.3f minut vyložení dodávky / %.3f zaměstnanců na jednu dodávku na depu\n", VAR_DODAVKY_DOBA_VYLOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, VAR_DODAVKA_DOBA_VYLOZENI, VAR_ZAMESTNANCI_DODAVKA);
    printf("\n");
    printf("\n");


    printf("Kamion (jeden zaměstnanec)\n");
    double VAR_KAMION_DOBA_NALOZENI = CONST_KAMION_DODAVEK*VAR_ZASILKY_DODAVKA_DEN*CONST_ZASILKA_NALOZENI/60;
    printf("- %.3f (+-%d) minut trvá naložení jednoho kamionu jedním zaměstnancem = %d dodávek rovná se jeden kamion * %.3f zásilek na dodávku na den * %d sekund trvá naložení jedné zásilky jedním zaměstnancem / 60 sekund\n", VAR_KAMION_DOBA_NALOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, CONST_KAMION_DODAVEK, VAR_ZASILKY_DODAVKA_DEN, CONST_ZASILKA_NALOZENI);
    double VAR_KAMION_DOBA_VYLOZENI = CONST_KAMION_DODAVEK*VAR_ZASILKY_DODAVKA_DEN*CONST_ZASILKA_VYLOZENI/60;
    printf("- %.3f (+-%d) minut trvá vyložení jednoho kamionu jedním zaměstnancem = %d dodávek rovná se jeden kamion * %.3f zásilek na dodávku na den * %d sekund trvá vyložení jedné zásilky jedním zaměstnancem / 60 sekund\n", VAR_KAMION_DOBA_VYLOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, CONST_KAMION_DODAVEK, VAR_ZASILKY_DODAVKA_DEN, CONST_ZASILKA_VYLOZENI);
    printf("\n");

    printf("Kamion/y (všichni zaměstnanci depa)\n");
    double VAR_ZAMESTNANCI_KAMION = VAR_ZAMESTNANCI_DEPO/VAR_KAMIONY_DEPO;
    printf("- %.3f zaměstnanců na jeden kamion na depu = %.3f zaměstnanců na depu / %.3f kamionů na depu\n", VAR_ZAMESTNANCI_KAMION, VAR_ZAMESTNANCI_DEPO, VAR_KAMIONY_DEPO);
    
    double VAR_KAMIONY_DOBA_NALOZENI = VAR_KAMION_DOBA_NALOZENI/VAR_ZAMESTNANCI_KAMION;
    printf("- %.3f (+-%d) minut trvá naložení všech kamionů na depu = %.3f minut naložení kamionu / %.3f zaměstnanců na jeden kamion na depu\n", VAR_KAMIONY_DOBA_NALOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, VAR_KAMION_DOBA_NALOZENI, VAR_ZAMESTNANCI_KAMION);
    double VAR_KAMIONY_DOBA_VYLOZENI = VAR_KAMION_DOBA_VYLOZENI/VAR_ZAMESTNANCI_KAMION;
    printf("- %.3f (+-%d) minut  trvá vyložení všech kamionů na depu = %.3f minut vyložení kamionu / %.3f zaměstnanců na jeden kamion na depu\n", VAR_KAMIONY_DOBA_VYLOZENI, CONST_DEPO_NAKLADANI_VYKLADANI_ROZPTYL, VAR_KAMION_DOBA_VYLOZENI, VAR_ZAMESTNANCI_KAMION);
    printf("\n");
    printf("\n");


    printf("Přeprava mezi depy\n");
    int VAR_KAMION_PREPRAVA_DOBA_MAX = CONST_KAMION_CESTA_DEPO_MAX*(CONST_KAMION_CESTA_DEPA_MAX-1);
    printf("- %d - %d minut trvá cesta kamionu přes až %d dep (%d cesty)\n", CONST_KAMION_CESTA_DEPO_MIN, VAR_KAMION_PREPRAVA_DOBA_MAX, CONST_KAMION_CESTA_DEPA_MAX, CONST_KAMION_CESTA_DEPA_MAX-1);
    int VAR_KAMION_PREKLAD_DOBA_MAX = (int)round((CONST_KAMION_CESTA_DEPA_MAX-2)*(VAR_KAMIONY_DOBA_NALOZENI+VAR_KAMIONY_DOBA_VYLOZENI));
    printf("- 0 - %d minut trvá překlad kamionu na až %d depech (%d překlady)\n", VAR_KAMION_PREKLAD_DOBA_MAX, CONST_KAMION_CESTA_DEPA_MAX, CONST_KAMION_CESTA_DEPA_MAX-2);



    printf("\n\n\n");

    printf("Návrh zadání:\n");
    printf("\n");
    printf("Hypotetická přepravní společnost zásilek HYPSOZ\n");
    printf("-----------------------------------------------\n");
    printf("\n");
    printf("Logistická firma HYPSOZ sváží a rozváží kusové zásilky po celé České republice. Každý pracovní den začíná v 8:00 rozvozem a svozem zásilek dodávkami na a z výdejních míst. Dodávky jsou naplněny zásilkami k doručení již z předchozí pracovní směny. Firma s %d zaměstnanci operuje se %d kamiony pro převoz zásilek mezi %d depy a s %d dodávkami na svoz a rozvoz z dep na výdejní místa. Na každé depo připadá průměrně %d dodávek. Každá dodávka průměrně rozveze %d zásilek a má na starosti %d výdejen a H-boxů. To vychází %d zásilek na výdejní místo. Šance, že dodávka při své cestě navštíví i H-box je %.3f %% a %.3f %% je šance, že její trasa je pouze přes výdejny. Na pobočce dodávka stráví přibližně %d minut, kde je započítáno i %d minut na režii (do režie se např. počítá i povinná 30 minutová přestávka na oběd). U H-boxů dochází pouze k vyložení zásilek a dodávka zde stráví pouze %d minut i s režií. Cesta mezi výdejními místy trvá %d - %d minut. Trasa dodávky k výdejním místům tak trvá %d - %d minut nebo %d - %d minut pokud se na trase nachází H-box. Dodávky se musí vrátit do 16:00 zpět do dep, kde dochází k vyložení svezených zásilek a řidičům dodávek končí směna. Pokud není dodávka v 16:00 zpět ve svém depu, řidič zanechá všeho a ihned se vrací zpět do svého depa, kam mu cesta trvá %d - %d minut.\n",
    DAT_ZAMESTNANCI, DAT_KAMIONY, DAT_DEPA, DAT_DODAVKY, (int)round(VAR_DODAVKY_DEPO), (int)round(VAR_ZASILKY_DODAVKA_DEN), (int)round(VAR_VYDEJNIMISTA_DODAVKA), (int)round(VAR_ZASILKY_VYDEJNIMISTO_DEN), VAR_DODAVKA_SANCE_CESTA_BOX, VAR_DODAVKA_SANCE_CESTA_BEZBOX, (int)round(VAR_DODAVKA_VYDEJNA), (int)round(CONST_DODAVKA_REZIE), (int)round(VAR_DODAVKA_BOX), CONST_DODAVKA_CESTA_MIN, CONST_DODAVKA_CESTA_MAX, (int)round(VAR_VYDEJNIMISTA_DODAVKA*VAR_DODAVKA_VYDEJNA+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MIN), (int)round(VAR_VYDEJNIMISTA_DODAVKA*VAR_DODAVKA_VYDEJNA+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MAX), (int)round((VAR_VYDEJNIMISTA_DODAVKA-1)*VAR_DODAVKA_VYDEJNA+VAR_DODAVKA_BOX+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MIN), (int)round((VAR_VYDEJNIMISTA_DODAVKA-1)*VAR_DODAVKA_VYDEJNA+VAR_DODAVKA_BOX+VAR_VYDEJNIMISTA_DODAVKA*CONST_DODAVKA_CESTA_MAX), CONST_DODAVKA_NAVRAT_MIN, CONST_DODAVKA_NAVRAT_MAX);
    printf("\n");
    printf("Na depo připadá %d zaměstnanců, kteří po příjezdu všech dodávek z nich začínají vykládat a třídit zásilky. Vyložení jedné zásilky jedním zaměstnancem trvá %d sekund. Po vyložení všech dodávek začnou zaměstnanci nakládat roztříděné zásilky do kamionů. Naložení jedné zásilky jedním zaměstnancem trvá %d sekund. Na jedno depo připadá %d kamionů a jeden uveze %d zásilek.\n",
    (int)round(VAR_ZAMESTNANCI_DEPO), CONST_ZASILKA_VYLOZENI, CONST_ZASILKA_NALOZENI, (int)round(VAR_KAMIONY_DEPO), (int)round(VAR_ZASILKY_KAMION_DEN));
    printf("\n");
    printf("Naložené kamiony na své trasu mohou jet až přes %d dep, kde složí nebo naloží část zásilek. Tato cesta může trvat %d - %d minut. Překlad na depech na trase může trvat 0 - %d minut.\n",
    CONST_KAMION_CESTA_DEPA_MAX, CONST_KAMION_CESTA_DEPO_MIN, VAR_KAMION_PREPRAVA_DOBA_MAX, VAR_KAMION_PREKLAD_DOBA_MAX);
    printf("\n");
    printf("V posledním depu dojde k vyložení kamionu, které trvá %d (+- 10) minut. Poté dochází k nakládání dodávek pro další pracovní den, což trvá %d (+- 10) minut. Vše se musí stihnout do začátku další pracovní směny v 8:00 hodin.\n",
    (int)round(VAR_KAMIONY_DOBA_VYLOZENI), (int)round(VAR_DODAVKA_DOBA_NALOZENI));
    printf("\n");
    printf("Během směny by nikdy nemělo dojít k porušení zákonem daných přestávek a maximální doby řízení.\n");

    printf("\n\n\n");
    return 0;
}
