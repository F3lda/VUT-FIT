//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     JMENO PRIJMENI <xlogin00@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author JMENO PRIJMENI
 *
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
    pHead = NULL;
}

PriorityQueue::~PriorityQueue()
{
    if(pHead != NULL){
		Element_t *item = pHead, *item_temp;
        while(item != NULL){
            item_temp = item->pNext;
            free(item);
            item = item_temp;
        }
        pHead = NULL;
	}
}

void PriorityQueue::Insert(int value)
{
    Element_t *newItem = (Element_t*)malloc(sizeof(Element_t));
	if(newItem != NULL){
        newItem->value = value;
        Element_t *prevItem = FindLastSmallerValue(value);
        if(prevItem == NULL){
            newItem->pPrev = NULL;
            newItem->pNext = pHead;

            if(pHead != NULL){pHead->pPrev = newItem;} // If Queue is EMPTY
            pHead = newItem;
        } else {
            newItem->pPrev = prevItem;
            newItem->pNext = prevItem->pNext;

            if(prevItem->pNext != NULL){prevItem->pNext->pPrev = newItem;} // If it's a last item
            prevItem->pNext = newItem;
        }
	}
}

bool PriorityQueue::Remove(int value)
{
    Element_t *foundItem = Find(value);
    if(foundItem != NULL){
        if(foundItem == pHead){
            pHead = foundItem->pNext;
	    if(pHead != NULL) pHead->pPrev = NULL;
	    free(foundItem);
        } else {
	    foundItem->pPrev->pNext = foundItem->pNext;
	    if(foundItem->pNext != NULL) foundItem->pNext->pPrev = foundItem->pPrev;
	    free(foundItem);
	}
        return true;
    }
    return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    if(pHead != NULL){
        Element_t *prevItem = FindLastSmallerValue(value);
        if(prevItem == NULL){
            if(pHead->value == value){
                return pHead;
            }
        } else if(prevItem->pNext != NULL && prevItem->pNext->value == value){
            return prevItem->pNext;
        }
    }
    return NULL;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return pHead;
}

/*** Konec souboru tdd_code.cpp ***/
