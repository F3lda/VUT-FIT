/**
Projekt:    Kostra 9. cviceni IZP 2015
Autor:      Marek Zak <izakmarek@fit.vutbr.cz>
Datum:      28. 11. 2015
*/

#include "struct.h"

/**
 * Inizializace objektu. Název objektu kopíruje. Objekt bude mít název
 * roven NULL, pokud se inicializace nezdařila.
 */
void object_ctor(Object *o, int id, char *name)
{
	if(o == NULL) return;
	if(name == NULL){
		o->name = NULL;
		o->id = -1;
		return;
	}
	
	o->name = malloc((strlen(name)+1) * sizeof(char));
	if(o->name != NULL){
		memcpy(o->name, name, strlen(name)+1);
		o->id = id;
	}
}

/**
 * Záměna dat dvou objektù.
 */
void object_swap(Object *i1, Object *i2)
{
	Object temp;
	temp = *i1;
	*i1 = *i2;
	*i2 = temp;
}

/**
 * Hluboká kopie objektu src na dst. Vrací dst, pokud se operace povedla,
 * jinak NULL.
 */
Object *object_cpy(Object *dst, Object *src)
{
	object_ctor(dst, src->id, src->name);
	if(dst->name == NULL){
		return NULL;
	}
	return dst;
}

/**
 * Uvolní objekt (resp. jeho jméno) z paměti. 
 * Nastavi jmeno objektu na NULL.
 */
void object_dtor(Object *o)
{
	if(o != NULL){
	    o->id = -1;
		free(o->name);
		o->name = NULL;
	}
}

/**
 * Vytiske hodnoty daneho objektu.
 */
void print_object(Object *o)
{
    printf("ID: %d, NAME: %s\n", o->id, o->name);
}
