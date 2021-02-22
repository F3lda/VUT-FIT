/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: parser.c
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#include "parser.h"


ParserStackPtr semanticStack = NULL;

ParserStackPtr returnIfStack = NULL;


int parserAnalyze(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	SymTableBinTreePtr globalSymTable;
	symTableInit(&globalSymTable);
	
	
	// Sémantický pre-run, naplnění tabulky definicemi funkcí a proměnnými
	parserPreRun(tokenList, &globalSymTable, errorHandle); 
	
	
	// Syntaktická analýza + Sémantická analýza
	parserRunPredictiveSyntaxAnalysis(tokenList, &globalSymTable, errorHandle); 
	
	
	// tree test print
//	Print_tree(globalSymTable);

	//printf("\n\n-----------------------------------------------------------------------------------\n\n");
	//if(!errorExists(*errorHandle)) {
	//	printf("\nSYNTAX OK!\n\n");
	//} else {
	//	printf("\nSYNTAX ERROR!\n\n");
	//}
	//printf("\n\n-----------------------------------------------------------------------------------\n\n");
	
	
	handleFreeError(symTableDispose(&globalSymTable), __LINE__, __FILE__);
	
	
	parserStackFree(&semanticStack);
	
	parserStackFree(&returnIfStack);
	
	
	return errorHandle->errorID;
}


/****************************************************** PARSER SYMTABLE PRERUN ******************************************************************************/

int parserPreRun(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle); // nastavení seznamu tokenů na začátek
	
	parserSymTableInitBuiltIn(globalSymTable, errorHandle); // vložení předdefinovaných funkcí

	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	Token tempToken;
	tempToken.type = TOKEN_EMPTY;
	tempToken.pos_line = 0;
	tempToken.pos_number = 0;
	
	int left_brackets_count = 0;
	int prolog_exists = 0;
	
	do{
		scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
		//******************************************************************************
		if(currentToken.type == TOKEN_ID){ // máme ID...
			char *tokenIdName = tokenList->active->token.attribute.string;
			if(scannerTokenListGetPrev(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_KEYWORD_FUNC){ // ...a jedná se o definici funkce
				if(symTableSearch(*globalSymTable, tokenIdName, NULL, errorHandle) == 0){ // pokud je již v tabulce, jedná se o redefinici -> error
					if(scannerTokenListGetNext(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_LROUNDBRACKET){
						//******************************************************************************
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListMoveNext(tokenList, errorHandle);
						
						
						// vytvoření functionLocalSymTable
						SymTableBinTreePtr localSymTable;
						symTableInit(&localSymTable);
						// add blackhole var '_'
						symTableInsert(&localSymTable, "_", symTableInitDataInLine(VAR, false, NIL, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
						
						// vytvoření functionParamDataTypes
						SymTableParamList paramList;
						symTableParamListInit(&paramList, errorHandle);
						// vytvoření functionReturnDataTypes
						SymTableParamList returnList;
						symTableParamListInit(&returnList, errorHandle);
						
						
						// uložení vstupních parametrů
						while(errorHandle->errorID == ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != TOKEN_RROUNDBRACKET){
							scannerTokenListGetPrev(tokenList, &tempToken, errorHandle);
							
							if(currentToken.type == TOKEN_ID){
								if(symTableSearch(localSymTable, currentToken.attribute.string, NULL, errorHandle) == 0){
									scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
									if(tempToken.type == TOKEN_KEYWORD_INT){
										//add id to local table
										symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, INT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
										//add type to param list
										symTableParamListAdd(&paramList, INT, errorHandle);
									} else if(tempToken.type == TOKEN_KEYWORD_FLOAT64){
										//add id to local table
										symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, FLOAT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
										//add type to param list
										symTableParamListAdd(&paramList, FLOAT, errorHandle);
									} else if(tempToken.type == TOKEN_KEYWORD_STRING){
										//add id to local table
										symTableInsert(&localSymTable, currentToken.attribute.string, symTableInitDataInLine(VAR, false, STRING, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
										//add type to param list
										symTableParamListAdd(&paramList, STRING, errorHandle);
									} else {
										errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
									}
								} else {
									errorSet(SEM_UNDEFINED_VAR_ERROR, "parserPreRun: redefinice promenne", __FILE__, __LINE__, errorHandle);
								}
								scannerTokenListMoveNext(tokenList, errorHandle);
							} else if(currentToken.type != TOKEN_COMMA){// jinak se cyklus neukončí
								errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
							}
							scannerTokenListMoveNext(tokenList, errorHandle);
						}
						
						
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
						scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
						// uložení výstupních parametrů (není povinné)
						if(errorHandle->errorID == ALL_OK && currentToken.type == TOKEN_LROUNDBRACKET){
							scannerTokenListMoveNext(tokenList, errorHandle);
							
							while(errorHandle->errorID == ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && currentToken.type != TOKEN_RROUNDBRACKET){
								scannerTokenListGetPrev(tokenList, &tempToken, errorHandle);
								
								if(currentToken.type == TOKEN_KEYWORD_INT){
									//add type to return list
									symTableParamListAdd(&returnList, INT, errorHandle);
								} else if(currentToken.type == TOKEN_KEYWORD_FLOAT64){
									//add type to return list
									symTableParamListAdd(&returnList, FLOAT, errorHandle);
								} else if(currentToken.type == TOKEN_KEYWORD_STRING){
									//add type to return list
									symTableParamListAdd(&returnList, STRING, errorHandle);
								} else if(currentToken.type != TOKEN_COMMA){// jinak se cyklus neukončí
									errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
								}
								scannerTokenListMoveNext(tokenList, errorHandle);
							}
						}
						
						
						/*// kontrola špičaté závorky a konce řádku -> kontroluje syntaktická analýza
						scannerTokenListMoveNext(tokenList, errorHandle);
						scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
						scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
						
						if(!(errorHandle->errorID == ALL_OK && currentToken.type == TOKEN_LCURLYBRACKET && tempToken.type == TOKEN_EOL)){
							errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);
						}
						left_brackets_count++;*/
						
						
						// přidání funkce do tabulky symbolů (musí být přidáno i při chybě pro uvolnění paměti)
						if(errorHandle->errorID != ALL_OK){
							ErrorHandle internalErrorHandle;
							errorHandleInit(&internalErrorHandle);
							symTableInsert(globalSymTable, tokenIdName, symTableInitData(FUNC, true, NIL, paramList, returnList, localSymTable), &internalErrorHandle);
						} else {
							symTableInsert(globalSymTable, tokenIdName, symTableInitData(FUNC, true, NIL, paramList, returnList, localSymTable), errorHandle);
						}
						//******************************************************************************
					} else {
						errorSet(SYNTAX_ERROR, "parserPreRun: spatny token v hlavicce funkce", __FILE__, __LINE__, errorHandle);// chybí levá závorka u vstupních parametrů
					}
				} else {
					errorSet(SEM_UNDEFINED_VAR_ERROR, "parserPreRun: redefinice funkce", __FILE__, __LINE__, errorHandle);// redefinice funkce
				}
			}/* else if(left_brackets_count == 0 && tempToken.type == TOKEN_EOL && scannerTokenListGetNext(tokenList, &tempToken, errorHandle) == ALL_OK && tempToken.type == TOKEN_INIT){// .. jedná se o globální proměnnou (Není v zadání a ani v GRAMATICE! -> nebude fungovat po odkomentování)
				scannerTokenListMoveNext(tokenList, errorHandle);
				scannerTokenListGetNext(tokenList, &currentToken, errorHandle);
				if(currentToken.type == TOKEN_INTVALUE){
					symTableInsert(globalSymTable, tokenIdName, symTableInitDataInLine(VAR, false, INT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
				} else if(currentToken.type == TOKEN_FLOATVALUE){
					symTableInsert(globalSymTable, tokenIdName, symTableInitDataInLine(VAR, false, FLOAT, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
				} else if(currentToken.type == TOKEN_STRINGVALUE){
					symTableInsert(globalSymTable, tokenIdName, symTableInitDataInLine(VAR, false, STRING, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
				}
			}*/
		} else if(currentToken.type == TOKEN_LCURLYBRACKET){
			left_brackets_count++;
		} else if(currentToken.type == TOKEN_RCURLYBRACKET){
			left_brackets_count--;
		} else if(currentToken.type == TOKEN_KEYWORD_PACKAGE){
			prolog_exists++;
			
			Token currentToken;
			currentToken.type = TOKEN_EMPTY;
			currentToken.pos_line = 0;
			currentToken.pos_number = 0;
			
			scannerTokenListGetNext(tokenList, &currentToken, errorHandle);
			
			if(currentToken.type != TOKEN_ID || strcmp(currentToken.attribute.string,"main") != 0){
				errorSet(SYNTAX_ERROR, "parserPreRun: prolog - wrong or missing!", __FILE__, __LINE__, errorHandle);
			}
		}
		//******************************************************************************
		if(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK){scannerTokenListMoveNext(tokenList, errorHandle);}
	} while(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK);
	
	
	// kontrola zda symtable obsahuje funkci main s 0 parametry a pouze jeden prolog
	SymTableData data;
	if(!errorExists(*errorHandle) && symTableSearch(*globalSymTable, "main", &data, errorHandle) == 1){
		if(symTableParamListGetSize(&data.functionParamDataTypes, errorHandle) != 0 || symTableParamListGetSize(&data.functionReturnDataTypes, errorHandle) != 0){
			errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "parserPreRun: main function has params!", __FILE__, __LINE__, errorHandle);
		} else if(prolog_exists != 1){
			errorSet(SYNTAX_ERROR, "parserPreRun: prolog - wrong count (maybe doesn't exist)!", __FILE__, __LINE__, errorHandle);
		}
	} else if(!errorExists(*errorHandle)){
		errorSet(SEM_UNDEFINED_VAR_ERROR, "parserPreRun: main function doesn't exist!", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}


// Naplnění tabulky built-in funkcemi
int parserSymTableInitBuiltIn(SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	symTableInsert(globalSymTable, "inputs", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){STRING, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "inputi", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){INT, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "inputf", symTableInitDataInLine(FUNC, true, NIL, 0, NULL, 2, ((IDdataType[]){FLOAT, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "print", symTableInitDataInLine(FUNC, true, NIL, -1, NULL, 0, NULL, NULL, errorHandle), errorHandle);

	symTableInsert(globalSymTable, "int2float", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){INT}), 1, ((IDdataType[]){FLOAT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "float2int", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){FLOAT}), 1, ((IDdataType[]){INT}), NULL, errorHandle), errorHandle);

	symTableInsert(globalSymTable, "len", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){STRING}), 1, ((IDdataType[]){INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "substr", symTableInitDataInLine(FUNC, true, NIL, 3, ((IDdataType[]){STRING, INT, INT}), 2, ((IDdataType[]){STRING, INT}), NULL, errorHandle), errorHandle);

	symTableInsert(globalSymTable, "ord", symTableInitDataInLine(FUNC, true, NIL, 2, ((IDdataType[]){STRING, INT}), 2, ((IDdataType[]){INT, INT}), NULL, errorHandle), errorHandle);
	symTableInsert(globalSymTable, "chr", symTableInitDataInLine(FUNC, true, NIL, 1, ((IDdataType[]){INT}), 2, ((IDdataType[]){STRING, INT}), NULL, errorHandle), errorHandle);

	return errorHandle->errorID;
}


/****************************************************** PARSER RUN ANALYZE ******************************************************************************/

int parserRunPredictiveSyntaxAnalysis(TokenList *tokenList, SymTableBinTreePtr *globalSymTable, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	int result = 1; // 0 = ok; 1 = fail
	int inExpr = 0;
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	Token tempToken;
	tempToken.type = TOKEN_EMPTY;
	tempToken.pos_line = 0;
	tempToken.pos_number = 0;
	
	scannerTokenListSetActiveFirst(tokenList, errorHandle);
	
	
	
	// symtable stack
	ParserStackPtr symtableStack;
	parserStackInit(&symtableStack);
	
	
	// syntax stack
	ParserStackPtr syntaxStack;
	parserStackInit(&syntaxStack);	
	
	//parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(TERM_EOF)); // $ -> ukončovací symbol -> už je součástí neterminálu <program>
	parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(NONTERM_program)); // S -> počáteční symbol
	
	
	// leftAndRightAnalysis stack (stack of rule numbers)
	ParserStackPtr leftAndRightAnalysisRuleStack;
	parserStackInit(&leftAndRightAnalysisRuleStack);
	
	
	// expression token list
	TokenList expressionList;
	scannerTokenListInit(&expressionList, errorHandle);
	
	
	while(result != ALL_OK && scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK)
	{
		// na stacku je ukončovací symbol - $
		if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_EOF){
		
			// i aktuální token je ukončovací symbol
			if(MAP_TOKEN_TO_TERM[currentToken.type] == TERM_EOF){
				result = ALL_OK;
			// jinak chyba
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NO EOF TOKEN", __FILE__, __LINE__, errorHandle);
			}
		
		// na stacku je terminal
		} else if(IS_TERM(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))){
			
			// epsilon jen přeskočíme
			if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_PSEUDO_EPSILON){
				parserStackPop(&syntaxStack);
			
			// jsme v EXPRESSION
			} else if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == TERM_EXPRESSION){
				
				// EXPRESSION - START
				parserStackPop(&syntaxStack); // pop EXPRESSION neterminal
			
				scannerTokenListAdd(&expressionList, currentToken, errorHandle);
				scannerTokenListMoveNext(tokenList, errorHandle);
				
				// pokud se jedná o terminál z precedeční gramatiky
				while(scannerTokenListGetActive(tokenList, &currentToken, errorHandle) == ALL_OK && TERM_PREC_ADD <= MAP_TOKEN_TO_PREC_TERM[currentToken.type] && MAP_TOKEN_TO_PREC_TERM[currentToken.type] <= TERM_PREC_IDVALUE){
					// EXPRESSION - ADD TOKEN
					
					scannerTokenListAdd(&expressionList, currentToken, errorHandle);
					scannerTokenListMoveNext(tokenList, errorHandle);
				}
				
				parserRunPrecedentSyntaxAnalysis(&expressionList, &symtableStack, globalSymTable, &leftAndRightAnalysisRuleStack, errorHandle);
				
				// EXPRESSION - END
				
			// porovnání zásobníku - na vrcholu zásobníku je stejný terminál jako aktuální token
			} else if(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)) == MAP_TOKEN_TO_TERM[currentToken.type]){
				parserSemanticAnalysis(tokenList, &symtableStack, globalSymTable, &leftAndRightAnalysisRuleStack, NULL, errorHandle);
				
				parserStackPop(&syntaxStack);
				scannerTokenListMoveNext(tokenList, errorHandle);
				
			// jinak chyba
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - NOT SAME TERM", __FILE__, __LINE__, errorHandle);
			}
		
		// na stacku je neterminal
		} else if(IS_NONTERM(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))){
			
			// načtení dalšího tokenu
			tempToken.type = TOKEN_EMPTY;
			if(tokenList->active != tokenList->last){
				scannerTokenListGetNext(tokenList, &tempToken, errorHandle);
			}
			
			// získání pravidla z LL tabulky
			int LLtableResult = LLTable[NONTERM_TO_TABLE(STACK_DATA_TO_TERM(parserStackPeek(&syntaxStack)))][TERM_TO_TABLE(MAP_TOKEN_TO_TERM[currentToken.type])];
			// kotrola o jeden token dopředu, zda se jedná o volání funkce nebo expression
			if((LLtableResult == 34 || LLtableResult == 36) && MAP_TOKEN_TO_TERM[currentToken.type] == TERM_ID && MAP_TOKEN_TO_TERM[tempToken.type] != TERM_LROUNDBRACKET){LLtableResult++;}
			
			// pravidlo nalezeno
			if(LLtableResult != 0){
				
				// levý rozbor
				parserLeftAnalysis(LLtableResult, &leftAndRightAnalysisRuleStack);
				
				LLtableResult--; // indexování v poli
				
				parserStackPop(&syntaxStack); // stack pop neterminal
				
				int i;
				for(i = GRAMMAR_RULE_LIST__ROW_MAX_SIZE-1; i >= 0; i--) // stack push reversal
				{
					Term_type ruleItem = GrammmarRuleList[LLtableResult][i];
					if(ruleItem != 0){
						parserStackPush(&syntaxStack, STACK_TERM_TO_DATA(ruleItem));
					}
				}
			
			// jinak chyba
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - LL TABLE -> NO RULE", __FILE__, __LINE__, errorHandle);
			}
		}
	}
	
	handleFreeError(parserTokenListFree(&expressionList), __LINE__, __FILE__);
	
	
	parserStackFree(&syntaxStack);
	
	if(symtableStack != NULL){
		while(symtableStack->next != NULL){
			SymTableBinTreePtr symtableFree = STACK_DATA_TO_SYMTABLE(parserStackPop(&symtableStack));
			handleFreeError(symTableDispose(&symtableFree), __LINE__, __FILE__);
		}
	}
	parserStackFree(&symtableStack);
	
	parserStackFree(&leftAndRightAnalysisRuleStack);
	
	
	if(result != ALL_OK && !errorExists(*errorHandle)) {
		errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR (INTERNAL_ERROR) - UNKNOWN", __FILE__, __LINE__, errorHandle);
	}
	
	return errorHandle->errorID;
}


int parserRunPrecedentSyntaxAnalysis(TokenList *expressionList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisRuleStack, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}


	ParserStackPtr expressionRuleStack;
	parserStackInit(&expressionRuleStack);
	
	
	ParserStackPtr semanticPrecedentStack;
	parserStackInit(&semanticPrecedentStack);
	
	
	ParserStackPtr precedentStack;
	parserStackInit(&precedentStack);
	
	parserStackPush(&precedentStack, STACK_TERM_TO_DATA(TERM_PSEUDO_DOLLAR)); // koncový symbol a počáteční symbol precedenčního zásobníku
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	
	currentToken.type = TOKEN_EOL;
	scannerTokenListAdd(expressionList, currentToken, errorHandle); // koncový symbol (token) seznamu tokenů
	currentToken.type = TOKEN_EOF;
	scannerTokenListAdd(expressionList, currentToken, errorHandle); // koncový symbol (token) seznamu tokenů
	
	scannerTokenListSetActiveFirst(expressionList, errorHandle);

	while(scannerTokenListGetActive(expressionList, &currentToken, errorHandle) == ALL_OK){
		
		Term_type stackTopTerm = STACK_DATA_TO_TERM(parserStackPrecedentTop(&precedentStack));
		Term_type tokenTerm = MAP_TOKEN_TO_PREC_TERM[currentToken.type];
		char operation = PrecedenceTable[TERM_TO_PREC_TABLE(stackTopTerm)][TERM_TO_PREC_TABLE(tokenTerm)];
		
		if(stackTopTerm == TERM_PSEUDO_DOLLAR && tokenTerm == TERM_PSEUDO_DOLLAR){
			generatorGenerateCode(expressionList, symtableStack, globalSymTable, leftAndRightAnalysisRuleStack, &expressionRuleStack, errorHandle);
			break;
		} else if(operation == '='){
			parserSemanticAnalysis(expressionList, symtableStack, globalSymTable, leftAndRightAnalysisRuleStack, &expressionRuleStack, errorHandle);
			
			parserStackPush(&semanticPrecedentStack, STACK_TOKEN_TO_DATA(currentToken));
			parserStackPush(&precedentStack, STACK_TERM_TO_DATA(tokenTerm));
			
			scannerTokenListMoveNext(expressionList, errorHandle);
		} else if(operation == '<'){
			parserSemanticAnalysis(expressionList, symtableStack, globalSymTable, leftAndRightAnalysisRuleStack, &expressionRuleStack, errorHandle);
			
			parserStackPrecedentTopAddHandle(&precedentStack);
			
			parserStackPush(&semanticPrecedentStack, STACK_TOKEN_TO_DATA(currentToken));
			parserStackPush(&precedentStack, STACK_TERM_TO_DATA(tokenTerm));
			
			scannerTokenListMoveNext(expressionList, errorHandle);
		} else if(operation == '>'){
			generatorGenerateCode(expressionList, symtableStack, globalSymTable, leftAndRightAnalysisRuleStack, &expressionRuleStack, errorHandle);
			
			int rightAnalysis = parserStackPrecedentTopPopAndPushRule(&precedentStack, &semanticPrecedentStack, &expressionRuleStack);
			if(rightAnalysis > 0){
				// pravý rozbor
				parserRightAnalysis(rightAnalysis, leftAndRightAnalysisRuleStack);
			} else {
				errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - PRECEDENT TABLE -> NO RULE", __FILE__, __LINE__, errorHandle);
			}
		} else {
			errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - PRECEDDENT (LAST OK TOKEN = FIRST OK TOKEN AFTER EXPRESSION)", __FILE__, __LINE__, errorHandle);
		}
	}
	// clear expressionList
	handleFreeError(parserTokenListFree(expressionList), __LINE__, __FILE__);
	scannerTokenListInit(expressionList, errorHandle);
	
	parserStackFree(&precedentStack);
	
	parserStackFree(&semanticPrecedentStack);
	
	parserStackFree(&expressionRuleStack);

	return errorHandle->errorID;
}


int parserSemanticAnalysis(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisRuleStack, ParserStackPtr *expressionRuleStack, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	// sémantika
	static int CodeBlockNumber = 1;
	static int returnExists = 0;
	static int inFor = 0;
	static int functionShouldEnd = 0;
	
	static int inReturn = 0;
	static int inDefinition = 0;
	static int inAssignment = 0;
	static int inFunctionCall = 0;
	static int inExpression = 0;
	
	static char *inFunctionName = NULL;
	static int SemicolonCount = 0;
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	Token prevToken;
	prevToken.type = TOKEN_EMPTY;
	prevToken.pos_line = 0;
	prevToken.pos_number = 0;
	Token prevPrevToken;
	prevPrevToken.type = TOKEN_EMPTY;
	prevPrevToken.pos_line = 0;
	prevPrevToken.pos_number = 0;
	
	// get current Token
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	// get previous Tokens
	ErrorHandle errorHandleMove;
	ErrorHandle errorHandleGet;
	errorHandleInit(&errorHandleMove);
	errorHandleInit(&errorHandleGet);
	scannerTokenListMovePrev(tokenList, &errorHandleMove);
	scannerTokenListGetActive(tokenList, &prevToken, &errorHandleGet);
	scannerTokenListGetPrev(tokenList, &prevPrevToken, &errorHandleGet);
	scannerTokenListMoveNext(tokenList, &errorHandleMove);
	
	
	
	// DEFINICE FUNKCE
	// při definici funkce se přidá do symtableStacku její localSymtable a CodeBlockNumber se nastaví na 1
	if(currentToken.type == TOKEN_ID && prevToken.type == TOKEN_KEYWORD_FUNC){
		char *tokenIdName = tokenList->active->token.attribute.string;
		SymTableData data;
		if(symTableSearch(*globalSymTable, tokenIdName, &data, errorHandle) == 1){ // pokud funkce existuje v globalSymTable
			if(data.functionLocalSymTable != NULL){
				data.functionLocalSymTable->data.functionParamDataTypes.size = data.functionParamDataTypes.size;
				data.functionLocalSymTable->data.functionReturnDataTypes.size = data.functionReturnDataTypes.size;
			}
			parserStackPush(symtableStack, STACK_SYMTABLE_TO_DATA(data.functionLocalSymTable));
			CodeBlockNumber = 1;
		} else {
			errorSet(SEM_UNDEFINED_VAR_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING FUNCTION", __FILE__, __LINE__, errorHandle);
		}
	}
	
	
	// VSTUP DO NOVÉHO ROZSAHU PLATNOSTI
	// při levé složené závorce '{' nebo klíčovém slově FOR přičteme CodeBlockNumber++ a přidáme novou symtable do symtableListu
	if(currentToken.type == TOKEN_LCURLYBRACKET || currentToken.type == TOKEN_KEYWORD_FOR){
		SymTableBinTreePtr newSymTable;
		symTableInit(&newSymTable);
		parserStackPush(symtableStack, STACK_SYMTABLE_TO_DATA(newSymTable));
		CodeBlockNumber++;
		
		// ZAČÁTEK CYKLU FOR
		// při klíčovém slově FOR inFor = 1;
		if(currentToken.type == TOKEN_KEYWORD_FOR){
			inFor = 1;
		}
	}
	
	
	// UKONČENÍ ROZSAHU PLATNOSTI
	// při pravé složené závorce '}' jednou nebo dvakrát inFor == 1 pokud CodeBlockNumber-- a odstranění symtables (1 nebo 2)
	if(currentToken.type == TOKEN_RCURLYBRACKET){
		if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1){
			SymTableBinTreePtr freeSymTable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
			handleFreeError(symTableDispose(&freeSymTable), __LINE__, __FILE__);
			CodeBlockNumber--;
		} else {
			errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING SYMTABLE", __FILE__, __LINE__, errorHandle);
		}
		
		// KONEC CYKLU FOR
		// pokud inFor == 1 a '}' -> inFor = 0; a odstranění ještě jedné tabulky
		if(inFor == 1){
			inFor = 0;
			
			if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1){
				SymTableBinTreePtr freeSymTable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
				handleFreeError(symTableDispose(&freeSymTable), __LINE__, __FILE__);
				CodeBlockNumber--;
			} else {
				errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING SYMTABLE (FOR)", __FILE__, __LINE__, errorHandle);
			}
		}
	}
	
	
	// KONEC DEFINICE FUNKCE
	// při pravé složené závorce '}' a CodeBlockNumber == 2
	// dojde také ke kontrole returnExists -> pokud má funkce návratové hodnoty a returnExists == 1 -> OK
	if(currentToken.type == TOKEN_RCURLYBRACKET && CodeBlockNumber == 1){// protože při definici == 1 a pak je teprve závorka {++
		CodeBlockNumber--;

		if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1){
			SymTableBinTreePtr functionLocalSymTable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
			if(functionLocalSymTable != NULL){
				if(functionLocalSymTable->data.functionReturnDataTypes.size > 0 && returnExists != 1){
					errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - key word return doesn't EXIST!", __FILE__, __LINE__, errorHandle);
					
				} else if(functionLocalSymTable->data.functionReturnDataTypes.size == 0 && returnExists == 1){
					errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - key word return EXISTS!", __FILE__, __LINE__, errorHandle);
					
				}
			} else {
				if(returnExists == 1){
					errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - key word return EXISTS!", __FILE__, __LINE__, errorHandle);
				}
			}
		} else {
			errorSet(SYNTAX_ERROR, "PARSER_ANALYZE: SYNTAX_ERROR - WRONG COUNT OF '}' - end of function not found", __FILE__, __LINE__, errorHandle);
		}
	}
	
	
	
	// RETURN NENALEZEN
	// při levé složené závorce '{' nastavíme returnExists na 0
	if(currentToken.type == TOKEN_LCURLYBRACKET){
		// KONTROLA RETURN V OBOU ČÁSTECH IF
		if(prevToken.type == TOKEN_KEYWORD_ELSE && returnExists == 0){
			parserStackPush(&returnIfStack, STACK_INT_TO_DATA(CodeBlockNumber));
		}
		returnExists = 0;
	}
	
	// KONTROLA RETURN V OBOU ČÁSTECH IF
	if(prevToken.type == TOKEN_RCURLYBRACKET && currentToken.type != TOKEN_KEYWORD_ELSE){
		if((CodeBlockNumber+1) == STACK_DATA_TO_INT(parserStackPeek(&returnIfStack))){
			parserStackPop(&returnIfStack);
			returnExists = 0;
		}
	}
	
	// KONTROLA RETURN JAKO POSLEDNÍ PŘÍKAZ V ROZSAHU PLATNOSTI (možná by stačilo jen varování)
	if(currentToken.type == TOKEN_EOL && CodeBlockNumber >= 2 && inReturn == 1){
		functionShouldEnd = 1;
	}
	if(functionShouldEnd == 1 && currentToken.type != TOKEN_EOL){
		functionShouldEnd = 0;
		if(currentToken.type != TOKEN_RCURLYBRACKET){
			errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - KEYWORD RETURN TOO EARLY!", __FILE__, __LINE__, errorHandle);
		}
	}
	
	
	// RETURN NALEZEN
	// při klíčovém slově return nastavíme returnExists a inReturn na 1
	if(currentToken.type == TOKEN_KEYWORD_RETURN){
		returnExists = 1;
		inReturn = 1;
	}
	
	
	// DEFINICE FUNKCE
	if(prevPrevToken.type == TOKEN_KEYWORD_FUNC && prevToken.type == TOKEN_ID && currentToken.type == TOKEN_LROUNDBRACKET){
		inFunctionName = prevToken.attribute.string;
	}
	
	
	// VOLÁNÍ FUNKCE
	if(prevPrevToken.type != TOKEN_KEYWORD_FUNC && prevToken.type == TOKEN_ID && currentToken.type == TOKEN_LROUNDBRACKET){
		inFunctionCall = 1;
		
		// push semantic stack
		parserStackPush(&semanticStack, STACK_TOKEN_TO_DATA(currentToken));
	}
	
	
	// DEFINICE PROMĚNNÉ
	if(currentToken.type == TOKEN_INIT){
		inDefinition = 1;
	}
	
	
	// PŘIŘAZENÍ PROMĚNNÉ
	if(currentToken.type == TOKEN_ASSIGN){
		inAssignment = 1;
	}
	
	// EXPRESSION (PDOMÍNKA)                    (FOR CYKLUS)
	if(currentToken.type == TOKEN_KEYWORD_IF || (inFor == 1 && SemicolonCount == 1)){
		inExpression = 1;
	}
	
	// POČET STŘEDNÍKŮ
	if(currentToken.type == TOKEN_SEMICOLON){
		SemicolonCount++;
	}
	
	
	
	// LEVÁ ČÁST PŘÍKAZU
	if(inReturn == 0 && inDefinition == 0 && inAssignment == 0 && inFunctionCall == 0 && inExpression == 0){
		
		// POČET VÝSTUPNÍCH PROMĚNNÝCH
		if(currentToken.type == TOKEN_ID){
			
			// push semantic stack
			parserStackPush(&semanticStack, STACK_TOKEN_TO_DATA(currentToken));
		}
		
	// PRAVÁ ČÁST PŘÍKAZU
	} else if(currentToken.type == TOKEN_ID || currentToken.type == TOKEN_COMMA
		|| currentToken.type == TOKEN_INTVALUE || currentToken.type == TOKEN_FLOATVALUE || currentToken.type == TOKEN_STRINGVALUE 
		|| currentToken.type == TOKEN_INIT || currentToken.type == TOKEN_ASSIGN || currentToken.type == TOKEN_KEYWORD_RETURN
		|| currentToken.type == TOKEN_ADD || currentToken.type == TOKEN_SUB || currentToken.type == TOKEN_MUL || currentToken.type == TOKEN_DIV
		|| currentToken.type == TOKEN_EQ || currentToken.type == TOKEN_NEQ || currentToken.type == TOKEN_GT || currentToken.type == TOKEN_LT || currentToken.type == TOKEN_GTE || currentToken.type == TOKEN_LTE){
		
		// push semantic stack
		parserStackPush(&semanticStack, STACK_TOKEN_TO_DATA(currentToken));
	}
	
	
	
	
	
	// kotrola všech in kontrol
	if(currentToken.type == TOKEN_EOL || currentToken.type == TOKEN_EOF || currentToken.type == TOKEN_SEMICOLON){
		
		// PROMĚNNÁ V EXPRESSION
		// kontrola, že proměnné jsou v některé symtable na listu, a že [všechny typy v expression jsou stejné a správné]
		if(inExpression == 1 && !errorExists(*errorHandle)){
			//printf("EXPRESSION\n");
			
			parserSemanticChangeIDsToTypes(&semanticStack, symtableStack, errorHandle);
			
			Token_type expressionType = parserSemanticExpressionCheckOperatorsAndOperands(&semanticStack, errorHandle);
			if(!(TOKEN_EQ <= expressionType && expressionType <= TOKEN_LTE)){
				// nikdy nenastane -> máme jako syntaktickou chybu
				errorSet(SEM_TYPE_COMPATIBILITY_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - WRONG TYPE IN IF or FOR expression", __FILE__, __LINE__, errorHandle);
			}
		}
		
		
		// RETURN
		// kontrola, zda [return vrací správné typy a počet]
		if(inReturn == 1 && !errorExists(*errorHandle)){
			//printf("RETURN (%s)\n", inFunctionName);
			
			int functionParamNumber = 0;
			
			ParserStackPtr paramStack;
			parserStackInit(&paramStack);
			
			// expressions to value types - start
			
			ParserStackPtr expressionStack;
			parserStackInit(&expressionStack);
			
			ParserStackPtr top = (semanticStack);
			ParserStackPtr topStart = (semanticStack);
			while(top != NULL){
				
				if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_COMMA || (STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_KEYWORD_RETURN && topStart != (semanticStack))){
					parserSemanticChangeIDsToTypes(&expressionStack, symtableStack, errorHandle);
					
					Token_type functionParamTokenType = parserSemanticExpressionCheckOperatorsAndOperands(&expressionStack, errorHandle);
					
					parserStackPush(&paramStack, STACK_TOKEN_TYPE_TO_DATA(functionParamTokenType));
					
					parserStackFree(&expressionStack);
					functionParamNumber++;
				} else {
					parserStackPush(&expressionStack, parserStackPeek(&semanticStack));
				}
				
				if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_KEYWORD_RETURN){
					break;
				}
				
				top = top->next;
				
				parserStackPop(&semanticStack);
			}
			
			parserStackFree(&expressionStack);
			
			// expressions to value types - end
			
			parserStackPop(&semanticStack); // pop TOKEN_KEYWORD_RETURN
			
			
			
			SymTableData data;
			if(symTableSearch(*globalSymTable, inFunctionName, &data, errorHandle) == 1){
				
				if(functionParamNumber == data.functionReturnDataTypes.size){
					
					if(data.functionReturnDataTypes.size != 0 && !errorExists(*errorHandle)){// if error not exists
						
						symTableParamListSetActiveFirst(&data.functionReturnDataTypes, errorHandle);
						
						IDdataType paramDataType;
						while(STACK_DATA_TO_INT(parserStackPeek(&paramStack)) != -1){
							
							symTableParamListGetActive(&data.functionReturnDataTypes, &paramDataType, errorHandle);
							
							if(parserSemanticTokenTypeToVarType(STACK_DATA_TO_TOKEN_TYPE(parserStackPeek(&paramStack))) != paramDataType){
								
								errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - WRONG RETURN PARAM TYPE", __FILE__, __LINE__, errorHandle);
								break;
							}
							
							parserStackPop(&paramStack);
							if(STACK_DATA_TO_INT(parserStackPeek(&paramStack)) != -1){
								symTableParamListMoveNext(&data.functionReturnDataTypes, errorHandle);
							}
						}
					}
					
				} else {
					errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - WRONG COUNT OF RETURN PARAMS", __FILE__, __LINE__, errorHandle);
				}
				
			} else if(!errorExists(*errorHandle)){
				errorSet(SEM_UNDEFINED_VAR_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - FUNCTION MISSING IN GLOBALSYMTABLE", __FILE__, __LINE__, errorHandle);
			}
			
			parserStackFree(&paramStack);
		}
		
		
		// VOLÁNÍ FUNKCE
		// kotrola, že funkce existuje v globalSymTable a vrací správný počet hodnot porovnáním s LeftSideVarCount a [vstupní parametry jsou správného typu]
		if(inFunctionCall == 1 && !errorExists(*errorHandle)){
			//printf("FUNCTION\n");
			
			int functionParamNumber = 0;
			
			ParserStackPtr paramStack;
			parserStackInit(&paramStack);
			
			// expressions to value types - start
			
			ParserStackPtr expressionStack;
			parserStackInit(&expressionStack);
			
			ParserStackPtr top = (semanticStack);
			ParserStackPtr topStart = (semanticStack);
			while(top != NULL){
				
				if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_COMMA || (STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_LROUNDBRACKET && topStart != (semanticStack))){
					parserSemanticChangeIDsToTypes(&expressionStack, symtableStack, errorHandle);
					
					Token_type functionParamTokenType = parserSemanticExpressionCheckOperatorsAndOperands(&expressionStack, errorHandle);
					
					parserStackPush(&paramStack, STACK_TOKEN_TYPE_TO_DATA(functionParamTokenType));
					
					parserStackFree(&expressionStack);
					functionParamNumber++;
				} else {
					parserStackPush(&expressionStack, parserStackPeek(&semanticStack));
				}
				
				if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_LROUNDBRACKET){
					break;
				}
				
				top = top->next;
				
				parserStackPop(&semanticStack);
			}
			
			parserStackFree(&expressionStack);
			
			// expressions to value types - end
			
			parserStackPop(&semanticStack); // pop TOKEN_LROUNDBRACKET
			
			
			
			if(STACK_DATA_TO_INT(parserStackPeek(&semanticStack)) != -1){
				
				Token functionIdToken = STACK_DATA_TO_TOKEN(parserStackPeek(&semanticStack)); // pop function id token
				if(functionIdToken.type == TOKEN_ID){
					
					parserStackPop(&semanticStack); // pop function name Token
					
					SymTableData data;
					if(symTableSearch(*globalSymTable, functionIdToken.attribute.string, &data, errorHandle) == 1){
						
						if(functionParamNumber == data.functionParamDataTypes.size || data.functionParamDataTypes.size == -1){
							
							// check input params
							
							// pokud funkce nemá proměnný počet vstupních parametrů (print()) nebo 0 vstupních parametrů
							if(data.functionParamDataTypes.size != -1 && data.functionParamDataTypes.size != 0){
								
								symTableParamListSetActiveFirst(&data.functionParamDataTypes, errorHandle);
								
								IDdataType paramDataType;
								while(STACK_DATA_TO_INT(parserStackPeek(&paramStack)) != -1){
									
									symTableParamListGetActive(&data.functionParamDataTypes, &paramDataType, errorHandle);
									
									if(parserSemanticTokenTypeToVarType(STACK_DATA_TO_TOKEN_TYPE(parserStackPeek(&paramStack))) != paramDataType){
										
										errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - FUNCTION WRONG PARAM TYPE", __FILE__, __LINE__, errorHandle);
										break;
									}
									
									parserStackPop(&paramStack);
									if(STACK_DATA_TO_INT(parserStackPeek(&paramStack)) != -1){
										symTableParamListMoveNext(&data.functionParamDataTypes, errorHandle);
									}
								}
							}
							
							
							// push output params on stack
							
							if(data.functionReturnDataTypes.size != 0 && !errorExists(*errorHandle)){// if error not exists
								
								symTableParamListSetActiveFirst(&data.functionReturnDataTypes, errorHandle);
								
								IDdataType paramDataType;
								while(symTableParamListGetActive(&data.functionReturnDataTypes, &paramDataType, errorHandle) == ALL_OK){
									
									parserStackPush(&semanticStack, STACK_TOKEN_TO_DATA(parserSemanticVarTypeToToken(paramDataType)));
									
									symTableParamListMoveNext(&data.functionReturnDataTypes, errorHandle);
								}
								
								errorHandleInit(errorHandle); // clear last get active error
							}
							
						} else {
							errorSet(SEM_FUNC_PARAM_RETURN_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - FUNCTION WRONG COUNT OF PARAMS", __FILE__, __LINE__, errorHandle);
						}
						
					} else if(!errorExists(*errorHandle)){
						errorSet(SEM_UNDEFINED_VAR_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - FUNCTION MISSING IN GLOBALSYMTABLE", __FILE__, __LINE__, errorHandle);
					}
					
				} else {
					errorSet(INTERNAL_ERROR, "PARSER_ANALYZE: INTERNAL_ERROR", __FILE__, __LINE__, errorHandle);
				}
				
			} else {
				errorSet(INTERNAL_ERROR, "PARSER_ANALYZE: INTERNAL_ERROR", __FILE__, __LINE__, errorHandle);
			}
			
			parserStackFree(&paramStack);
		}
		
		
		// PŘIŘAZENÍ PROMĚNNÉ
		// kontrola, že proměnné jsou v nějaké symtable na stacku a změna jejich typu podle hodnoty
		if(inAssignment == 1 && !errorExists(*errorHandle)){
			//printf("ASSIGNMENT\n");
			
			// if the right side of the cmd wans't prepared in FUNCTION part -> prepare for check
			if(inFunctionCall == 0){
				
				ParserStackPtr paramStack;
				parserStackInit(&paramStack);
				
				// expressions to value types - start
				
				ParserStackPtr expressionStack;
				parserStackInit(&expressionStack);
				
				ParserStackPtr top = (semanticStack);
				ParserStackPtr topStart = (semanticStack);
				while(top != NULL){
					
					if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_COMMA || (STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_ASSIGN && topStart != (semanticStack))){
						parserSemanticChangeIDsToTypes(&expressionStack, symtableStack, errorHandle);
						
						Token_type functionParamTokenType = parserSemanticExpressionCheckOperatorsAndOperands(&expressionStack, errorHandle);
						
						Token tempToken;
						tempToken.type = functionParamTokenType;
						tempToken.pos_line = 0;
						tempToken.pos_number = 0;
						
						parserStackPush(&paramStack, STACK_TOKEN_TO_DATA(tempToken));
						
						parserStackFree(&expressionStack);
					} else if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_LROUNDBRACKET){// TOKEN_LROUNDBRACKET just pop
						parserStackPush(&expressionStack, parserStackPeek(&semanticStack));
					}
					
					if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_ASSIGN){
						break;
					}
					
					top = top->next;
					
					parserStackPop(&semanticStack);
				}
				
				parserStackFree(&expressionStack);
				
				// expressions to value types - end
				
				parserSemanticChangeIDsToTypes(&paramStack, symtableStack, errorHandle);

				while(STACK_DATA_TO_INT(parserStackPeek(&paramStack)) != -1){
					parserStackPush(&semanticStack, parserStackPop(&paramStack));
				}
				
				parserStackFree(&paramStack);
			}
			
			
			
			// change var types in symtable
			
			ParserStackPtr leftSideStack;
			parserStackInit(&leftSideStack);
			
			ParserStackPtr rightSideStack;
			parserStackInit(&rightSideStack);
			
			// split left and right side of cmd
			int assignTokenLoaded = 0;
			while(STACK_DATA_TO_INT(parserStackPeek(&semanticStack)) != -1){
				if(STACK_DATA_TO_TOKEN(parserStackPeek(&semanticStack)).type == TOKEN_ASSIGN){
					parserStackPop(&semanticStack);
					assignTokenLoaded = 1;
				} else if(assignTokenLoaded == 0){
					parserStackPush(&rightSideStack, parserStackPop(&semanticStack));
				} else if(assignTokenLoaded == 1){
					parserStackPush(&leftSideStack, parserStackPop(&semanticStack));
				}
			}
			
			
			//find ID in symtableStack and change var type
			while(STACK_DATA_TO_INT(parserStackPeek(&leftSideStack)) != -1 && STACK_DATA_TO_INT(parserStackPeek(&rightSideStack)) != -1 && !errorExists(*errorHandle)){
				
				Token currentToken = STACK_DATA_TO_TOKEN(parserStackPeek(&leftSideStack));
				if(currentToken.type == TOKEN_ID && strcmp(currentToken.attribute.string,"_") != 0){
					ParserStackPtr topSymtable = (*symtableStack);
					while(topSymtable != NULL){
						
						SymTableBinTreePtr currentLocalSymtable = STACK_DATA_TO_SYMTABLE(parserStackPeek(&topSymtable));
						
						SymTableData data;
						if(symTableSearch(currentLocalSymtable, currentToken.attribute.string, &data, errorHandle) == 1){
							
							if(data.idDataType != parserSemanticTokenTypeToVarType(STACK_DATA_TO_TOKEN(parserStackPeek(&rightSideStack)).type)){
								// SEMANTIC ERROR - VALUE WRONG TYPE
								errorSet(SEM_TYPE_COMPATIBILITY_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - ASSIGNMENT - VALUE WRONG TYPE", __FILE__, __LINE__, errorHandle);
							}
							
							break;
						}
						
						topSymtable = topSymtable->next;
					}
					if(topSymtable == NULL){
						// SEMANTIC ERROR - MISSING VAR
						errorSet(SEM_UNDEFINED_VAR_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - ASSIGNMENT - MISSING VAR", __FILE__, __LINE__, errorHandle);
					}
				} else if(strcmp(currentToken.attribute.string,"_") != 0){
					// error wrong type
					errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - ASSIGNMENT - WRONG TOKEN TYPE", __FILE__, __LINE__, errorHandle);
				}
				
				parserStackPop(&leftSideStack);
				parserStackPop(&rightSideStack);
			}
			
			if((STACK_DATA_TO_INT(parserStackPeek(&leftSideStack)) != -1 || STACK_DATA_TO_INT(parserStackPeek(&rightSideStack)) != -1) && !errorExists(*errorHandle)){
				// ERROR wrong count of params
				errorSet(SEM_OTHER_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - ASSIGNMENT - WRONG COUNT OF PARAMS", __FILE__, __LINE__, errorHandle);
			}
			
			parserStackFree(&leftSideStack);
			parserStackFree(&rightSideStack);
		}
		
		
		// DEFINICE PROMĚNNÉ
		// kontrola, že proměnná není v aktuální symtable a její přidání s typem hodnoty do aktuální symtable
		if(inDefinition == 1 && !errorExists(*errorHandle)){
			//printf("DEFINITION\n");
			
			// expression to value type - start
			
			// expression stack
			ParserStackPtr expressionStack;
			parserStackInit(&expressionStack);	
			
			ParserStackPtr top = (semanticStack);
			while(top != NULL && STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_INIT){
				top = top->next;
				
				parserStackPush(&expressionStack, parserStackPop(&semanticStack));
			}
			
			parserSemanticChangeIDsToTypes(&expressionStack, symtableStack, errorHandle);
			
			Token_type newVarTokenType = parserSemanticExpressionCheckOperatorsAndOperands(&expressionStack, errorHandle);
			
			parserStackFree(&expressionStack);
			
			IDdataType newVarDataType = parserSemanticTokenTypeToVarType(newVarTokenType);

			// expression to value type - end

			parserStackPop(&semanticStack); // pop TOKEN_INIT
			
			
			
			if(newVarTokenType != TOKEN_EMPTY){
			
				if(STACK_DATA_TO_INT(parserStackPeek(symtableStack)) != -1 && STACK_DATA_TO_INT(parserStackPeek(&semanticStack)) != -1){
					Token newVarIdToken = STACK_DATA_TO_TOKEN(parserStackPeek(&semanticStack)); // pop var id token
					if(newVarIdToken.type == TOKEN_ID){
						
						// pop currentLocalSymtable
						SymTableBinTreePtr currentLocalSymtable = STACK_DATA_TO_SYMTABLE(parserStackPop(symtableStack));
						
						if(symTableSearch(currentLocalSymtable, newVarIdToken.attribute.string, NULL, errorHandle) == 0){
							symTableInsert(&currentLocalSymtable, newVarIdToken.attribute.string, symTableInitDataInLine(VAR, true, newVarDataType, 0, NULL, 0, NULL, NULL, errorHandle), errorHandle);
						} else {
							// SEMANTIC ERROR - VAR REDEFINITION
							errorSet(SEM_UNDEFINED_VAR_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - VAR REDEFINITION", __FILE__, __LINE__, errorHandle);
						}
						// push currentLocalSymtable
						parserStackPush(symtableStack, STACK_SYMTABLE_TO_DATA(currentLocalSymtable));
						
					} else {
						errorSet(INTERNAL_ERROR, "PARSER_ANALYZE: INTERNAL_ERROR", __FILE__, __LINE__, errorHandle);
					}
				} else {
					errorSet(INTERNAL_ERROR, "PARSER_ANALYZE: INTERNAL_ERROR", __FILE__, __LINE__, errorHandle);
				}
			}
		}
		
		
		
		inReturn = 0;
		inDefinition = 0;
		inAssignment = 0;
		inFunctionCall = 0;
		inExpression = 0;
		if(currentToken.type == TOKEN_EOL){
			SemicolonCount = 0;
		}
		
		// clear semantic stack
		parserStackFree(&semanticStack);
	}
	
	
	
	// call generator and generate code from token and rules
	generatorGenerateCode(tokenList, symtableStack, globalSymTable, leftAndRightAnalysisRuleStack, expressionRuleStack, errorHandle);
	
	
	return errorHandle->errorID;
}


int parserSemanticChangeIDsToTypes(ParserStackPtr *expressionStack, ParserStackPtr *symtableStack, ErrorHandle *errorHandle)
{
	ParserStackPtr top = (*expressionStack);
	while(top != NULL){
		
		Token currentToken = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
		if(currentToken.type == TOKEN_ID){
			ParserStackPtr topSymtable = (*symtableStack);
			while(topSymtable != NULL){
				
				SymTableBinTreePtr currentLocalSymtable = STACK_DATA_TO_SYMTABLE(parserStackPeek(&topSymtable));
				
				SymTableData data;
				if(symTableSearch(currentLocalSymtable, currentToken.attribute.string, &data, errorHandle) == 1){
					
					if(data.idDataType == INT){
						top->data.token.type = TOKEN_INTVALUE;
					} else if(data.idDataType == FLOAT){
						top->data.token.type = TOKEN_FLOATVALUE;
					} else if(data.idDataType == STRING){
						top->data.token.type = TOKEN_STRINGVALUE;
					}
					break;
				}
				
				topSymtable = topSymtable->next;
			}
			if(topSymtable == NULL){
				// SEMANTIC ERROR - MISSING VAR
				errorSet(SEM_UNDEFINED_VAR_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - MISSING VAR", __FILE__, __LINE__, errorHandle);
				return -1;
			}
		}
		
		top = top->next;
	}
	
	return 0;
}


Token_type parserSemanticExpressionCheckOperatorsAndOperands(ParserStackPtr *expressionStack, ErrorHandle *errorHandle)
{
	ParserStackPtr top = (*expressionStack);
	if(top->next != NULL){
		
		// COMPARE
		if(TOKEN_EQ <= top->next->data.token.type && top->next->data.token.type <= TOKEN_LTE){
			
			top = (*expressionStack);
			Token_type expressionTokenType = top->data.token.type;
			while(top != NULL){
				
				if(!((TOKEN_EQ <= top->data.token.type && top->data.token.type <= TOKEN_LTE) || (top->data.token.type == expressionTokenType))){
					// error type
					errorSet(SEM_TYPE_COMPATIBILITY_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - OPERANDS OR OPERATORS ARE NOT THE SAME TYPE", __FILE__, __LINE__, errorHandle);
					return TOKEN_EMPTY;
				}
				
				top = top->next;
			}
			
			top = (*expressionStack);
			return top->next->data.token.type;
			
		// ACTION
		} else if(TOKEN_ADD <= top->next->data.token.type && top->next->data.token.type <= TOKEN_DIV){
			
			Token_type prevTokenType = TOKEN_EMPTY;
			
			// MATH INT
			if(top->data.token.type == TOKEN_INTVALUE){
				
				top = (*expressionStack);
				Token_type expressionTokenType = top->data.token.type;
				while(top != NULL){
					
					if(!((TOKEN_ADD <= top->data.token.type && top->data.token.type <= TOKEN_DIV) || (top->data.token.type == expressionTokenType))){
						// error type
						errorSet(SEM_TYPE_COMPATIBILITY_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - OPERANDS OR OPERATORS ARE NOT THE SAME OR THE RIGHT TYPE", __FILE__, __LINE__, errorHandle);
						return TOKEN_EMPTY;
					} else if(prevTokenType == TOKEN_DIV && top->data.token.attribute.integer == 0){
						errorSet(ZERO_DIVISION_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - OPERANDS OR OPERATORS ARE NOT THE SAME OR THE RIGHT TYPE", __FILE__, __LINE__, errorHandle);
						return TOKEN_EMPTY;
					}
					
					prevTokenType = top->data.token.type;
					top = top->next;
				}
				return expressionTokenType;
				
			// MATH FLOAT
			} else if(top->data.token.type == TOKEN_FLOATVALUE){
				
				top = (*expressionStack);
				Token_type expressionTokenType = top->data.token.type;
				while(top != NULL){
					
					if(!((TOKEN_ADD <= top->data.token.type && top->data.token.type <= TOKEN_DIV) || (top->data.token.type == expressionTokenType))){
						// error type
						errorSet(SEM_TYPE_COMPATIBILITY_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - OPERANDS OR OPERATORS ARE NOT THE SAME OR THE RIGHT TYPE", __FILE__, __LINE__, errorHandle);
						return TOKEN_EMPTY;
					} else if(prevTokenType == TOKEN_DIV && top->data.token.attribute.real == 0.0){
						errorSet(ZERO_DIVISION_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - OPERANDS OR OPERATORS ARE NOT THE SAME OR THE RIGHT TYPE", __FILE__, __LINE__, errorHandle);
						return TOKEN_EMPTY;
					}
					
					prevTokenType = top->data.token.type;
					top = top->next;
				}
				return expressionTokenType;
				
			// STRING
			} else if(top->data.token.type == TOKEN_STRINGVALUE){
				
				top = (*expressionStack);
				Token_type expressionTokenType = top->data.token.type;
				while(top != NULL){
					
					if(!((TOKEN_ADD == top->data.token.type) || (top->data.token.type == expressionTokenType))){
						// error type
						errorSet(SEM_TYPE_COMPATIBILITY_ERROR, "PARSER_ANALYZE: SEMANTIC_ERROR - OPERANDS OR OPERATORS ARE NOT THE SAME OR THE RIGHT TYPE", __FILE__, __LINE__, errorHandle);
						return TOKEN_EMPTY;
					}
					
					top = top->next;
				}
				return expressionTokenType;
				
			} else {
				errorSet(INTERNAL_ERROR, "PARSER_ANALYZE: INTERNAL_ERROR_2", __FILE__, __LINE__, errorHandle);
				return TOKEN_EMPTY;
			}
			
		} else {
			errorSet(INTERNAL_ERROR, "PARSER_ANALYZE: INTERNAL_ERROR_1", __FILE__, __LINE__, errorHandle);
			return TOKEN_EMPTY;
		}
	} else {
		return top->data.token.type;
	}
	
	return TOKEN_EMPTY;
}


IDdataType parserSemanticTokenTypeToVarType(Token_type tokenType)
{
	IDdataType varDataType = NIL;
	if(tokenType == TOKEN_INTVALUE){
		varDataType = INT;
	} else if(tokenType == TOKEN_FLOATVALUE){
		varDataType = FLOAT;
	} else if(tokenType == TOKEN_STRINGVALUE){
		varDataType = STRING;
	}
	return varDataType;
}


Token parserSemanticVarTypeToToken(IDdataType varDataType)
{
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	if(varDataType == INT){
		currentToken.type = TOKEN_INTVALUE;
	} else if(varDataType == FLOAT){
		currentToken.type = TOKEN_FLOATVALUE;
	} else if(varDataType == STRING){
		currentToken.type = TOKEN_STRINGVALUE;
	}
	return currentToken;
}


int parserTokenListFree(TokenList *tokenList)
{
	if(tokenList != NULL){
		TokenListElementPtr first = tokenList->first;
		TokenListElementPtr destroy;
		while(first != NULL){
			destroy = first;
			first = first->rightPtr;
			free(destroy);
		}
		tokenList->first = NULL;
		tokenList->active = NULL;
		tokenList->last = NULL;
		return ALL_OK;
	}
	return INTERNAL_ERROR;
}


int parserLeftAnalysis(int ruleNumber, ParserStackPtr *leftAndRightAnalysisRuleStack)
{
	// Levý rozbor (číslováno od 1)
	//printf("Lr: %d\n", ruleNumber);
	
	parserStackPush(leftAndRightAnalysisRuleStack, STACK_INT_TO_DATA(ruleNumber));
	
	return 0;
}


int parserRightAnalysis(int ruleNumber, ParserStackPtr *leftAndRightAnalysisRuleStack)
{
	// Pravý rozbor (číslováno od 1)
	//printf("Rr: %d\n", ruleNumber);
	
	parserStackPush(leftAndRightAnalysisRuleStack, STACK_INT_TO_DATA(ruleNumber));
	
	return 0;
}


/****************************************************** PRECEDENT SYNTAX STACK ******************************************************************************/

ParserStackData parserStackPrecedentTop(ParserStackPtr *stack)
{
	ParserStackPtr top = (*stack);
	while(top != NULL && (!IS_TERM(STACK_DATA_TO_TERM(top->data)) || (STACK_DATA_TO_TERM(top->data)) == TERM_PSEUDO_HANDLE)){
		top = top->next;
	}
	if(top != NULL){
		return top->data;
	}
	return STACK_INT_TO_DATA(-1);
}

int parserStackPrecedentTopAddHandle(ParserStackPtr *stack)
{
	ParserStackPtr prevTop = (*stack);
	ParserStackPtr top = (*stack);
	while(top != NULL && !IS_TERM(STACK_DATA_TO_TERM(top->data))){
		prevTop = top;
		top = top->next;
	}
	if(top == prevTop){
		return STACK_DATA_TO_INT(parserStackPush(stack, STACK_TERM_TO_DATA(TERM_PSEUDO_HANDLE)));
	} else {
		ParserStackPtr item = malloc(sizeof(struct parserStackNode));
		if(item != NULL){
			item->data = STACK_TERM_TO_DATA(TERM_PSEUDO_HANDLE);
			item->next = top;
			prevTop->next = item;
			return 0;
		}
	}
	return -1;
}

int parserStackPrecedentTopHasHandle(ParserStackPtr *stack)
{
	return (STACK_DATA_TO_INT(parserStackPrecedentTop(stack)) != -1);
}

int parserStackPrecedentTopPopAndPushRule(ParserStackPtr *stack, ParserStackPtr *semanticPrecedentStack, ParserStackPtr *expressionRuleStack)
{
	ParserStackPtr ruleStack;
	parserStackInit(&ruleStack);
		
	// pop
	ParserStackPtr top = (*stack);
	while(top != NULL && (STACK_DATA_TO_TERM(top->data)) != TERM_PSEUDO_HANDLE){
		parserStackPush(&ruleStack, top->data);
		
		if(STACK_DATA_TO_TOKEN(parserStackPeek(semanticPrecedentStack)).type != TOKEN_EMPTY){
			parserStackPush(expressionRuleStack, parserStackPeek(semanticPrecedentStack));
		}
		parserStackPop(semanticPrecedentStack);
		
		top = top->next;
		parserStackPop(stack);
	}
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;
	parserStackPush(semanticPrecedentStack, STACK_TOKEN_TO_DATA(currentToken));
	
	if(top != NULL && (STACK_DATA_TO_TERM(top->data)) == TERM_PSEUDO_HANDLE){
		parserStackPop(stack);
		int found = 0;
		int i, j = 0;
		
		// find
		for(i = PREC_GRAMM_RULES_START-1; i < PREC_GRAMM_RULES_END && found == 0; i++)
		{
			top = ruleStack;
			j = 0;
			
			while(j < GRAMMAR_RULE_LIST__ROW_MAX_SIZE && found == 0){
				
				if(STACK_DATA_TO_TERM(parserStackPeek(&top)) == -1){
					if(GrammmarRuleList[i][j] != 0){
						found = 0;
					} else {
						found = 1;
					}
					break;
				} else if(STACK_DATA_TO_TERM(parserStackPeek(&top)) != GrammmarRuleList[i][j]){
					found = 0;
					break;
				}
				
				j++;
				top = top->next;
			}
			if(found == 1){
				break;
			}
		}
		
		parserStackFree(&ruleStack);
		
		if(found == 1){
			found = i;
			
			// push
			parserStackPush(stack, STACK_TERM_TO_DATA(GrammmarExprLeftRuleList[PREC_GRAMM_RULES_TO_RULE(found)]));
			
			return found+1;
		}
		return -2;
	}
	parserStackFree(&ruleStack);
	return -1;
}


/****************************************************** SYNTAX STACK ******************************************************************************/

void parserStackInit(ParserStackPtr *stack)
{
	(*stack) = NULL;
}

ParserStackData parserStackPush(ParserStackPtr *stack, ParserStackData data)
{
    ParserStackPtr item = malloc(sizeof(struct parserStackNode));
	if(item != NULL){
		item->data = data;
		item->next = (*stack);
		(*stack) = item;
		return STACK_INT_TO_DATA(0);
	}
	return STACK_INT_TO_DATA(-1);
}

ParserStackData parserStackPeek(ParserStackPtr *stack)
{
	if((*stack) != NULL){
		return (*stack)->data;
	}
	return STACK_INT_TO_DATA(-1);
}

ParserStackData parserStackPop(ParserStackPtr *stack)
{
	ParserStackPtr top = (*stack);
	if(top != NULL){
		(*stack) = top->next;
		ParserStackData data = top->data;
		free(top);
		return data;
	}
	return STACK_INT_TO_DATA(-1);
}

void parserStackFree(ParserStackPtr *stack)
{
	while(STACK_DATA_TO_INT(parserStackPop(stack)) >= 0){}
	parserStackInit(stack);
}
