/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: common.h
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>



FILE *FILE_INPUT;
FILE *FILE_OUTPUT;
FILE *FILE_ERROR;


#define ARRAY_SIZE(x)  (int)(sizeof(x) / sizeof((x)[0]))

#define STATIC_STRING_LENGHT 255

#define ROW(x) (x-1)
#define ITEM(x) (x-1)



/****************************************************** SYNTAX ANALYSIS ******************************************************************************/

#define IS_TERM(x) (x < NONTERM_program)
#define IS_NONTERM(x) (x > TERM_PSEUDO_EPSILON)

#define TERM_TO_TABLE(x) (x - TERM_KEYWORD_PACKAGE)
#define NONTERM_TO_TABLE(x) (x - NONTERM_program)
#define TERM_TO_PREC_TABLE(x) (x - TERM_PREC_ADD)

#define PREC_GRAMM_RULES_START 50
#define PREC_GRAMM_RULES_MAX 15
#define PREC_GRAMM_RULES_END 65
#define PREC_GRAMM_RULES_TO_TABLE(x) (x + 50)
#define PREC_GRAMM_RULES_TO_RULE(x) (x - 50)




typedef enum
{
	TERM_EMPTY, //prazdny defaultni typ



	// SEŘAZENO PODLE LL TABULKY
	TERM_KEYWORD_PACKAGE,
	TERM_ID, //identifikator
	TERM_EOL, //end of line - konec radku
	TERM_KEYWORD_FUNC,
	TERM_LROUNDBRACKET, //leva zavorka (
	TERM_RROUNDBRACKET, //prava zavorka )
	TERM_EOF, //end of file - konec souboru
	TERM_COMMA, //klasicka oddelovaci carka ,
	TERM_LCURLYBRACKET, //leva spicata zavorka {
	TERM_RCURLYBRACKET, //prava spicata zavorka }
	TERM_KEYWORD_RETURN,
	TERM_EXPRESSION,
	TERM_KEYWORD_IF,
	TERM_KEYWORD_FOR,
	TERM_SEMICOLON, // strednik ;
	TERM_INIT, //inicializace promenne :=
	TERM_ASSIGN, //prirazeni =
	TERM_KEYWORD_INT,
	TERM_KEYWORD_FLOAT64,
	TERM_KEYWORD_STRING,
	TERM_INTVALUE, //celociselna hodnota
	TERM_FLOATVALUE, //desetinna hodnota
	TERM_STRINGVALUE, //hodnota string neboli retezec znaku



	// NEVYUŽITÉ TERMINALY
	TERM_KEYWORD_ELSE,
	
	
	
	// SEŘAZENO PODLE PRECEDENČNÍ TABULKY
	TERM_PREC_ADD, //scitani +
	TERM_PREC_SUB, //odcitani -
	TERM_PREC_MUL, //nasobeni *
	TERM_PREC_DIV, //deleni /

	TERM_PREC_EQ, //relacni operator ==
	TERM_PREC_NEQ, //relacni operator !=
	TERM_PREC_GT, //relacni operator >
	TERM_PREC_LT, //relacni operator <
	TERM_PREC_GTE, //relacni operator >=
	TERM_PREC_LTE, //relacni operator <=
	
	TERM_PREC_LROUNDBRACKET, //leva zavorka (
	TERM_PREC_RROUNDBRACKET, //prava zavorka )
	
	TERM_PREC_INTVALUE, //celociselna hodnota
	TERM_PREC_FLOATVALUE, //desetinna hodnota
	TERM_PREC_STRINGVALUE, //hodnota string neboli retezec znaku
	TERM_PREC_IDVALUE, // ID pro precedenční tabulku
	TERM_PSEUDO_DOLLAR,
	TERM_PSEUDO_HANDLE,
	TERM_PSEUDO_EPSILON,
	
	
	
	// SEŘAZENO PODLE LL TABULKY
	NONTERM_program,
	NONTERM_param_in_first,
	NONTERM_param_in_next,
	NONTERM_funkce_body,
	NONTERM_param_out_next,
	NONTERM_statements,
	NONTERM_statement_id,
	NONTERM_id_next,
	NONTERM_for_definition,
	NONTERM_for_assignment,
	NONTERM_for_assign_id,
	NONTERM_id_expression,
	NONTERM_ids_expression,
	NONTERM_type,
	NONTERM_expr_next,
	NONTERM_arg_first,
	NONTERM_arg_next,
	NONTERM_arg,
	
	
	
	// NEVYUŽITÉ NETERMINALY
	NONTERM_expression
	
} Term_type;

static char termTypes[][STATIC_STRING_LENGHT] = {"TERM_EMPTY", "TERM_KEYWORD_PACKAGE", "TERM_ID", "TERM_EOL", "TERM_KEYWORD_FUNC",
	"TERM_LROUNDBRACKET", "TERM_RROUNDBRACKET", "TERM_EOF", "TERM_COMMA", "TERM_LCURLYBRACKET", "TERM_RCURLYBRACKET", "TERM_KEYWORD_RETURN",
	"TERM_EXPRESSION", "TERM_KEYWORD_IF", "TERM_KEYWORD_FOR", "TERM_SEMICOLON", "TERM_INIT", "TERM_ASSIGN", "TERM_KEYWORD_INT", "TERM_KEYWORD_FLOAT64",
	"TERM_KEYWORD_STRING", "TERM_INTVALUE", "TERM_FLOATVALUE", "TERM_STRINGVALUE", 
	
	"TERM_KEYWORD_ELSE",

	"TERM_PREC_ADD", "TERM_PREC_SUB", "TERM_PREC_MUL", "TERM_PREC_DIV", "TERM_PREC_EQ", "TERM_PREC_NEQ", "TERM_PREC_GT", "TERM_PREC_LT", "TERM_PREC_GTE", 
	"TERM_PREC_LTE", "TERM_PREC_LROUNDBRACKET", "TERM_PREC_RROUNDBRACKET", "TERM_PREC_INTVALUE", "TERM_PREC_FLOATVALUE", "TERM_PREC_STRINGVALUE", "TERM_PREC_IDVALUE",
	"TERM_PSEUDO_DOLLAR", "TERM_PSEUDO_HANDLE", "TERM_PSEUDO_EPSILON",
	
	"NONTERM_program", "NONTERM_param_in_first", "NONTERM_param_in_next", "NONTERM_funkce_body", "NONTERM_param_out_next", "NONTERM_statements",
	"NONTERM_statement_id", "NONTERM_id_next", "NONTERM_for_definition", "NONTERM_for_assignment", "NONTERM_for_assign_id", "NONTERM_id_expression",
	"NONTERM_ids_expression", "NONTERM_type", "NONTERM_expr_next", "NONTERM_arg_first", "NONTERM_arg_next", "NONTERM_arg",

	"NONTERM_expression"
};

static Term_type MAP_TOKEN_TO_TERM[] = {
	TERM_EMPTY, //prazdny defaultni typ

	TERM_KEYWORD_ELSE,
	TERM_KEYWORD_FLOAT64,
	TERM_KEYWORD_FOR,
	TERM_KEYWORD_FUNC,
	TERM_KEYWORD_IF,
	TERM_KEYWORD_INT,
	TERM_KEYWORD_RETURN,
	TERM_KEYWORD_STRING,
	TERM_KEYWORD_PACKAGE,

	TERM_ID, //identifikator
	TERM_ASSIGN, //prirazeni =
	TERM_INIT, //inicializace promenne :=
	TERM_COMMA, //klasicka oddelovaci carka ,
	TERM_SEMICOLON, // strednik ;
	TERM_LROUNDBRACKET, //leva zavorka (
	TERM_RROUNDBRACKET, //prava zavorka )
	TERM_LCURLYBRACKET, //leva spicata zavorka {
	TERM_RCURLYBRACKET, //prava spicata zavorka }
	TERM_EOL, //end of line - konec radku
	TERM_EOF, //end of file - konec souboru
	TERM_INTVALUE, //celociselna hodnota
	TERM_FLOATVALUE, //desetinna hodnota
	TERM_STRINGVALUE, //hodnota string neboli retezec znaku

	TERM_PREC_ADD, //scitani +
	TERM_PREC_SUB, //odcitani -
	TERM_PREC_MUL, //nasobeni *
	TERM_PREC_DIV, //deleni /

	TERM_PREC_EQ, //relacni operator ==
	TERM_PREC_NEQ, //relacni operator !=
	TERM_PREC_GT, //relacni operator >
	TERM_PREC_LT, //relacni operator <
	TERM_PREC_GTE, //relacni operator >=
	TERM_PREC_LTE, //relacni operator <=
};

static Term_type MAP_TOKEN_TO_PREC_TERM[] = {
	TERM_EMPTY, //prazdny defaultni typ

	TERM_KEYWORD_ELSE,
	TERM_KEYWORD_FLOAT64,
	TERM_KEYWORD_FOR,
	TERM_KEYWORD_FUNC,
	TERM_KEYWORD_IF,
	TERM_KEYWORD_INT,
	TERM_KEYWORD_RETURN,
	TERM_KEYWORD_STRING,
	TERM_KEYWORD_PACKAGE,

	TERM_PREC_IDVALUE, //identifikator
	TERM_ASSIGN, //prirazeni =
	TERM_INIT, //inicializace promenne :=
	TERM_COMMA, //klasicka oddelovaci carka ,
	TERM_SEMICOLON, // strednik ;
	TERM_PREC_LROUNDBRACKET, //leva zavorka (
	TERM_PREC_RROUNDBRACKET, //prava zavorka )
	TERM_LCURLYBRACKET, //leva spicata zavorka {
	TERM_RCURLYBRACKET, //prava spicata zavorka }
	TERM_PSEUDO_DOLLAR, //end of line - konec radku
	TERM_EOF, //end of file - konec souboru
	TERM_PREC_INTVALUE, //celociselna hodnota
	TERM_PREC_FLOATVALUE, //desetinna hodnota
	TERM_PREC_STRINGVALUE, //hodnota string neboli retezec znaku

	TERM_PREC_ADD, //scitani +
	TERM_PREC_SUB, //odcitani -
	TERM_PREC_MUL, //nasobeni *
	TERM_PREC_DIV, //deleni /

	TERM_PREC_EQ, //relacni operator ==
	TERM_PREC_NEQ, //relacni operator !=
	TERM_PREC_GT, //relacni operator >
	TERM_PREC_LT, //relacni operator <
	TERM_PREC_GTE, //relacni operator >=
	TERM_PREC_LTE, //relacni operator <=
};


#define GRAMMAR_RULE_LIST__ROW_MAX_SIZE 13

static Term_type GrammmarRuleList[][GRAMMAR_RULE_LIST__ROW_MAX_SIZE] = {
	{TERM_KEYWORD_PACKAGE, TERM_ID, TERM_EOL, NONTERM_program},
	{TERM_KEYWORD_FUNC, TERM_ID, TERM_LROUNDBRACKET, NONTERM_param_in_first, TERM_RROUNDBRACKET, NONTERM_funkce_body, NONTERM_program},
	{TERM_EOL, NONTERM_program},
	{TERM_EOF},

	{TERM_ID, NONTERM_type, NONTERM_param_in_next},
	{TERM_PSEUDO_EPSILON},
	{TERM_COMMA, TERM_ID, NONTERM_type, NONTERM_param_in_next},
	{TERM_PSEUDO_EPSILON},

	{TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET}, //9
	{TERM_LROUNDBRACKET, NONTERM_type, NONTERM_param_out_next, TERM_RROUNDBRACKET, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET},

	{TERM_COMMA, NONTERM_type, NONTERM_param_out_next},
	{TERM_PSEUDO_EPSILON},
	
	{TERM_KEYWORD_INT},
	{TERM_KEYWORD_FLOAT64},
	{TERM_KEYWORD_STRING},//15
	
	//<statements>
	{TERM_ID, NONTERM_statement_id, TERM_EOL, NONTERM_statements},
	{TERM_KEYWORD_RETURN, TERM_EXPRESSION, NONTERM_expr_next, TERM_EOL, NONTERM_statements}, //14
	{TERM_KEYWORD_IF, TERM_EXPRESSION, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET, TERM_KEYWORD_ELSE, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET, TERM_EOL, NONTERM_statements},
	{TERM_KEYWORD_FOR, NONTERM_for_definition, TERM_SEMICOLON, TERM_EXPRESSION, TERM_SEMICOLON, NONTERM_for_assignment, TERM_LCURLYBRACKET, TERM_EOL, NONTERM_statements, TERM_RCURLYBRACKET, TERM_EOL, NONTERM_statements},
	{TERM_EOL, NONTERM_statements},
	{TERM_PSEUDO_EPSILON},

	//<statement_id>
	{TERM_LROUNDBRACKET, NONTERM_arg_first, TERM_RROUNDBRACKET},
	{TERM_INIT, NONTERM_id_expression},
	{TERM_ASSIGN, NONTERM_id_expression},
	{TERM_COMMA, TERM_ID, NONTERM_id_next},

	{TERM_COMMA, TERM_ID, NONTERM_id_next},
	{TERM_ASSIGN, NONTERM_ids_expression},

	//<for_definition>
	{TERM_ID, TERM_INIT, NONTERM_id_expression},
	{TERM_PSEUDO_EPSILON},
	
	{TERM_ID, NONTERM_for_assign_id},
	{TERM_PSEUDO_EPSILON},
	{TERM_COMMA, TERM_ID, NONTERM_for_assign_id},
	{TERM_ASSIGN, NONTERM_ids_expression},

	//<id_expression>
	{TERM_ID, TERM_LROUNDBRACKET, NONTERM_arg_first, TERM_RROUNDBRACKET},
	{TERM_EXPRESSION},

	{TERM_ID, TERM_LROUNDBRACKET, NONTERM_arg_first, TERM_RROUNDBRACKET},
	{TERM_EXPRESSION, NONTERM_expr_next},


	{TERM_COMMA, TERM_EXPRESSION, NONTERM_expr_next},
	{TERM_PSEUDO_EPSILON},
	
	//<value_first>
	{TERM_INTVALUE, NONTERM_arg_next},
	{TERM_FLOATVALUE, NONTERM_arg_next},
	{TERM_STRINGVALUE, NONTERM_arg_next},
	{TERM_ID, NONTERM_arg_next},
	{TERM_PSEUDO_EPSILON},
	
	{TERM_COMMA, NONTERM_arg, NONTERM_arg_next},
	{TERM_PSEUDO_EPSILON},
	
	{TERM_INTVALUE},
	{TERM_FLOATVALUE},
	{TERM_STRINGVALUE},
	{TERM_ID},
	
	
	
	// <expression> pravidla
	{TERM_PREC_LROUNDBRACKET, NONTERM_expression, TERM_PREC_RROUNDBRACKET},
	
	{NONTERM_expression, TERM_PREC_ADD, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_SUB, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_MUL, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_DIV, NONTERM_expression},
	
	{NONTERM_expression, TERM_PREC_EQ, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_NEQ, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_GT, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_LT, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_GTE, NONTERM_expression},
	{NONTERM_expression, TERM_PREC_LTE, NONTERM_expression},
	
	{TERM_PREC_INTVALUE},
	{TERM_PREC_FLOATVALUE},
	{TERM_PREC_STRINGVALUE},
	{TERM_PREC_IDVALUE},
};

static Term_type GrammmarExprLeftRuleList[] = {
	NONTERM_expression,
	
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression,
	NONTERM_expression
};


#define LL_TABLE__ROW_MAX_SIZE 23

static char LLTable[][LL_TABLE__ROW_MAX_SIZE] = {
	{ 1, 0, 3, 2, 0, 0, 4}, //<program>
	{ 0, 5, 0, 0, 0, 6}, 	//<param_in_first>
	{ 0, 0, 0, 0, 0, 8, 0, 7}, 	//<param_in_next>
	{ 0, 0, 0, 0,10, 0, 0, 0, 9}, //<funkce_body>
	{ 0, 0, 0, 0, 0,12, 0,11}, 	//<param_out_next>
	{ 0,16,20, 0, 0, 0, 0, 0, 0,21,17, 0,18,19}, //<statements>
	{ 0, 0, 0, 0,22, 0, 0,25, 0, 0, 0, 0, 0, 0, 0,23,24}, //<statement_id>
	{ 0, 0, 0, 0, 0, 0, 0,26, 0, 0, 0, 0, 0, 0, 0, 0,27}, //<id_next>
	{ 0,28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,29}, //<for_definition>
	{ 0,30, 0, 0, 0, 0, 0, 0,31}, //<for_assignment>
	{ 0,33, 0, 0, 0, 0, 0,32, 0, 0, 0, 0, 0, 0, 0, 0,33}, //<for_assign_id>
	{ 0,34, 0, 0,35, 0, 0, 0, 0, 0, 0,35, 0, 0, 0, 0, 0, 0, 0, 0,35,35,35}, //<id_expression>
	{ 0,36, 0, 0,37, 0, 0, 0, 0, 0, 0,37, 0, 0, 0, 0, 0, 0, 0, 0,37,37,37}, //<ids_expression>
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,13,14,15},	//<type>
	{ 0, 0,39, 0, 0, 0, 0,38,39}, //<expr_next>
	{ 0,43, 0, 0, 0,44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,40,41,42}, //<arg_first>
	{ 0, 0, 0, 0, 0,46, 0,45}, //<arg_next>
	{ 0,50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,47,48,49} //<arg>
	//Pozor, číslování pravidel začíná od 1, ne od 0.
};


#define PRECEDENCE_TABLE__ROW_MAX_SIZE 17

static char PrecedenceTable[][PRECEDENCE_TABLE__ROW_MAX_SIZE] = {
	{ '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '>'}, // +
	{ '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '>'}, // -
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '>'}, // *
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '>'}, //'/'
	{ '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '<', '<', '<', '>'}, //==
	{ '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '<', '<', '<', '>'}, //!=
	{ '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '<', '<', '<', '>'}, // >
	{ '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '<', '<', '<', '>'}, // <
	{ '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '<', '<', '<', '>'}, //>=
	{ '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '<', '<', '<', '>'}, //<=
	{ '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<',  0 }, //'('
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 ,  0 ,  0 ,  0 , '>'}, //')'
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 ,  0 ,  0 ,  0 , '>'}, //int
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 ,  0 ,  0 ,  0 , '>'}, //float
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 ,  0 ,  0 ,  0 , '>'}, //string
	{ '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 ,  0 ,  0 ,  0 , '>'}, //id
	{ '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<',  0 , '<', '<', '<', '<',  0 }  //'$'
};


/****************************************************** ERROR HANDLE ******************************************************************************/

//onumerovani navratovych hodnot pro chyby = errorID
#define ALL_OK 0
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define SEM_UNDEFINED_VAR_ERROR 3
#define SEM_DATA_TYPE_ERROR 4
#define SEM_TYPE_COMPATIBILITY_ERROR 5
#define SEM_FUNC_PARAM_RETURN_ERROR 6
#define SEM_OTHER_ERROR 7
#define ZERO_DIVISION_ERROR 9
#define INTERNAL_ERROR 99
#define ERROR_ALREADY_EXISTS 100


typedef struct {
	int errorID;
	char errorString[STATIC_STRING_LENGHT];
	char errorPosFile[STATIC_STRING_LENGHT];
	int errorPosLine;
} ErrorHandle;

// vytiskne chybovou hlášku podle čísla chyby
int handleError(ErrorHandle errorHandle, char *lastOKTokenType, int lastOKTokenPosLine, int lastOKTokenPosNumber, char *lastOKTokenString);

void handleFreeError(int errorID, int errorLine, char *errorFile);

int errorHandleInit(ErrorHandle *errorHandle);

int errorExists(ErrorHandle errorHandle);

int errorSet(int errorID, char *errorString, const char *errorPosFile, int errorPosLine, ErrorHandle *errorHandle);


#endif //defined _COMMON_H