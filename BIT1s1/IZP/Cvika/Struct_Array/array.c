/**
Projekt:    Kostra 9. cviceni IZP 2015
Autor:      Marek Zak <izakmarek@fit.vutbr.cz>
Datum:      28. 11. 2015
*/

#include "array.h"
#include <string.h>

/**
 * Konstruktor pole. Vytvoří pole o velikosti size a každý prvek
 * v něm inicializuje na hodnoty 0, NULL.
 */
Array array_ctor(unsigned size)
{
	Array object;
	object.size = 0;
	object.items = NULL;
	
	if(size > 0){
		object.items = malloc(size*sizeof(Object));
		if(object.items == NULL) return object;
		object.size = size;
		unsigned int i;
		for(i = 0; i < size; i++){
			object_ctor(&object.items[i],-1,NULL);
		}
	}
	
	return object;
}

/**
 * Uvolní alokované místo pro pole (a každý jeho objekt) z paměti.
 */
void array_dtor(Array *arr)
{
	if(arr == NULL) return;
	
	if(arr->size > 0){
		unsigned int i;
		for(i = 0; i < arr->size; i++){
			object_dtor(&arr->items[i]);
		}
		
		arr->size = 0;
		free(arr->items);
		arr->items = NULL;
	}
}

/**
 * Změna velikosti pole. Změní/realokuje zadané pole na novou velikost.
 * V případě zvětšení ještě inicializuje každý prvek na hodnoty 0, NULL
 */
Array array_resize(Array *arr, unsigned size)
{
	if(arr == NULL) return *arr;

	if(size > arr->size){
		arr->items = realloc(arr->items,size*sizeof(Object));
		if(arr->items == NULL) return *arr;
		unsigned int i;
		for(i = arr->size; i < size; i++){
			object_ctor(&arr->items[i],0,NULL);
		}
		arr->size = size;
	} else if(size < arr->size){
		unsigned int i;
		for(i = arr->size; i > size; i--){
			object_dtor(&arr->items[i-1]);
		}
		arr->items = realloc(arr->items,size*sizeof(Object));
		if(arr->items == NULL) return *arr;
		arr->size = size;
	}
	
	return *arr;
}

/**
 * Hledání prvku v poli podle identifikátoru prvku. Vrací index prvku v poli
 * nebo -1, pokud prvek pole neobsahuje.
 */
int array_find_id(Array *arr, int id)
{
	if(arr == NULL) return -1;
	
	unsigned int i;
	for(i = 0; i < arr->size; i++){
		if(arr->items[i].id == id){
			return i;
		}
	}
	
	return -1;
}

/**
 * Hledání prvku v poli podle názvu. Vrací index prvku v poli
 * nebo -1, pokud prvek pole neobsahuje.
 */
int array_find_name(Array *arr, char *name)
{
	if(arr == NULL || name == NULL) return -1;
	
	unsigned int i;
	for(i = 0; i < arr->size; i++){
		if(arr->items[i].name != NULL){
			if(strcmp(arr->items[i].name, name) == 0){
				return i;
			}
		}
	}
	
	return -1;
}

/**
 * Vložení prvku do pole na zadaný index. Vrací index vloženého prvku (idx)
 * nebo -1, pokud se operace nezdarila.
 */
int array_insert_item(Array *arr, Object *item, unsigned idx)
{
	if(arr == NULL || item == NULL) return -1;
	
	if(idx < arr->size+1 && array_resize(arr, arr->size+1).items != NULL){
		unsigned int i;
		for(i = arr->size-1; i > idx; i--){
			arr->items[i] = arr->items[i-1];
		}
		arr->items[idx] = *item;
		return idx;
	}
	
	return -1;
}

/**
 * Hledání prvku s nejmenším identifikátorem. Vrací index prvku nebo -1,
 * pokud je pole prázdné.
 * Promenna l urcuje index, od ktereho se bude pole prohledavat.
 */
int array_find_min(Array *arr, unsigned l)
{
	if(arr == NULL || arr->size < l+1) return -1;
	
	unsigned int i;
	int minID = l;
	for(i = minID+1; i < arr->size; i++){
		if(arr->items[i].id < arr->items[minID].id){
			minID = i;
		}
	}
	
	return minID;
}

/**
 * Øazení prvkù v poli podle jejich identifikátorù.
 */
void array_sort(Array *arr)
{
	if(arr == NULL) return;
	
	unsigned int i;
	for(i = 0; i < arr->size; i++){
		int minID = array_find_min(arr,i);
		Object temp = arr->items[minID];
		
		unsigned int ii;
		for(ii = minID; ii > i; ii--){
			arr->items[ii] = arr->items[ii-1];
		}
		
		arr->items[i] = temp;
	}
}

/**
 * Vypise prvky pole.
 */
void print_array(Array a)
{
    for (unsigned int i = 0; i < a.size; i++)
    {
        printf("#%d\t", i);
        print_object(&a.items[i]);
    }
    printf("\n");
}

/**
 * Vypise prvky pole dane velikosti.
 */
void print_array_size(Array a, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        printf("#%d\t", i);
        print_object(&a.items[i]);
    }
    printf("\n");
}


