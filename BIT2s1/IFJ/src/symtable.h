/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: symtable.h
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include "common.h"




typedef enum {
	VAR,	// proměnná
	FUNC	// funkce
} IDtype;

// Datový typ identifikátoru
typedef enum {
	INT,
	FLOAT,
	STRING,
	BOOL,
	NIL
} IDdataType;

// Seznam pro vstupní a výstupní prametry
typedef struct SymTableParamListElement {
	IDdataType dataType;
	struct SymTableParamListElement *leftPtr;
	struct SymTableParamListElement *rightPtr;
} *SymTableParamListElementPtr;

typedef struct {
	int size;
	SymTableParamListElementPtr active;
	SymTableParamListElementPtr first;
	SymTableParamListElementPtr last;
} SymTableParamList;

// Data v uzlu
typedef struct SymTableDataStruct{
	IDtype idType;										// typ identifikátoru
	bool idDefined;										// je identifikátor definovaný
	IDdataType idDataType;								// datový typ [proměnné]
	SymTableParamList functionParamDataTypes;			// seznam parametrů [funkce]
	SymTableParamList functionReturnDataTypes;			// seznam návratových parametrů [funkce]
	struct SymTableBinTreeNode *functionLocalSymTable;	// ukazatel na lokální tabulku symbolů [funkce]
} SymTableData;

// Uzel stromu
typedef struct SymTableBinTreeNode{
	char *key;								// identifikátor
	SymTableData data;						// data uzlu
	struct SymTableBinTreeNode *leftPtr;	// levá větev stromu (menší klíč)
	struct SymTableBinTreeNode *rightPtr;	// pravá větev stromu (větší klíč)
} *SymTableBinTreePtr;



/****************************************************** SYMTABLE TREE ******************************************************************************/

void symTableInit(SymTableBinTreePtr *RootPtr);

SymTableData symTableInitData(IDtype idType, bool idDefined, IDdataType idDataType, SymTableParamList functionParamDataTypes, SymTableParamList functionReturnDataTypes, SymTableBinTreePtr functionLocalSymTable);

SymTableData symTableInitDataInLine(IDtype idType, bool idDefined, IDdataType idDataType, int paramCount, IDdataType paramDataTypes[], int returnCount, IDdataType returnDataTypes[], SymTableBinTreePtr functionLocalSymTable, ErrorHandle *errorHandle);

int symTableInsert(SymTableBinTreePtr *RootPtr, char *key, SymTableData data, ErrorHandle *errorHandle);

int symTableSearch(SymTableBinTreePtr RootPtr, char *key, SymTableData *data, ErrorHandle *errorHandle);

int symTableDispose(SymTableBinTreePtr *RootPtr);


/****************************************************** PARAMS STACK ******************************************************************************/

int symTableParamListInit(SymTableParamList *paramList, ErrorHandle *errorHandle);

int symTableParamListAdd(SymTableParamList *paramList, IDdataType dataType, ErrorHandle *errorHandle);

int symTableParamListMoveNext(SymTableParamList *paramList, ErrorHandle *errorHandle);

int symTableParamListGetActive(SymTableParamList *paramList, IDdataType *dataType, ErrorHandle *errorHandle);

int symTableParamListSetActiveFirst(SymTableParamList *paramList, ErrorHandle *errorHandle);

int symTableParamListGetSize(SymTableParamList *paramList, ErrorHandle *errorHandle);

int symTableParamListFree(SymTableParamList *paramList);


/****************************************************** TEST PRINT TREE ******************************************************************************/

void Print_tree2(SymTableBinTreePtr TempTree, char* sufix, char fromdir, char* space_size);

void Print_tree(SymTableBinTreePtr TempTree);


#endif //defined _SYMTABLE_H
