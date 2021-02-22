/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: main.c
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */

#include "common.h"
#include "scanner.h"
#include "parser.h"
#include "generator.h"

int getResult(ErrorHandle errorHandle, TokenList *tokenList);



int main()
{
	FILE_INPUT	= stdin;
	FILE_OUTPUT	= stdout;
	FILE_ERROR	= stderr;
	
	ErrorHandle errorHandle;
	errorHandleInit(&errorHandle);
	
	
	TokenList tokenList;
	scannerTokenListInit(&tokenList, &errorHandle);
	
	
	// získání seznamu s tokeny
	scannerGetTokenList(&tokenList, &errorHandle);
	
	
	// Syntaktická analýza + Sémantická analýza
	parserAnalyze(&tokenList, &errorHandle);
	
	
	// generování výsledného kódu -> přímé volání z parseru
	// uvolnění generátoru
	generatorFree();
	
	
	int result = getResult(errorHandle, &tokenList);
	handleFreeError(scannerTokenListFree(&tokenList), __LINE__, __FILE__);
	return result;
}


int getResult(ErrorHandle errorHandle, TokenList *tokenList)
{
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	ErrorHandle internalErrorHandle;
	errorHandleInit(&internalErrorHandle);
	handleFreeError(scannerTokenListGetActive(tokenList, &currentToken, &internalErrorHandle), __LINE__, __FILE__);
	
	char tokenIdName[STATIC_STRING_LENGHT] = "";
	if(currentToken.type == TOKEN_STRINGVALUE || currentToken.type == TOKEN_ID){
		snprintf(tokenIdName, STATIC_STRING_LENGHT, "%s", currentToken.attribute.string);
	} else if(currentToken.type == TOKEN_INTVALUE){
		snprintf(tokenIdName, STATIC_STRING_LENGHT, "%ld", currentToken.attribute.integer);
	} else if(currentToken.type == TOKEN_FLOATVALUE){
		snprintf(tokenIdName, STATIC_STRING_LENGHT, "%f", currentToken.attribute.real);
	}
	
	return handleError(errorHandle, tokenTypes[currentToken.type], currentToken.pos_line, currentToken.pos_number, tokenIdName);
}
