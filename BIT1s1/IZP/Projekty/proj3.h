/*! 
 *  \file      proj3.h
 *  \author    Karel Jirgl
 *  \version   1.0
 *  \date      2019-12-18
 *  \brief     Program pro nalezení cesty ven z bludiště (mapy) z daných souřadnic
 */

#ifndef PROJ3_H
#define PROJ3_H

/*! Makro pro zjištění orientace daného políčka (trojúhelníku) */
#define IS_TRIANGLE_NORMAL(r,c) (r % 2 != c % 2)
/*! Makro pro získání daného bitu daného čísla (číslo <= 7) */
#define GET_BIT(n,b) ((n >> b) & 1)

/*! Typy hran trojúhelníku */
enum triangle_sides{LEFT, RIGHT, TOP_BOTTOM};
/*! Způsoby nalezení cesty podle dané ruky (způsob orientace v prostoru) */
enum hand_rules{LEFT_HAND, RIGHT_HAND};

/*! Struktura pro ukládání map */
typedef struct {
    int rows;				/*! Počet řádků mapy */
    int cols;				/*! Počet sloupců mapy */
    unsigned char **cells;	/*! Jednotlivé buňky mapy */
} Map;


/*! 
 * @brief Načtení čísla typu double z řetězce znaků
 * 
 * @param[in] *str Pointer na prohledávaný řetězec znaků
 * @param[out] *num Pointer typu double pro uložení nalezeného čísla
 * 
 * @return 1 při úspěchu, 0 při chybě
 */
int sscand(const char *str, double *num)

/*! 
 * @brief Načtení čísla typu int z řetězce znaků
 * 
 * @param[in] *str Pointer na prohledávaný řetězec znaků
 * @param[out] *num Pointer typu int pro uložení nalezeného čísla
 * 
 * @return 1 při úspěchu, 0 při chybě
 */
int sscani(const char *str, int *num)

/*! 
 * @brief Vytištění nápovědy do STDIN
 */
void help()

/*! 
 * @brief Načtení mapy ze souboru
 * 
 * @param[in] *file_name Pointer na řetězec znaků obsahující název souboru, ze kterého se bude mapa načítat
 * @param[out] *map Pointer na struktury typu Map, do které se uloží načtená mapa
 * 
 * @pre Pokud existuje daný soubor
 * @post Povede-li se alokovat paměť pro políčka mapy
 * 
 * @return 0 při úspěchu, jinak kladné přirozené číslo
 */
int load_map(char *file_name, Map *map)

/*! 
 * @brief Otestování validnosti mapy (kontrola zda sedí velikost mapy a sousední typy hran políček [trojúhelníků])
 * 
 * @param[in] *map Pointer na strukturu typu Map, ve které je uložena mapa bludiště
 * 
 * @pre Struktura Map nesmí být prázdná
 * 
 * @return 0 pokud je mapa validní, jinak 1
 */
bool test_map(Map *map)

/*! 
 * @brief Uvolnění paměti, ve které jsou uložena políčka mapy, a vynulování struktury
 * 
 * @param[in] *map Pointer na strukturu typu Map, ve které je uložena mapa bludiště
 * 
 * @pre Pokud struktura Map není prázdná
 */
void free_map(Map *map)

/*! 
 * @brief Zjistí, zda se na dáné hraně políčka nachází zeď nebo ne
 * 
 * @param[in] *map Pointer na strukturu typu Map, ve které je uložena mapa bludiště
 * @param[in] r Řádek na dané mapě
 * @param[in] c Sloupec na dané mapě
 * @param[in] border Typ hrany políčka (trojúhelníku)
 * 
 * @pre Pokud struktura Map není prázdná a souřadnice se nacházejí uvnitř mapy
 * 
 * @return 1 pokud daná hrana zeď, 0 pokud je hrana průchodná, jinak špatná vstupní data
 */
int isBorder(Map *map, int r, int c, int border)

/*! 
 * @brief Z daných souřadnic a způsobu orientace v prostoru určí výchozí typ hrany políčka (trojúhelníku)
 * 
 * @param[in] *map Pointer na strukturu typu Map, ve které je uložena mapa bludiště
 * @param[in] r Řádek na dané mapě
 * @param[in] c Sloupec na dané mapě
 * @param[in] leftright Způsob orientace v prostoru
 * 
 * @pre Pokud struktura Map není prázdná a souřadnice se nacházejí uvnitř mapy
 * 
 * @return Vrací typ hrany políčka (vstupní hranu do trojúhelníka), 4 při chybě
 */
int start_border(Map *map, int r, int c, int leftright)

/*! 
 * @brief Udělá jeden krok na mapě
 * 
 * @param[in] *map Pointer na strukturu typu Map, ve které je uložena mapa bludiště
 * @param[in/out] *start_pos_row Pointer na int s číslem aktuálního řádku na mapě
 * @param[in/out] *start_pos_col Pointer na int s číslem aktuálního sloupce na mapě
 * @param[in/out] *start_side Pointer na int s typem auktální hrany políčka
 * @param[in] leftright Způsob orientace v prostoru
 * 
 * @pre Pokud struktura Map není prázdná a souřadnice se nacházejí uvnitř mapy
 * 
 * @return 0 při úspěchu, 1 při chybě
 */
bool map_step(Map *map, int *start_pos_row, int *start_pos_col, int *start_side, int leftright)

#endif
