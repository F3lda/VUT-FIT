/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: generator.c
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */

#include "generator.h"



ParserStackPtr variableStack = NULL;
ParserStackPtr argumentStack = NULL;
integerStack ifForStack = NULL;


void generatorFree()
{
	parserStackFree(&variableStack);
	parserStackFree(&argumentStack);
	integerStackFree(&ifForStack);
}

IDdataType getIDdatatype(ParserStackPtr *symtableStack, char *variable, ErrorHandle *errorHandle){
		ParserStackPtr topSymtable = (*symtableStack);
		while(topSymtable != NULL){
			
			SymTableBinTreePtr currentLocalSymtable = STACK_DATA_TO_SYMTABLE(parserStackPeek(&topSymtable));
			
			SymTableData data;
			if(symTableSearch(currentLocalSymtable, variable, &data, errorHandle) == 1){
				return data.idDataType;
			}
			topSymtable = topSymtable->next;
		}
		return NIL;
}

void integerStackInit(integerStack *stack)
{
	(*stack) = NULL;
}

void integerStackPush(integerStack *stack, int ID, int keywordType)
{
    integerStack item = malloc(sizeof(struct integerStack));
	if(item != NULL){
		item->ID = ID;
		item->keywordType = keywordType;
		item->bracketCount = 0;
		item->next = (*stack);
		(*stack) = item;
		//printf("PUSHING: %d\n", item->ifID);
	}
}

int IntegerStackPeekID(integerStack *stack)
{
	if((*stack) != NULL){
		return (*stack)->ID;
	}
	return -1;
}

int IntegerStackPeekKeywordType(integerStack *stack)
{
	if((*stack) != NULL){
		return (*stack)->keywordType;
	}
	return -1;
}

int IntegerStackSetKeywordType(integerStack *stack, int keywordType)
{
	(*stack)->keywordType = keywordType;
}

int IntegerStackPeekBrackets(integerStack *stack)
{
	if((*stack) != NULL){
		return (*stack)->bracketCount;
	}
	return -1;
}

void IntegerStackIncreaseBrackets(integerStack *stack)
{
	if((*stack) != NULL){
		(*stack)->bracketCount++;
		return;
	}
	return;
}

int integerStackPop(integerStack *stack)
{
	integerStack top = (*stack);
	if(top != NULL){
		(*stack) = top->next;
		int returned = top->ID;
		free(top);
		//printf("POPPING: %d", returned);
		return returned;
	}
	return -1;
}

void integerStackFree(integerStack *stack)
{
	int top;
	top = integerStackPop(stack);
	while(top >= 0){
		top = integerStackPop(stack);
	}
	integerStackInit(stack);
}

ParserStackPtr reverseStack(ParserStackPtr *varStack){
	ParserStackPtr tempStack = NULL;
	ParserStackData top;
	if (varStack == NULL) return NULL;

	while(STACK_DATA_TO_INT(parserStackPeek(varStack)) != -1){
		top = parserStackPop(varStack);
		parserStackPush(&tempStack, top);
	}
	return tempStack;
}

void preorderTreeParamsTraversal(SymTableBinTreePtr TempTree)
{
	if (TempTree == NULL) 
        return; 

	if(strcmp(TempTree->key, "_")){
		printf("\nDEFVAR LF@%s\nPOPS LF@%s\n", TempTree->key, TempTree->key);
	}
    //printf("PARAM: %s \n", TempTree->key);   

    preorderTreeParamsTraversal(TempTree->leftPtr);   
    preorderTreeParamsTraversal(TempTree->rightPtr); 
}

void pushArguments(ParserStackPtr *argStack, int argCount){
	ParserStackData data;
	printf("\n");
	for(int i = 0; i < argCount; i++){
		data = parserStackPop(argStack);
		if(data.token.type == TOKEN_INTVALUE){
            printf("PUSHS int@%ld\n", data.token.attribute.integer);
		}
		else if (data.token.type == TOKEN_FLOATVALUE){
			printf("PUSHS float@%a\n", data.token.attribute.real);
		}
		else if (data.token.type == TOKEN_STRINGVALUE){
			printf("PUSHS string@%s\n", data.token.attribute.string);
		}
		else if(data.token.type == TOKEN_ID){
			printf("PUSHS LF@%s\n", data.token.attribute.string);
		}
	}
}

void printPops(ParserStackPtr *varStack, int varCount){
	ParserStackData data;
	Token variableToken[varCount];
	//reverseStack(varStack, varCount);
	
	*varStack = reverseStack(varStack);
	while(STACK_DATA_TO_INT(parserStackPeek(varStack)) != -1){
		data = parserStackPop(varStack);
		if(!strcmp(data.token.attribute.string,"_")){
			printf("POPS GF@_\n");
		}
		else{
			printf("POPS LF@%s\n", data.token.attribute.string);
		}
	}
}

void printString(char* string, bool printNewLine){
	for (int i = 0; string[i] != '\0'; i++){
		if(string[i] <= 32)
		{
			if(string[i] < 10){
				printf("\\00%d", string[i]);
			}
			else{
				printf("\\0%d", string[i]);
			}
		}
		else if (string[i] == 35) printf("\\035");
		else if (string[i] == 92) printf("\\092");
		else {
			printf("%c", string[i]);
		}
	}
	if(printNewLine == true){
		printf("\n");
	}
}


int generatorGenerateCode(TokenList *tokenList, ParserStackPtr *symtableStack, SymTableBinTreePtr *globalSymTable, ParserStackPtr *leftAndRightAnalysisStack, ParserStackPtr *semanticRuleStack, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}
	
	
	Token currentToken;
	currentToken.type = TOKEN_EMPTY;
	currentToken.pos_line = 0;
	currentToken.pos_number = 0;

	Token previousToken;
	previousToken.type = TOKEN_EMPTY;
	previousToken.pos_line = 0;
	previousToken.pos_number = 0;
	
	
	static int bracketCnt = 0;
	static int variableCount = 0; 
	static int argCount = 0;
	static int ifCount = 0;
	static int forCount = 0;

	//pomocné proměnné na uchovávání kontextu aktuální pozices
	static bool inFunction = false;
	static char *inFunctionName = NULL;
	static char *inFunctionCallName = NULL;
	static bool inExpression = false;
	static bool inPrint = false;
	static bool inMain = false;
	static bool multiExpression = false;
	static bool leftSide = false;
	static bool inArguments = false;
	static bool inFunctionCall = false;
	static bool isStringExpression = false;
	static bool inMultiAssign = false;
	static bool inReturn = false;
	static bool inIfExpression = false;
	static bool inForDefinition = false;
	static bool inForExpression = false;
	static bool inForAssignment = false;
	static bool inForBody = false;
	static bool inElse = false;
	static bool inIf = false;
	static bool singleExpression = false;

	//Vestavěné funkce
	static bool inputi = false;
	static bool inputf = false;
	static bool inputs = false;
	static bool int2float = false;
	static bool float2int = false;
	static bool len = false;
	static bool substr = false;
	static bool ord = false;
	static bool chr = false;
	static bool greaterOrEqual = false;
	static bool lessOrEqual = false;


	static int expressionCounter = 0;
	static char* currentVariableID;
	static Token_type expressionType = TOKEN_EMPTY;
	
	// get current and previous Token
	scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
	errorHandleInit(errorHandle);
	scannerTokenListGetActive(tokenList, &currentToken, errorHandle);
	
	//printf("\n\nVolam generatorGenerateCode:\n\n");

	//printf("TOKEN: %s \n", tokenTypes[currentToken.type]);
	//if(currentToken.type == TOKEN_ID) {printf(", NAME: %s", currentToken.attribute.string);}
	//printf("\n");
	//printf("Variable ID: %s\n", currentVariableID);
	static int grammarRule = 0;

	// GENEROVÁNÍ PODLE PRAVIDEL
	while(STACK_DATA_TO_INT(parserStackPeek(leftAndRightAnalysisStack)) >= 0){
		grammarRule = STACK_DATA_TO_INT(parserStackPop(leftAndRightAnalysisStack));
		//printf("  rule: %d\n", grammarRule);
	
		// ZAČÁTEK SOUBORU (PROLOG)
		if(grammarRule == 1){
			printf(".IFJcode20\nJUMP main\n\n");
		}
		
		
		// DEFINICE FUNKCE
		if(grammarRule == 2){
			//printf("\n# -----func %s--------------------------------\n", currentToken.attribute.string);
			//printf("LABEL %s\n\n", currentToken.attribute.string);
			//printf("CREATEFRAME\nPUSHFRAME\n\n");
			inFunction = true;
		}

		if(grammarRule == 16){
			if(currentToken.type == TOKEN_ID) leftSide = true;
			if(!strcmp(currentToken.attribute.string,"print")) inPrint = true;		
		}

		if(grammarRule == 28){
			IntegerStackSetKeywordType(&ifForStack, FOR_INIT);
		}

		if(grammarRule == 34 || grammarRule == 36){
			inFunctionCall = true;
		}

		if(grammarRule == 39){
			if(inReturn){
				printf("\n\nPOPFRAME\nRETURN\n");
				inReturn = false;
			}
			
			//TISKNE POSLEDNI PRVEK
		}

		if(grammarRule == 38 || grammarRule == 39){
			
			if(inMultiAssign == true && variableCount != 1){
				ParserStackData data;
				Token variableToken;
				data = parserStackPop(&variableStack);
				variableToken = data.token;
				if(variableToken.type == TOKEN_ID || variableToken.type == TOKEN_STRINGVALUE){
					if(!strcmp(variableToken.attribute.string,"_")){
						printf("POPS GF@_\n");
					}
					else{
						printf("POPS LF@%s\n", variableToken.attribute.string);
					}
				}
			}
		}
		
		// VÝRAZY
		// VÍCE OPERANDŮ
		if(52 <= grammarRule && grammarRule <= 61){
			multiExpression = true;
			int currentKeyword = IntegerStackPeekKeywordType(&ifForStack);
			if(inExpression == false || currentKeyword == FOR_ASSIGNMENT){
				expressionType = TOKEN_EMPTY;
				printf("\nCREATEFRAME\n");
				printf("DEFVAR TF@$operand1\n");
				printf("DEFVAR TF@$operand2\n");
				printf("DEFVAR TF@$result\n\n");
			}
			inExpression = true;
			
			if(semanticRuleStack != NULL){
				Token operator; // OPERATOR
				operator.type = TOKEN_EMPTY;
				Token operand2; // OPERAND 2
				operand2.type = TOKEN_EMPTY;
				Token operand1; // OPERAND 1
				operand1.type = TOKEN_EMPTY;
				int tokenCount = 0;
				
				int operandFromBrackets = 0;
				
				ParserStackPtr top = (*semanticRuleStack);
				while(top != NULL && tokenCount < 3){ // načte max první tři terminály
					//printf("\n\n    EXPRESSION: %s\n", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
					if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_LROUNDBRACKET && STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type != TOKEN_RROUNDBRACKET){
						if(tokenCount == 0){
							operator = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
						} else if(operandFromBrackets == 0){
							if(tokenCount == 1){
								operand2 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand2.type;
							} else if(tokenCount == 2){
								operand1 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand1.type;
							}
						} else if(operandFromBrackets == 1){
							if(tokenCount == 1){
								operand1 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand1.type;
							} else if(tokenCount == 2){
								operand2 = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
								expressionType = operand2.type;
							}
						}
						
						tokenCount++;
					} else if(STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type == TOKEN_LROUNDBRACKET){
						operandFromBrackets = 1;
					}

					if(operand1.type == TOKEN_STRINGVALUE || operand2.type == TOKEN_STRINGVALUE){
						isStringExpression = true;
					}

					top = top->next;
					parserStackPop(semanticRuleStack);
				}
				//printf("OPERAND 2 TOKEN: %s\n", tokenTypes[operand2.type]);
				//printf("OPERAND 1 TOKEN: %s\n", tokenTypes[operand1.type]);
					int currentKeyword = IntegerStackPeekKeywordType(&ifForStack);
					if(operand2.type == TOKEN_EMPTY){
						printf("POPS TF@$operand2\n");
					}
					if(operand1.type == TOKEN_EMPTY){
						printf("POPS TF@$operand1\n");
					}
					//Vypsání instrukcí pro GTE a LTE
					if(operator.type == TOKEN_GTE){
						greaterOrEqual = true;			
						printf("PUSHS");
					}
					else if(operator.type == TOKEN_LTE){
						lessOrEqual = true;
						printf("PUSHS");
					}
					else{
						// Vypsání instrukcí mimo LTE a GTE
						// TISK INSTRUKCI VYRAZU
						if(operator.type == TOKEN_MUL){
							printf("MUL TF@$result");
						}
						else if (operator.type == TOKEN_DIV && expressionType == TOKEN_FLOATVALUE){
							printf("DIV TF@$result");
						}
						else if (operator.type == TOKEN_DIV && expressionType == TOKEN_INTVALUE){
							printf("IDIV TF@$result");
						}
						else if(operator.type == TOKEN_ADD){
							if(operand1.type == TOKEN_ID){
								if(getIDdatatype(symtableStack, operand1.attribute.string, errorHandle) == STRING){
									printf("CONCAT TF@$result");
								}else{
								printf("ADD TF@$result");
								}
							}else if(operand2.type == TOKEN_ID){
								if(getIDdatatype(symtableStack, operand2.attribute.string, errorHandle) == STRING){
									printf("CONCAT TF@$result");
								}else{
								printf("ADD TF@$result");
								}
							}
							else if(isStringExpression){
								printf("CONCAT TF@$result");
							}else{
							printf("ADD TF@$result");
							}
							
						}
						else if(operator.type == TOKEN_SUB){
							printf("SUB TF@$result");
						}
						else if(operator.type == TOKEN_LT){
							printf("LT TF@$result");
						}
						else if(operator.type == TOKEN_GT){
							printf("GT TF@$result");
						}
						else if(operator.type == TOKEN_EQ){
							printf("EQ TF@$result");
						}
						else if(operator.type == TOKEN_NEQ){
							printf("EQ TF@$result");
						}
						
					}
					//TISK OPERANDŮ
					// OPERAND 1
					if(operand1.type == TOKEN_INTVALUE) {
						printf(" int@%ld", operand1.attribute.integer);
					}
					else if(operand1.type == TOKEN_FLOATVALUE) {
						printf(" float@%a", operand1.attribute.real);
					}
					else if(operand1.type == TOKEN_STRINGVALUE) {
						printf(" string@");
						printString(operand1.attribute.string, false);
					}
					else if(operand1.type == TOKEN_ID){
						printf(" LF@%s", operand1.attribute.string);
					}
					else if(operand1.type == TOKEN_EMPTY){
						printf(" TF@$operand1");
					}

					if(operator.type == TOKEN_GTE || operator.type == TOKEN_LTE){
						printf("\nPUSHS");
					}

					// OPERAND 2
					if(operand2.type == TOKEN_INTVALUE){
						printf(" int@%ld\n", operand2.attribute.integer);
					} 
					else if(operand2.type == TOKEN_FLOATVALUE){
						printf(" float@%a\n", operand2.attribute.real);
					}
					else if(operand2.type == TOKEN_STRINGVALUE) {
						printf(" string@");
						printString(operand2.attribute.string, true);
					}
					else if(operand2.type == TOKEN_ID){
						printf(" LF@%s\n", operand2.attribute.string);
					}
					else if(operand2.type == TOKEN_EMPTY){
						printf(" TF@$operand2\n");
					}

					if(operator.type == TOKEN_NEQ){
						printf("\nNOT TF@$result TF@$result\n");
					}

					if(operator.type == TOKEN_GTE){
						printf("\nCALL $greaterOrEqual");
					}
					else if(operator.type == TOKEN_LTE){
						printf("\nCALL $lessOrEqual");
					}
					else if(currentKeyword == FOR_EXPRESSION){
						printf("\n");
					}
					else{
						printf("PUSHS TF@$result\n\n");
					}
		}
		// POUZE 1 OPERAND
		} else if(grammarRule > 61 && currentToken.type == TOKEN_EOL){
			if(semanticRuleStack != NULL){
				ParserStackPtr top = (*semanticRuleStack);
				//printf("\nreturn = %d\n",inReturn);
				if(top != NULL && top->next == NULL && inExpression == false){
					//printf("\n\nSINGLE EXPRESSION: %s", tokenTypes[STACK_DATA_TO_TOKEN(parserStackPeek(&top)).type]);
							
					Token expressionValue;
					expressionValue.type = TOKEN_EMPTY;
					expressionValue = STACK_DATA_TO_TOKEN(parserStackPeek(&top));
					

					if(inReturn == false && currentVariableID != NULL){
						if(!strcmp(currentVariableID,"_")){
							
							printf("MOVE GF@_ ");
						}
						else{
							printf("MOVE LF@%s ", currentVariableID);
						}
						
						currentVariableID = NULL; //////////////////////////////////toto jsem přidal + podmínku currentVariableID != NULL

						
						if(expressionValue.type == TOKEN_INTVALUE){
							printf("int@%ld\n", expressionValue.attribute.integer);
						} 
						else if(expressionValue.type == TOKEN_FLOATVALUE){
							printf("float@%a\n", expressionValue.attribute.real);
						}
						else if(expressionValue.type == TOKEN_STRINGVALUE) {
							printf("string@");
							printString(expressionValue.attribute.string, true);
						}
						else if(expressionValue.type == TOKEN_ID){
							printf("LF@%s\n", expressionValue.attribute.string);
						}
					}
					top = top->next;
					parserStackPop(semanticRuleStack);
				}
			}
		}
	}
	
	if(previousToken.type == TOKEN_EOL && inExpression == true && grammarRule < 49){
		if(inMultiAssign == false && inReturn == false && currentVariableID != NULL){
			if(!strcmp(currentVariableID,"_")){
				printf("POPS GF@_\n\n");
			}
			else{
				printf("POPS LF@%s\n\n", currentVariableID);
			}
			currentVariableID = NULL;
		}
		int currentKeyword = IntegerStackPeekKeywordType(&ifForStack);
		if(currentKeyword == IF_EXPRESSION){
			IntegerStackSetKeywordType(&ifForStack, IF);
			printf("\nPOPS TF@$result\n\nJUMPIFEQ $%s_if_else%d TF@$result bool@false\n", inFunctionName, IntegerStackPeekID(&ifForStack));
		}
		if(currentKeyword != FOR_ASSIGNMENT){
			inExpression = false;
		}
		else if(currentKeyword == FOR_ASSIGNMENT){
			IntegerStackSetKeywordType(&ifForStack, FOR);
			printf("JUMP $for_%d_start\n\nLABEL $for_%d_assignment_end\n\n", IntegerStackPeekID(&ifForStack), IntegerStackPeekID(&ifForStack));
		}
	}

	//
	if(previousToken.type == TOKEN_EOL){
		multiExpression = false;
		inMultiAssign = false;
		inReturn = false;
		variableCount = 0;
		parserStackFree(&variableStack);
		parserStackFree(&argumentStack);
	}	

	// GENEROVANÍ PODLE TOKENŮ
	switch (currentToken.type)
	{
		
		case TOKEN_ASSIGN:
			leftSide = false;
			variableStack = reverseStack(&variableStack);
			if(variableCount > 1){
				inMultiAssign = true;
			}
			else{
				currentVariableID = previousToken.attribute.string;
			}

			break;
		case TOKEN_INIT:
			leftSide = false;
			scannerTokenListGetPrev(tokenList, &previousToken, errorHandle);
			printf("DEFVAR LF@%s\n", previousToken.attribute.string);
			currentVariableID = previousToken.attribute.string;
			
			break;

		

		case TOKEN_STRINGVALUE:

			if(inPrint == true){
				printf("\nWRITE string@");
				printString(currentToken.attribute.string, true);
			}
			if(inArguments == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %s\n", data.token.attribute.string);
				parserStackPush(&argumentStack, data);
			}
			if(inReturn == true && multiExpression == false  && (grammarRule < 51 ) /*&& grammarRule != 38*/){
				Token tmp;
				int shiftCount = 0;
				scannerTokenListGetNext(tokenList, &tmp, errorHandle);
				if(tmp.type == TOKEN_COMMA  || tmp.type == TOKEN_EOL){
					printf("\nPUSHS string@%s\n", currentToken.attribute.string);
				}
				//ošetření, když je jediný token v závorkách
				else if(tmp.type == TOKEN_RROUNDBRACKET){//TOTO JSEM PŘIDAL
					while(tmp.type == TOKEN_RROUNDBRACKET){
						scannerTokenListMoveNext(tokenList, errorHandle);
						shiftCount++;
						scannerTokenListGetActive(tokenList, &tmp, errorHandle);
					}
					if(tmp.type == TOKEN_COMMA || tmp.type == TOKEN_EOL){
						printf("\nPUSHS string@%s\n", currentToken.attribute.string);
					}
					for (int i = 0; i < shiftCount; i++)
					{
						scannerTokenListMovePrev(tokenList, errorHandle);
					}
					scannerTokenListGetActive(tokenList, &tmp, errorHandle);
				}
			}
			break;

		case TOKEN_INTVALUE:
			if(inPrint == true){
				printf("\nWRITE int@%ld\n", currentToken.attribute.integer);
			}
			if(inArguments == true && inFunctionCall == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %ld\n", currentToken.attribute.integer);
				parserStackPush(&argumentStack, data);
			}
			if(inReturn == true && multiExpression == false  && (grammarRule < 51 )/* && grammarRule != 38*/){
				Token tmp;
				int shiftCount = 0;
				scannerTokenListGetNext(tokenList, &tmp, errorHandle);
				if(tmp.type == TOKEN_COMMA  || tmp.type == TOKEN_EOL){
					printf("\nPUSHS int@%ld\n", currentToken.attribute.integer);
				}
				//ošetření, když je jediný token v závorkách
				else if(tmp.type == TOKEN_RROUNDBRACKET){//TOTO JSEM PŘIDAL
					while(tmp.type == TOKEN_RROUNDBRACKET){
						scannerTokenListMoveNext(tokenList, errorHandle);
						shiftCount++;
						scannerTokenListGetActive(tokenList, &tmp, errorHandle);
					}
					if(tmp.type == TOKEN_COMMA || tmp.type == TOKEN_EOL){
						printf("\nPUSHS int@%ld\n", currentToken.attribute.integer);
					}
					for (int i = 0; i < shiftCount; i++)
					{
						scannerTokenListMovePrev(tokenList, errorHandle);
					}
					scannerTokenListGetActive(tokenList, &tmp, errorHandle);
				}
			}
			break;

		case TOKEN_FLOATVALUE:
			if(inPrint == true){
				printf("\nWRITE float@%a\n", currentToken.attribute.real);
			}
			if(inArguments == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %f\n", data.token.attribute.real);
				parserStackPush(&argumentStack, data);
			}
			if(inReturn == true && multiExpression == false && (grammarRule < 51 )/* && grammarRule != 38*/){
				Token tmp;
				int shiftCount = 0;
				scannerTokenListGetNext(tokenList, &tmp, errorHandle);
				if(tmp.type == TOKEN_COMMA || tmp.type == TOKEN_EOL/*|| tmp.type == TOKEN_RROUNDBRACKET*/){
					printf("\nPUSHS float@%a\n", currentToken.attribute.real);
				}
				//ošetření, když je jediný token v závorkách
				else if(tmp.type == TOKEN_RROUNDBRACKET){//TOTO JSEM PŘIDAL
					while(tmp.type == TOKEN_RROUNDBRACKET){
						scannerTokenListMoveNext(tokenList, errorHandle);
						shiftCount++;
						scannerTokenListGetActive(tokenList, &tmp, errorHandle);
					}
					if(tmp.type == TOKEN_COMMA || tmp.type == TOKEN_EOL){
						printf("\nPUSHS float@%a\n", currentToken.attribute.real);
					}
					for (int i = 0; i < shiftCount; i++)
					{
						scannerTokenListMovePrev(tokenList, errorHandle);
					}
					scannerTokenListGetActive(tokenList, &tmp, errorHandle);
				}
			}
			
			break;
		case TOKEN_COMMA:
			multiExpression = false;
		break;

		case TOKEN_ID:
	
			// TOKEN SE JMÉNEM FUNKCE
			if(inFunction == true && inFunctionName == NULL){
				inFunctionName = currentToken.attribute.string;
				printf("# --- func %s ------------------------------\nLABEL %s\nCREATEFRAME\nPUSHFRAME\n\n", inFunctionName, inFunctionName);
				if(!strcmp(inFunctionName, "main")) printf("DEFVAR GF@_\n\n");
				SymTableData data;
				SymTableData varData;
				int i = 0;
				symTableSearch(*globalSymTable, inFunctionName, &data, errorHandle);
				preorderTreeParamsTraversal(data.functionLocalSymTable);

			}
			

			if(inArguments == true){
				ParserStackData data;
				data.token = currentToken;
				argCount++;
				//printf("\nPUSHING: %s\n", data.token.attribute.string);
				parserStackPush(&argumentStack, data);
			}

			// PUSHujeme do variableStacku proměnné na levé straně
			if(leftSide == true && previousToken.type != TOKEN_KEYWORD_FUNC){
				ParserStackData tokenData;
				tokenData.token = currentToken;
				variableCount++;
				//printf("\nPUSHING: %s\n, VariableCount: %d", tokenData.token.attribute.string, variableCount);
				parserStackPush(&variableStack, tokenData);
			}

			if(inReturn == true && multiExpression == false && (grammarRule < 51 )/* && grammarRule != 38*/){
				Token tmp;
				int shiftCount = 0;
				scannerTokenListGetNext(tokenList, &tmp, errorHandle);
				if(tmp.type == TOKEN_COMMA  || tmp.type == TOKEN_EOL){
					printf("\nPUSHS LF@%s\n", currentToken.attribute.string);
				}
				//ošetření, když je jediný token v závorkách
				else if(tmp.type == TOKEN_RROUNDBRACKET){//TOTO JSEM PŘIDAL
					while(tmp.type == TOKEN_RROUNDBRACKET){
						scannerTokenListMoveNext(tokenList, errorHandle);
						shiftCount++;
						scannerTokenListGetActive(tokenList, &tmp, errorHandle);
					}
					if(tmp.type == TOKEN_COMMA || tmp.type == TOKEN_EOL){
						printf("\nPUSHS LF@%s\n", currentToken.attribute.string);
					}
					for (int i = 0; i < shiftCount; i++)
					{
						scannerTokenListMovePrev(tokenList, errorHandle);
					}
					scannerTokenListGetActive(tokenList, &tmp, errorHandle);
				}
			}

			//BUILT IN FUNKCE
			if(inPrint == true && strcmp(currentToken.attribute.string,"print")){
				printf("\nWRITE LF@%s\n", currentToken.attribute.string);
			}
			if(!strcmp(currentToken.attribute.string, "inputi")){inputi = true;}
			if(!strcmp(currentToken.attribute.string, "inputf")){inputf = true;}
			if(!strcmp(currentToken.attribute.string, "inputs")){inputs = true;}
			if(!strcmp(currentToken.attribute.string, "int2float")){int2float = true;}
			if(!strcmp(currentToken.attribute.string, "float2int")){float2int = true;}
			if(!strcmp(currentToken.attribute.string, "len")){len = true;}
			if(!strcmp(currentToken.attribute.string, "substr")){substr = true;}
			if(!strcmp(currentToken.attribute.string, "ord")){ord = true;}
			if(!strcmp(currentToken.attribute.string, "chr")){chr = true;}
			
			break;
		
		case TOKEN_LCURLYBRACKET:
			
			bracketCnt++;
			int currentID = IntegerStackPeekID(&ifForStack);
			if(currentID != -1){	
				IntegerStackIncreaseBrackets(&ifForStack);
			}

			break;

		case TOKEN_RCURLYBRACKET:;
			// UKONČENÍ DEFINICE FUNKCE
			int currentKeywordType = IntegerStackPeekKeywordType(&ifForStack);
			

			bracketCnt--;
			if(currentKeywordType == ELSE){
				IntegerStackIncreaseBrackets(&ifForStack);
				int elseBrackets = IntegerStackPeekBrackets(&ifForStack);
				int tmp = IntegerStackPeekID(&ifForStack);
				//printf("\n%d\n",elseBrackets);
				if(elseBrackets == 4){
					printf("\n\nLABEL $if_end%d", tmp);
					integerStackPop(&ifForStack);
				}
			}
			if(currentKeywordType == FOR){
				IntegerStackIncreaseBrackets(&ifForStack);
				int forBrackets = IntegerStackPeekBrackets(&ifForStack);
				int tmp = IntegerStackPeekID(&ifForStack);
				if(forBrackets == 2){
					printf("\n\nJUMP $for_%d_assignment_start\n\nLABEL $for_%d_end\n\n", tmp, tmp);
					integerStackPop(&ifForStack);
				}
			}

			if(inFunction == true && bracketCnt == 0){
				if(strcmp(inFunctionName, "main") == 0){
					printf("\n\nPOPFRAME\nEXIT int@0\n\n");
					} else {
						printf("\n\nPOPFRAME\nRETURN\n\n");
					}
			
						
				inFunction = false;
				inFunctionName = NULL;

				if(inputi == true){//INPUT INT
					printf("\n%s", FUNC_INPUTI);
				}
				if(inputf == true){//INPUT FLOAT
					printf("\n%s", FUNC_INPUTF);
				}
				if(inputs == true){//INPUT STRING
					printf("\n%s", FUNC_INPUTS);
				}
				if(int2float == true){//INT2FLOAT
					printf("\n%s", FUNC_INT_TO_FLOAT);
				}
				if(float2int == true){//FLOAT2INT
					printf("\n%s", FUNC_FLOAT_TO_INT);
				}
				if(len == true){//STRING LENGTH
					printf("\n%s", FUNC_LEN);
				}
				if(substr == true){//SUBSTRING
					printf("\n%s", FUNC_SUBSTRING);
				}
				if(ord == true){//???
					printf("\n%s", FUNC_ORD);
				}
				if(chr == true){//???
					printf("\n%s", FUNC_CHR);
				}
				if(greaterOrEqual == true){//GTE
					printf("\n%s", FUNC_GTE);
				}
				if(lessOrEqual == true){//LTE
					printf("\n%s", FUNC_LTE);
				}
			}
			break;

		case TOKEN_EOL:
			break;
		
		case TOKEN_LROUNDBRACKET:
			if(inFunctionCall == true && previousToken.type == TOKEN_ID){
				inFunctionCallName = previousToken.attribute.string;
				inArguments = true;
			}
			break;

		case TOKEN_RROUNDBRACKET:
			
			if(inArguments == true && inPrint == false && strcmp(inFunctionCallName, "print") && inFunctionCall == true) {
				inArguments = false;
				inFunctionCall = false;
				pushArguments(&argumentStack, argCount);
				argCount = 0;
				printf("CALL %s\n", inFunctionCallName);
				if(variableCount != 0){
				printPops(&variableStack, variableCount);
				}
			}
			if(inPrint == true){
				inPrint = false;
			}
			break;

		case TOKEN_KEYWORD_RETURN:
			inReturn = true;
			break;

		case TOKEN_KEYWORD_IF:
			printf("\n\n#-------- if%d  -----------\n\n", ifCount);
			integerStackPush(&ifForStack, ifCount, IF);
			ifCount++;
			IntegerStackSetKeywordType(&ifForStack, IF_EXPRESSION);
			break; 

		case TOKEN_KEYWORD_ELSE:;
			int tmp = IntegerStackPeekID(&ifForStack);
			IntegerStackSetKeywordType(&ifForStack, ELSE);
			printf("\nJUMP $if_end%d", tmp);
			printf("\n\n#-------- else%d -----------\n\n", tmp);
			printf("\nLABEL $%s_if_else%d\n", inFunctionName, tmp);
			break;

		case TOKEN_KEYWORD_FOR:;

			printf("\n\n#-------- FOR%d  -----------\n\n", forCount);
			integerStackPush(&ifForStack, forCount, FOR);
			forCount++;
			break;

		case TOKEN_SEMICOLON:;
			int currentKeyword = IntegerStackPeekKeywordType(&ifForStack);
			if(currentKeyword == FOR_EXPRESSION){
				IntegerStackSetKeywordType(&ifForStack, FOR_ASSIGNMENT);
				printf("JUMPIFEQ $for_%d_end TF@$result bool@false\n\n", IntegerStackPeekID(&ifForStack));
				printf("\n\nJUMP $for_%d_assignment_end\n\nLABEL $for_%d_assignment_start\n\n", IntegerStackPeekID(&ifForStack), IntegerStackPeekID(&ifForStack));
			}
			if(currentKeyword == FOR_INIT){
				IntegerStackSetKeywordType(&ifForStack, FOR_EXPRESSION);
				printf("\nLABEL $for_%d_start\n", IntegerStackPeekID(&ifForStack));
			}
			
			break;
		
		default:
			break;
	}	
	
	
	return errorHandle->errorID;
}