/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: scanner.h
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#ifndef _SCANNER_H
#define _SCANNER_H

#include "common.h"
#include "symtable.h"



typedef enum
{
	TOKEN_EMPTY, //prazdny defaultni typ

	TOKEN_KEYWORD_ELSE, //Klicova slova
	TOKEN_KEYWORD_FLOAT64,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_FUNC,
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_INT,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_STRING,
	TOKEN_KEYWORD_PACKAGE,

	TOKEN_ID, //identifikator
	TOKEN_ASSIGN, //prirazeni =
	TOKEN_INIT, //inicializace promenne :=
	TOKEN_COMMA, //klasicka oddelovaci carka ,
	TOKEN_SEMICOLON, // strednik ;
	TOKEN_LROUNDBRACKET, //leva zavorka (
	TOKEN_RROUNDBRACKET, //prava zavorka )
	TOKEN_LCURLYBRACKET, //leva spicata zavorka {
	TOKEN_RCURLYBRACKET, //prava spicata zavorka }
	TOKEN_EOL, //end of line - konec radku
	TOKEN_EOF, //end of file - konec souboru
	TOKEN_INTVALUE, //celociselna hodnota
	TOKEN_FLOATVALUE, //desetinna hodnota
	TOKEN_STRINGVALUE, //hodnota string neboli retezec znaku

	TOKEN_ADD, //scitani +
	TOKEN_SUB, //odcitani -
	TOKEN_MUL, //nasobeni *
	TOKEN_DIV, //deleni /

	TOKEN_EQ, //relacni operator ==
	TOKEN_NEQ, //relacni operator !=
	TOKEN_GT, //relacni operator >
	TOKEN_LT, //relacni operator <
	TOKEN_GTE, //relacni operator >=
	TOKEN_LTE, //relacni operator <=

} Token_type;

// Ulozena jmena typu tokenu primarne pro debugovaci ucely
static char tokenTypes[][STATIC_STRING_LENGHT] = {"TOKEN_EMPTY", "TOKEN_KEYWORD_ELSE", "TOKEN_KEYWORD_FLOAT64", "TOKEN_KEYWORD_FOR", "TOKEN_KEYWORD_FUNC",
	"TOKEN_KEYWORD_IF", "TOKEN_KEYWORD_INT", "TOKEN_KEYWORD_RETURN", "TOKEN_KEYWORD_STRING", "TOKEN_KEYWORD_PACKAGE", "TOKEN_ID",
	"TOKEN_ASSIGN", "TOKEN_INIT", "TOKEN_COMMA", "TOKEN_SEMICOLON", "TOKEN_LROUNDBRACKET", "TOKEN_RROUNDBRACKET", "TOKEN_LCURLYBRACKET",
	"TOKEN_RCURLYBRACKET", "TOKEN_EOL", "TOKEN_EOF", "TOKEN_INTVALUE", "TOKEN_FLOATVALUE", "TOKEN_STRINGVALUE", "TOKEN_ADD", "TOKEN_SUB", "TOKEN_MUL",
	"TOKEN_DIV", "TOKEN_EQ", "TOKEN_NEQ", "TOKEN_GT", "TOKEN_LT", "TOKEN_GTE", "TOKEN_LTE"
};


typedef union {
	char *string;
	int64_t integer;
	double real;
} Token_attribute;



typedef struct {
	Token_type type;
	Token_attribute attribute;
	int pos_line;
	int pos_number;
} Token;


//funkce ktera kontroluje zda je string v tokenu keyword, kdyz ano tak zmeni jeho typ, uvolni pamet stringu a vraci true
bool keywordCheck (Token *currentToken);


//funkce co nacte a ulozi novy token na urcitou adresu tokenu
int scannerGetToken (Token *currentToken);


/****************************************************** SCANNER TOKEN LIST ******************************************************************************/

// Struktura prvku v obousmerne vazanem linearnim seznamu pro tokeny
typedef struct TokenListElement {
	Token token;
	struct TokenListElement *leftPtr;
	struct TokenListElement *rightPtr;
} *TokenListElementPtr;


// Struktura obousmerne vazaneho linearniho seznamu na tokeny
typedef struct {
	TokenListElementPtr active;
	TokenListElementPtr first;
	TokenListElementPtr last;
} TokenList;


// Inicializace dvousmerne vazaneho linearniho seznamu tokenu
int scannerTokenListInit(TokenList *tokenList, ErrorHandle *errorHandle);

// Pridani noveho prvku (tokenu) do seznamu
int scannerTokenListAdd(TokenList *tokenList, Token token, ErrorHandle *errorHandle);

// Smazani aktivniho prvku (tokenu) ze seznamu
int scannerTokenListDeleteActive(TokenList *tokenList, ErrorHandle *errorHandle);

// Posunuti aktivity seznamu na predchozi prvek
int scannerTokenListMovePrev(TokenList *tokenList, ErrorHandle *errorHandle);

// Posunuti aktivity seznamu na nasledujici prvek
int scannerTokenListMoveNext(TokenList *tokenList, ErrorHandle *errorHandle);

// Nacteni predchoziho prvku do adresy tokenu
int scannerTokenListGetPrev(TokenList *tokenList, Token *token, ErrorHandle *errorHandle);

// Nacteni aktivniho prvku do adresy tokenu
int scannerTokenListGetActive(TokenList *tokenList, Token *token, ErrorHandle *errorHandle);

// Nacteni nasledujiciho prvku do adresy tokenu
int scannerTokenListGetNext(TokenList *tokenList, Token *token, ErrorHandle *errorHandle);

// Nastavi prvni prvek seznamu jako aktivni prvek
int scannerTokenListSetActiveFirst(TokenList *tokenList, ErrorHandle *errorHandle);

// Nastavi posledni prvek seznamu jako aktivni prvek
int scannerTokenListSetActiveLast(TokenList *tokenList, ErrorHandle *errorHandle);

// Uvolni cely seznam i vsechny obsazene tokeny z pameti
int scannerTokenListFree(TokenList *tokenList);


/****************************************************** SCANNER GET TOKEN LIST ******************************************************************************/

// Naplni seznam tokeny pomoci opakovaneho volani funkce scannerGetToken
int scannerGetTokenList(TokenList *tokenList, ErrorHandle *errorHandle);


/****************************************************** PARSER GLOBAL DEFS ******************************************************************************/

#define STACK_INT_TO_DATA(x) (ParserStackData)((int)x)
#define STACK_TERM_TO_DATA(x) (ParserStackData)((Term_type)x)
#define STACK_SYMTABLE_TO_DATA(x) (ParserStackData)((SymTableBinTreePtr)x)
#define STACK_TOKEN_TO_DATA(x) (ParserStackData)((Token)x)
#define STACK_TOKEN_TYPE_TO_DATA(x) (ParserStackData)((Token_type)x)

#define STACK_DATA_TO_INT(x) ((int)x.result)
#define STACK_DATA_TO_TERM(x) ((Term_type)x.TERM_NONTERM)
#define STACK_DATA_TO_SYMTABLE(x) ((SymTableBinTreePtr)x.SYM_TABLE)
#define STACK_DATA_TO_TOKEN(x) ((Token)x.token)
#define STACK_DATA_TO_TOKEN_TYPE(x) ((Token_type)x.tokenType)

typedef union {
	int result;
	Term_type TERM_NONTERM;
	SymTableBinTreePtr SYM_TABLE;
	Token token;
	Token_type tokenType;
} ParserStackData;

typedef struct parserStackNode {
	ParserStackData data;
	struct parserStackNode *next;
} *ParserStackPtr;


#endif //defined _SCANNER_H
