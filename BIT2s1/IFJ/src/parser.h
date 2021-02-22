/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: parser.h
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#ifndef _PARSER_H
#define _PARSER_H

#include "common.h"
#include "scanner.h"
#include "generator.h"



int parserAnalyze(TokenList *tokenList, ErrorHandle *errorHandle);


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunPredictiveSyntaxAnalysis(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle);

int parserRunPrecedentSyntaxAnalysis(TokenList *expressionList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ErrorHandle *errorHandle);

int parserSemanticAnalysis(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ParserStackPtr *semanticRuleStack, ErrorHandle *errorHandle);

int parserSemanticChangeIDsToTypes(ParserStackPtr *expressionStack, ParserStackPtr *symtableStack, ErrorHandle *errorHandle);

Token_type parserSemanticExpressionCheckOperatorsAndOperands(ParserStackPtr *expressionStack, ErrorHandle *errorHandle);

IDdataType parserSemanticTokenTypeToVarType(Token_type tokenType);

Token parserSemanticVarTypeToToken(IDdataType varDataType);

int parserTokenListFree(TokenList *tokenList);

int parserLeftAnalysis(int ruleNumber, ParserStackPtr *leftAndRightAnalysisStack);

int parserRightAnalysis(int ruleNumber, ParserStackPtr *leftAndRightAnalysisStack);


/****************************************************** PRECEDENT SYNTAX STACK ******************************************************************************/

ParserStackData parserStackPrecedentTop(ParserStackPtr *stack);

int parserStackPrecedentTopAddHandle(ParserStackPtr *stack);

int parserStackPrecedentTopHasHandle(ParserStackPtr *stack);

int parserStackPrecedentTopPopAndPushRule(ParserStackPtr *stack, ParserStackPtr *semanticPrecedentStack, ParserStackPtr *semanticRuleStack);


/****************************************************** SYNTAX STACK ******************************************************************************/

void parserStackInit(ParserStackPtr *stack);

ParserStackData parserStackPush(ParserStackPtr *stack, ParserStackData data);

ParserStackData parserStackPeek(ParserStackPtr *stack);

ParserStackData parserStackPop(ParserStackPtr *stack);

void parserStackFree(ParserStackPtr *stack);


#endif //defined _PARSER_H
