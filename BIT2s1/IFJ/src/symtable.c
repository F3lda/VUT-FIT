/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: symtable.c
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */

#include "symtable.h"



/****************************************************** SYMTABLE TREE ******************************************************************************/

void symTableInit(SymTableBinTreePtr *RootPtr)
{
	(*RootPtr) = NULL;
}

SymTableData symTableInitData(IDtype idType, bool idDefined, IDdataType idDataType, SymTableParamList functionParamDataTypes, SymTableParamList functionReturnDataTypes, SymTableBinTreePtr functionLocalSymTable)
{
	SymTableData data;
	
	data.idType = idType;
	data.idDefined = idDefined;
	data.idDataType = idDataType;
	data.functionParamDataTypes = functionParamDataTypes;
	data.functionReturnDataTypes = functionReturnDataTypes;
	data.functionLocalSymTable = functionLocalSymTable;
	
	return data;
}

SymTableData symTableInitDataInLine(IDtype idType, bool idDefined, IDdataType idDataType, int paramCount, IDdataType paramDataTypes[], int returnCount, IDdataType returnDataTypes[], SymTableBinTreePtr functionLocalSymTable, ErrorHandle *errorHandle)
{
	SymTableData data;
	symTableParamListInit(&(data.functionParamDataTypes), errorHandle);
	symTableParamListInit(&(data.functionReturnDataTypes), errorHandle);
	
	data.idType = idType;
	data.idDefined = idDefined;
	data.idDataType = idDataType;
	
	int i;
	for(i = 0; i < paramCount; i++){
		symTableParamListAdd(&(data.functionParamDataTypes), paramDataTypes[i], errorHandle);
	}
	if(paramCount == -1){ // libovolný počet parametrů 
		data.functionParamDataTypes.size = -1;
	}
	for(i = 0; i < returnCount; i++){
		symTableParamListAdd(&(data.functionReturnDataTypes), returnDataTypes[i], errorHandle);
	}
	
	data.functionLocalSymTable = functionLocalSymTable;
	
	return data;
}

int symTableInsert(SymTableBinTreePtr *RootPtr, char *key, SymTableData data, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	SymTableBinTreePtr *ROOT = RootPtr;
	while((*ROOT) != NULL){
		if(strcmp((*ROOT)->key, key) == 0){ // existující klíč - přepsat data (nebo chyba?)
			(*ROOT)->data = data;
			return errorHandle->errorID;
		} else if(strcmp(key, (*ROOT)->key) < 0){
			ROOT = &((*ROOT)->leftPtr);
		} else {
			ROOT = &((*ROOT)->rightPtr);
		}
	}
	
	(*ROOT) = (SymTableBinTreePtr)malloc(sizeof(struct SymTableBinTreeNode));
	if((*ROOT) != NULL){
		(*ROOT)->key = (char*)malloc((sizeof(char)*strlen(key))+1);
		(*ROOT)->data = data;
		(*ROOT)->leftPtr = NULL;
		(*ROOT)->rightPtr = NULL;
		if((*ROOT)->key != NULL){
			strcpy((*ROOT)->key, key);
		} else {
			free((*ROOT));
			(*ROOT) = NULL;
			errorSet(INTERNAL_ERROR, "symTableInsert: malloc struct SymTableBinTreeNode NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "symTableInsert: malloc char NULL", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}

int symTableSearch(SymTableBinTreePtr RootPtr, char *key, SymTableData *data, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(RootPtr != NULL){
		if(strcmp(RootPtr->key, key) == 0){
			if(data != NULL){
				(*data) = RootPtr->data;
			} else {
				//errorSet(INTERNAL_ERROR, "symTableSearch: data NULL", __FILE__, __LINE__, errorHandle);
			}
			return 1;
		} else if(strcmp(key, RootPtr->key) < 0){
			return symTableSearch(RootPtr->leftPtr, key, data, errorHandle);
		} else {
			return symTableSearch(RootPtr->rightPtr, key, data, errorHandle);
		}
	}
	return 0;
}

int symTableDispose(SymTableBinTreePtr *RootPtr)
{	
	SymTableBinTreePtr root = (*RootPtr);
	if(root != NULL){
		symTableDispose(&root->leftPtr);
		symTableDispose(&root->rightPtr);
		free(root->key);
		symTableParamListFree(&(root->data.functionParamDataTypes));
		symTableParamListFree(&(root->data.functionReturnDataTypes));
		symTableDispose(&(root->data.functionLocalSymTable));
		free(root);
		(*RootPtr) = NULL;
		return ALL_OK;
	}
	return INTERNAL_ERROR;
}


/****************************************************** PARAMS STACK ******************************************************************************/

int symTableParamListInit(SymTableParamList *paramList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(paramList != NULL){
		paramList->size = 0;
		paramList->first = NULL;
		paramList->active = NULL;
		paramList->last = NULL;
	} else {
		errorSet(INTERNAL_ERROR, "symTableParamListInit: SymTableParamList NULL", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}

int symTableParamListAdd(SymTableParamList *paramList, IDdataType dataType, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(paramList != NULL){
		SymTableParamListElementPtr added = (SymTableParamListElementPtr)malloc(sizeof(struct SymTableParamListElement));
		if(added != NULL){
			added->dataType = dataType;
			
			added->leftPtr = paramList->last;
			added->rightPtr = NULL;
			
			if(paramList->last != NULL){
				paramList->last->rightPtr = added;
			} else {
				paramList->first = added;
			}
			paramList->last = added;
			
			(paramList->size)++;
		} else {
			errorSet(INTERNAL_ERROR, "symTableParamListAdd: malloc struct SymTableParamListElement NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "symTableParamListAdd: SymTableParamList NULL", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}

int symTableParamListMoveNext(SymTableParamList *paramList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(paramList != NULL){
		if(paramList->active != NULL){
			if(paramList->active != paramList->last){
				paramList->active = paramList->active->rightPtr;
			} else {
				errorSet(INTERNAL_ERROR, "symTableParamListMoveNext: Active Last", __FILE__, __LINE__, errorHandle);
			}
		} else {
			errorSet(INTERNAL_ERROR, "symTableParamListMoveNext: Active NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "symTableParamListMoveNext: SymTableParamList NULL", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}

int symTableParamListGetActive(SymTableParamList *paramList, IDdataType *dataType, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(paramList != NULL){
		if(paramList->active != NULL){
			(*dataType) = paramList->active->dataType;
		} else {
			errorSet(INTERNAL_ERROR, "symTableParamListGetActive: Active NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "symTableParamListGetActive: SymTableParamList NULL", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}

int symTableParamListSetActiveFirst(SymTableParamList *paramList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(paramList != NULL){
		if(paramList->first != NULL){
			paramList->active = paramList->first;
		} else {
			errorSet(INTERNAL_ERROR, "symTableParamListSetActiveFirst: First NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "symTableParamListSetActiveFirst: SymTableParamList NULL", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}

int symTableParamListGetSize(SymTableParamList *paramList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	if(paramList != NULL){
		return paramList->size;
	}
	
	errorSet(INTERNAL_ERROR, "symTableParamListGetSize: SymTableParamList NULL", __FILE__, __LINE__, errorHandle);
	
	return -2;
}

int symTableParamListFree(SymTableParamList *paramList)
{
	if(paramList != NULL){
		SymTableParamListElementPtr first = paramList->first;
		SymTableParamListElementPtr destroy;
		while(first != NULL){
			destroy = first;
			first = first->rightPtr;
			free(destroy);
		}
		paramList->size = 0;
		paramList->first = NULL;
		paramList->active = NULL;
		paramList->last = NULL;
		return ALL_OK;
	}
	return INTERNAL_ERROR;
}


/****************************************************** TEST PRINT TREE ******************************************************************************/

/* Name: Print_tree
*  Source: Algoritmy (IAL) - FIT (Fakulta Informacnich Technologii) - file: c401-test.c
*  Date: 2020-11-10
*  Authors: Martin Tucek - 2005-10-01; (Roman Lukas - 2006), (Karel Masarik - 2013)
*  Edited: true
*/
void Print_tree2(SymTableBinTreePtr TempTree, char* sufix, char fromdir, char* space_size)
{
	if (TempTree != NULL)
	{
		char* suf2 = (char*) malloc(strlen(sufix) + 4);
		strcpy(suf2, sufix);
		suf2 = strcat(suf2, space_size);
		if (fromdir == 'L') {
			suf2 = strcat(suf2, "  |");
			printf("%s\n", suf2);
		} else {
			suf2 = strcat(suf2, "   ");
		}
		Print_tree2(TempTree->rightPtr, suf2, 'R', space_size);
		printf("%s%s  +-[%s][%d,%d]\n", sufix, space_size, TempTree->key, TempTree->data.functionParamDataTypes.size, TempTree->data.functionReturnDataTypes.size);
		strcpy(suf2, sufix);
		suf2 = strcat(suf2, space_size);
		if (fromdir == 'R') {
			suf2 = strcat(suf2, "  |");
		} else {
			suf2 = strcat(suf2, "   ");
		}
		Print_tree2(TempTree->leftPtr, suf2, 'L', space_size);
		if (fromdir == 'R') {
			printf("%s\n", suf2);
		}
		free(suf2);
	}
}

void Print_tree(SymTableBinTreePtr TempTree)
{
	printf("Struktura binarniho stromu:\n");
	printf("\n");
	if (TempTree != NULL) {
		Print_tree2(TempTree, "", 'X', "   ");
	} else {
		printf("strom je prazdny\n");
	}
	printf("\n");
	printf("=================================================\n");
}
/* ******* Print_tree - end ******* */
