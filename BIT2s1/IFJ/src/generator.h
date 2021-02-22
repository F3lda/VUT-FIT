/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: generator.h
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "common.h"
#include "scanner.h"
#include "parser.h"

#define FUNC_INPUTI "# --- func inputi ------------------------------\n\
LABEL inputi\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
READ TF@$1 int\n\n\
JUMPIFEQ $inputi_err TF@$1 nil@nil\n\
PUSHS TF@$1\n\
PUSHS int@0\n\
RETURN\n\
LABEL $inputi_err\n\
PUSHS int@0\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_INPUTF "# --- func inputf ------------------------------\n\
LABEL inputf\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
READ TF@$1 float\n\n\
JUMPIFEQ $inputf_err TF@$1 nil@nil\n\
PUSHS TF@$1\n\
PUSHS int@0\n\
RETURN\n\
LABEL $inputf_err\n\
PUSHS int@0\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_INPUTS "# --- func inputs ------------------------------\n\
LABEL inputs\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
READ TF@$1 string\n\n\
JUMPIFEQ $inputs_err TF@$1 nil@nil\n\
PUSHS TF@$1\n\
PUSHS int@0\n\
RETURN\n\
LABEL $inputs_err\n\
PUSHS int@0\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_INT_TO_FLOAT "# --- func int2float ------------------------------\n\
LABEL int2float\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
DEFVAR TF@$2\n\
POPS TF@$1\n\n\
INT2FLOAT TF@$2 TF@$1\n\n\
PUSHS TF@$2\n\
RETURN\n"

#define FUNC_FLOAT_TO_INT "# --- func float2int ------------------------------\n\
LABEL float2int\n\n\
CREATEFRAME\n\
DEFVAR TF@$1\n\
DEFVAR TF@$2\n\
POPS TF@$1\n\n\
FLOAT2INT TF@$2 TF@$1\n\n\
PUSHS TF@$2\n\
RETURN\n"

#define FUNC_LEN "# --- func len ------------------------------\n\
LABEL len\n\n\
CREATEFRAME\n\n\
DEFVAR TF@s\n\
POPS TF@s\n\n\
DEFVAR TF@$1\n\n\
STRLEN TF@$1 TF@s\n\n\
PUSHS TF@$1\n\n\
RETURN\n"

#define FUNC_SUBSTRING "# --- func substr ------------------------------\n\
LABEL substr\n\n\
CREATEFRAME\n\
DEFVAR TF@$s\n\
DEFVAR TF@$i\n\
DEFVAR TF@$n\n\n\
DEFVAR TF@$strlen\n\
DEFVAR TF@$substring\n\
DEFVAR TF@$char\n\
MOVE TF@$substring string@\n\
POPS TF@$s\n\
POPS TF@$i\n\
POPS TF@$n\n\
DEFVAR TF@$count\n\
DEFVAR TF@$result\n\
DEFVAR TF@$lenght\n\
STRLEN TF@$strlen TF@$s\n\
LT TF@$result TF@$i int@0\n\
JUMPIFEQ substr_err TF@$result bool@true\n\
GT TF@$result TF@$i TF@$strlen\n\
JUMPIFEQ substr_err TF@$result bool@true\n\
LT TF@$result TF@$n int@0\n\
JUMPIFEQ substr_err TF@$result bool@true\n\
SUB TF@$strlen TF@$strlen TF@$i\n\
GT TF@$result TF@$n TF@$strlen\n\
JUMPIFEQ n_less_strlen TF@$result bool@false\n\
MOVE TF@$n TF@$strlen\n\
LABEL n_less_strlen\n\
MOVE TF@$count TF@$i\n\
SUB TF@$count TF@$count int@1\n\
LABEL $concat_start\n\
ADD TF@$lenght TF@$i TF@$n\n\
SUB TF@$lenght TF@$lenght int@1\n\
LT TF@$result TF@$count TF@$lenght\n\
JUMPIFEQ $concat_end TF@$result bool@false\n\
ADD TF@$count TF@$count int@1\n\
GETCHAR TF@$char TF@$s TF@$count\n\
SUB TF@$count TF@$count int@1\n\
CONCAT TF@$substring TF@$substring TF@$char\n\
ADD TF@$count TF@$count int@1\n\
JUMP $concat_start\n\
LABEL $concat_end\n\n\
PUSHS TF@$substring\n\
PUSHS int@0\n\
RETURN\n\n\
LABEL substr_err\n\n\
PUSHS string@/000\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_ORD "# --- func ord ------------------------------\n\
LABEL ord\n\
CREATEFRAME\n\
DEFVAR TF@$s\n\
DEFVAR TF@$i\n\
DEFVAR TF@$strlen\n\
DEFVAR TF@$result\n\
DEFVAR TF@$intResult\n\
POPS TF@$s\n\
POPS TF@$i\n\n\
STRLEN TF@$strlen TF@$s\n\
GT TF@$result TF@$i TF@$strlen \n\
JUMPIFEQ $ord_err TF@$result bool@true\n\
EQ TF@$result TF@$i TF@$strlen \n\
JUMPIFEQ $ord_err TF@$result bool@true\n\
LT TF@$result TF@$i int@0 \n\
JUMPIFEQ $ord_err TF@$result bool@true\n\
STRI2INT TF@$intResult TF@$s TF@$i\n\n\
PUSHS TF@$intResult\n\
PUSHS int@0\n\n\
RETURN\n\n\
LABEL $ord_err\n\n\
PUSHS string@/000\n\
PUSHS int@1\n\n\
RETURN"

#define FUNC_CHR "# --- func chr ------------------------------\n\
LABEL chr\n\n\
CREATEFRAME\n\n\
DEFVAR TF@$0\n\
DEFVAR TF@$1\n\
DEFVAR TF@$2\n\
DEFVAR TF@$compareResult\n\n\
POPS TF@$1\n\
LT TF@$compareResult TF@$1 int@0\n\
JUMPIFEQ $chr_error TF@$compareResult bool@true\n\
GT TF@$compareResult TF@$1 int@255\n\
JUMPIFEQ $chr_error TF@$compareResult bool@true\n\n\
INT2CHAR TF@$2 TF@$1\n\
PUSHS TF@$2\n\
PUSHS int@0\n\
RETURN\n\n\
LABEL $chr_error\n\
PUSHS string@/000\n\
PUSHS int@1\n\
RETURN\n"

#define FUNC_GTE "# --- func GTE ------------------------------\n\
LABEL $greaterOrEqual\n\n\
DEFVAR TF@$op1\n\
DEFVAR TF@$op2\n\
DEFVAR TF@$res\n\
DEFVAR TF@$res1\n\
DEFVAR TF@$res2\n\n\
POPS TF@$op2\n\
POPS TF@$op1\n\n\n\
GT TF@$res1 TF@$op1 TF@$op2\n\
EQ TF@$res2 TF@$op1 TF@$op2\n\
OR TF@$res TF@$res1 TF@$res2\n\
PUSHS TF@$res\n\
RETURN\n"

#define FUNC_LTE "# --- func LTE ------------------------------\n\
LABEL $lessOrEqual\n\n\
DEFVAR TF@$op1\n\
DEFVAR TF@$op2\n\
DEFVAR TF@$res\n\
DEFVAR TF@$res1\n\
DEFVAR TF@$res2\n\n\
POPS TF@$op2\n\
POPS TF@$op1\n\n\n\
LT TF@$res1 TF@$op1 TF@$op2\n\
EQ TF@$res2 TF@$op1 TF@$op2\n\
OR TF@$res TF@$res1 TF@$res2\n\
PUSHS TF@$res\n\
RETURN\n"


void generatorFree();

void pushArguments(ParserStackPtr *argStack, int argCount);

void printPushs(ParserStackPtr *varStack, int argCount);

//Tiskne pop návratových hodnot funkcí mimo main
void printPops(ParserStackPtr *varStack, int varCount);

//Tiskne řetězcovou hodnotu podle zadání
void printString(char* string, bool printNewLine);

//Generování kódu
int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ParserStackPtr *semanticRuleStack, ErrorHandle *errorHandle);


typedef struct integerStack {
	int ID;
	int bracketCount;
	int keywordType;
	struct integerStack *next;
} *integerStack;

void integerStackFree(integerStack *stack);


#define IF 0
#define IF_EXPRESSION 1
#define ELSE 2
#define FOR 3
#define FOR_INIT 4
#define FOR_EXPRESSION 5
#define FOR_ASSIGNMENT 6

#endif //defined _GENERATOR_H