/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: common.c
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#include "common.h"



int handleError(ErrorHandle errorHandle, char *lastOKTokenType, int lastOKTokenPosLine, int lastOKTokenPosNumber, char *lastOKTokenString)
{
	if(errorHandle.errorID == ALL_OK){
		fprintf(FILE_ERROR, "--------------------------------------------------------------------------------\n");
		fprintf(FILE_ERROR, "Exit code: %d\n\n\n", errorHandle.errorID);
		return ALL_OK;
	}
	
	if(errorHandle.errorID == LEX_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Lexical Error!\n");
	} else if(errorHandle.errorID == SYNTAX_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Syntax Error!\n");
	} else if(errorHandle.errorID == SEM_UNDEFINED_VAR_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - undefined variable!\n");
	} else if(errorHandle.errorID == SEM_DATA_TYPE_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - wrong data type!\n");
	} else if(errorHandle.errorID == SEM_TYPE_COMPATIBILITY_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - type compatibility!\n");
	} else if(errorHandle.errorID == SEM_FUNC_PARAM_RETURN_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - wrong return params!\n");
	} else if(errorHandle.errorID == SEM_OTHER_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Semantic Error - other!\n");
	} else if(errorHandle.errorID == ZERO_DIVISION_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Zero Division!\n");
	} else if(errorHandle.errorID == INTERNAL_ERROR){
		fprintf(FILE_ERROR, "FATAL ERROR: Internal Error!\n");
	}
	fprintf(FILE_ERROR, "ERROR - %s\n", errorHandle.errorString);
	fprintf(FILE_ERROR, "ERROR POSITION - file: %s; line: %d\n", errorHandle.errorPosFile, errorHandle.errorPosLine);
	fprintf(FILE_ERROR, "LAST OK TOKEN - \"%s\"; type: %s; token line: %d; token number: %d\n", lastOKTokenString, lastOKTokenType, lastOKTokenPosLine, lastOKTokenPosNumber);
	fprintf(FILE_ERROR, "--------------------------------------------------------------------------------\n");
	fprintf(FILE_ERROR, "Exit code: %d\n\n\n", errorHandle.errorID);

	return errorHandle.errorID;
}

void handleFreeError(int errorID, int errorLine, char *errorFile)
{// pouze pro debug - použití u funkcí, které už neovlivňují výsledek celého programu (funkce uvolňující paměť nebo vypisující data)
	if(errorID != ALL_OK){
		//fprintf(FILE_ERROR, "FATAL ERROR: memory free - <%d>!\nFile: %s; Line: %d;\n", errorID, errorFile, errorLine);
	}
}

int errorHandleInit(ErrorHandle *errorHandle)
{
	if(errorHandle != NULL){
		errorHandle->errorID = ALL_OK;
		memset(errorHandle->errorString, '\0', sizeof(char)*STATIC_STRING_LENGHT);
		memset(errorHandle->errorPosFile, '\0', sizeof(char)*STATIC_STRING_LENGHT);
		errorHandle->errorPosLine = 0;
		return 0;
	}
	return 1;
}

int errorExists(ErrorHandle errorHandle)
{
	return (errorHandle.errorID != ALL_OK);
}

int errorSet(int errorID, char *errorString, const char *errorPosFile, int errorPosLine, ErrorHandle *errorHandle)
{// soubor a řádek by měl být (pro lepší info) přijímán z místa použití funkce a ne z vnitřku funkce - příklad: scannerTokenListInit(&tokenList, &errorHandle, __FILE__, __LINE__);
	if(errorHandle != NULL){
		errorHandle->errorID = errorID;
		snprintf(errorHandle->errorString, STATIC_STRING_LENGHT, "%s", errorString); // snprintf(NULL, 0, "%d", x); -> returns max length of number value
		snprintf(errorHandle->errorPosFile, STATIC_STRING_LENGHT, "%s", errorPosFile); // saves (STATIC_STRING_LENGHT-1) chars and last (STATIC_STRING_LENGHT) char sets to '\0'
		errorHandle->errorPosLine = errorPosLine;
		return 0;
	}
	return 1;
}
