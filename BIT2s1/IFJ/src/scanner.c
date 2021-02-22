/**
 * Název projektu: Implementace překladače imperativního jazyka IFJ20
 * soubor: scanner.c
 * autoři: 	Karel Jirgl (xjirgl01)
 * 			Michal Pyšík (xpysik00)
 * 			Václav Klem (xklemv00)
 * 			Thanh Quang Tran (xtrant02)
 */


#include "scanner.h"

//tento buffer neni deklarovany v .h souboru umyslne!! je jen pro mistni ucely
char strBuffer[2048];


// Stavy konecneho automatu scanneru
typedef enum
{
	SSTATE_START,	//Pocatecni stav
	SSTATE_ID,	//cte ID (zacina pismenem nebo _)
	SSTATE_NUM,	//cte cele cislo
	SSTATE_NUM_UNDERSCORE,	//cte cele cislo a narazil na jedno podtrzitko (BASE extension)
	SSTATE_NUM_REAL,	//cte realne cislo, neboli float
	SSTATE_NUM_REAL_UNDERSCORE,	//cte realne cislo a narazil na podtrzitko (BASE extension)
	SSTATE_NUM_DOT,	//cte cele cislo, narazil na tecku a nejspise bude cist realne cislo
	SSTATE_NUM_EXPONENT,	//cte cislo a narazil na exponent (znak e nebo E)
	SSTATE_NUM_EXPONENT_SIGN,	//po znaku e/E narazil na znamenko + nebo -
	SSTATE_NUM_EXPONENT_AFTER,	//cte exponent, ocekava pouze cisla
	SSTATE_NUM_EXPONENT_UNDERSCORE,	//narazil na jedno podtrzitko u psani exponentu
	SSTATE_NUM_ZERO, //precetl nulu
	SSTATE_NUM_BIN_PREFIX,	//po nule nasledoval binarni prefix b/B
	SSTATE_NUM_OCT_PREFIX,	//to stejne s oktalnim prefixem o/O
	SSTATE_NUM_HEX_PREFIX,	//to stejne s hexadecimalnim prefixem x/X
	SSTATE_NUM_BIN,	//cte binarni cislo
	SSTATE_NUM_OCT, //cte oktalni cislo
	SSTATE_NUM_HEX, //cte hexadecimalni cislo
	SSTATE_NUM_BIN_UNDERSCORE,	//narazil na podtrzitko pri cteni binarniho cisla
	SSTATE_NUM_OCT_UNDERSCORE,	//to stejne s oktalnim
	SSTATE_NUM_HEX_UNDERSCORE,	//to stejne s hexadecimalnim
	SSTATE_STRING,	//cte string
	SSTATE_STRING_BACKSLASH,	//cte nejakou escape sekvenci uvnitr stringu
	SSTATE_STRING_HEX1,	//cte prvni znak hexadecimalniho ascii kodu ve stringu
	SSTATE_STRING_HEX2,	//stejne tak druhy znak
	SSTATE_LESS,	//precetl znak <
	SSTATE_MORE,	//precetl znak >
	SSTATE_EQUAL,	//precetl znak =
	SSTATE_EXMARK,	//precetl znak !
	SSTATE_INITVAR,	//precetl znak :
	SSTATE_SLASH,	//precetl lomitko /
	SSTATE_COMMENT,	//cte jednoradkovy komentar
	SSTATE_BLOCK_COMMENT,	//cte blokovy komentar
	SSTATE_BLOCK_COMMENT_END,	//cte blokovy komentar a narazil na znak *

} SFSM_STATE; //Vsechny stavy scanner FSM


// test zda je token keyword, kdyz ano zmeni jeho typ na konkretni keyword a uvolni alokovanou string value
bool keywordCheck (Token *currentToken)
{
	if (currentToken->type != TOKEN_ID) return false;

	if (!strcmp(currentToken->attribute.string, "else"))
	{
		currentToken->type = TOKEN_KEYWORD_ELSE;
	}
	else if (!strcmp(currentToken->attribute.string, "float64"))
	{
		currentToken->type = TOKEN_KEYWORD_FLOAT64;
	}
	else if (!strcmp(currentToken->attribute.string, "for"))
	{
		currentToken->type = TOKEN_KEYWORD_FOR;
	}
	else if (!strcmp(currentToken->attribute.string, "func"))
	{
		currentToken->type = TOKEN_KEYWORD_FUNC;
	}
	else if (!strcmp(currentToken->attribute.string, "if"))
	{
		currentToken->type = TOKEN_KEYWORD_IF;
	}
	else if (!strcmp(currentToken->attribute.string, "int"))
	{
		currentToken->type = TOKEN_KEYWORD_INT;
	}
	else if (!strcmp(currentToken->attribute.string, "return"))
	{
		currentToken->type = TOKEN_KEYWORD_RETURN;
	}
	else if (!strcmp(currentToken->attribute.string, "string"))
	{
		currentToken->type = TOKEN_KEYWORD_STRING;
	}
	else if (!strcmp(currentToken->attribute.string, "package"))
	{
		currentToken->type = TOKEN_KEYWORD_PACKAGE;
	}
	else return false;

	free(currentToken->attribute.string); //pokud to byl keyword tak uvolni pamet
	return true; // true se vrati kdyz je to keyword
} // konec funkce keyword check


// funkce co nacte novy Token do adresy currentToken
// vraci 0 pri kdyz nacte platny token, 1 kdyz narazi na lexikalni chybu
int scannerGetToken (Token *currentToken)
{
	currentToken->type = TOKEN_EMPTY;
	SFSM_STATE state = SSTATE_START;

	char currChar = '\0';

	int i = 0; //pocitadlo string attributu
	char hexString[2] = {'0', '0'}; //hex cislo uvnitr stringu (napriklad "\x2F")


	while(1)
	{
		currChar = getc(FILE_INPUT);

		switch(state)
		{
			case(SSTATE_START):
				if (currChar == '\n')
				{
					currentToken->type = TOKEN_EOL;
					return ALL_OK;
				}
				else if (isspace(currChar))
				{
					state = SSTATE_START;
				}
				else if (isalpha(currChar) || currChar == '_')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_ID;
				}
				else if (isdigit(currChar) && currChar != '0')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM;
				}
				else if (currChar == '0')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_ZERO;
				}
				else if (currChar == '\"')
				{
					state = SSTATE_STRING;
				}
				else if (currChar == ',')
				{
					currentToken->type = TOKEN_COMMA;
					return ALL_OK;
				}
				else if (currChar == ';')
				{
					currentToken->type = TOKEN_SEMICOLON;
					return ALL_OK;
				}
				else if (currChar == '/')
				{
					state = SSTATE_SLASH;
				}
				else if (currChar == '(')
				{
					currentToken->type = TOKEN_LROUNDBRACKET;
					return ALL_OK;
				}
				else if (currChar == ')')
				{
					currentToken->type = TOKEN_RROUNDBRACKET;
					return ALL_OK;
				}
				else if (currChar == '{')
				{
					currentToken->type = TOKEN_LCURLYBRACKET;
					return ALL_OK;
				}
				else if (currChar == '}')
				{
					currentToken->type = TOKEN_RCURLYBRACKET;
					return ALL_OK;
				}
				else if (currChar == '+')
				{
					currentToken->type = TOKEN_ADD;
					return ALL_OK;
				}
				else if (currChar == '-')
				{
					currentToken->type = TOKEN_SUB;
					return ALL_OK;
				}
				else if (currChar == '*')
				{
					currentToken->type = TOKEN_MUL;
					return ALL_OK;
				}
				else if (currChar == '=')
				{
					state = SSTATE_EQUAL;
				}
				else if (currChar == '!')
				{
					state = SSTATE_EXMARK;
				}
				else if (currChar == '<')
				{
					state = SSTATE_LESS;
				}
				else if (currChar == '>')
				{
					state = SSTATE_MORE;
				}
				else if (currChar == ':')
				{
					state = SSTATE_INITVAR;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					return ALL_OK;
				}
				else
				{
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid token\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_ID):
				if (isalnum(currChar) || currChar == '_')
				{
					strBuffer[i] = currChar;
					i++;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					strBuffer[i] = '\0';
					currentToken->type = TOKEN_ID;
					if( (currentToken->attribute.string = malloc(1 + strlen(strBuffer))) == NULL )
					{
						fprintf(FILE_ERROR, "Internal error: Memory allocation failed\n");
						return INTERNAL_ERROR;
					}
					strcpy(currentToken->attribute.string, strBuffer);
					keywordCheck(currentToken);
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM):
				if (isdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
				}
				else if (currChar == '.')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_DOT;
				}
				else if (currChar == 'e' || currChar == 'E')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_UNDERSCORE;
				}
        		else
        		{
        			ungetc(currChar, FILE_INPUT);
        			strBuffer[i] = '\0';
        			currentToken->attribute.integer = atol(strBuffer);
        			currentToken->type = TOKEN_INTVALUE;
        			return ALL_OK;
        		}
        		break;


			case(SSTATE_NUM_UNDERSCORE):
				if (isdigit(currChar))
				{
					ungetc(currChar, FILE_INPUT);
					state = SSTATE_NUM;
				}
	        	else
	        	{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Underscore (only single one) can be only between digits\n");
					return LEX_ERROR;
	        	}
	        	break;


			case(SSTATE_NUM_DOT):
				if (isdigit(currChar))
				{
        			strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_REAL;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: The dot must be followed by atlest one digit for a valid float value\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_REAL):
				if (isdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
				}
				else if (currChar == 'e' || currChar == 'E')
				{
					strBuffer[i] = 'e';
					i++;
					state = SSTATE_NUM_EXPONENT;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_REAL_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					strBuffer[i] = '\0';
					currentToken->attribute.real = atof(strBuffer);
					currentToken->type = TOKEN_FLOATVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM_REAL_UNDERSCORE):
				if (isdigit(currChar))
				{
					ungetc(currChar, FILE_INPUT);
					state = SSTATE_NUM_REAL;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Single underscore can be placed only between digits\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_ZERO):
				if (currChar == '.')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_DOT;
				}
				else if (isdigit(currChar))
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Whole decimal number cannot start with a zero\n");
					return LEX_ERROR;
				}
				else if (tolower(currChar) == 'b')
				{
					i--;
					state = SSTATE_NUM_BIN_PREFIX;
				}
				else if (tolower(currChar) == 'o')
				{
					i--;
					state = SSTATE_NUM_OCT_PREFIX;
				}
				else if (tolower(currChar) == 'x')
				{
					i--;
					state = SSTATE_NUM_HEX_PREFIX;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
        			currentToken->attribute.integer = 0;
        			currentToken->type = TOKEN_INTVALUE;
        			return ALL_OK;
				}
        		break;


			case(SSTATE_NUM_BIN_PREFIX):
				if (currChar == '0' || currChar == '1')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_BIN;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_BIN_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Prefix 0b must be followed by a binary number with optional single underscore in front of it\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_BIN):
				if (currChar == '0' || currChar == '1')
				{
					strBuffer[i] = currChar;
					i++;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_BIN_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					strBuffer[i] = '\0';
					currentToken->attribute.integer = strtol(strBuffer, NULL, 2);
					currentToken->type = TOKEN_INTVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM_BIN_UNDERSCORE):
				if (currChar == '0' || currChar == '1')
				{
					ungetc(currChar, FILE_INPUT);
					state = SSTATE_NUM_BIN;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Single underscores can be only between digits, or prefix and a digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_OCT_PREFIX):
				if (currChar >= '0' && currChar <= '7')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_OCT;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_OCT_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Prefix 0o must be followed by a binary number with optional single underscore in front of it\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_OCT):
				if (currChar >= '0' && currChar <= '7')
				{
					strBuffer[i] = currChar;
					i++;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_OCT_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					strBuffer[i] = '\0';
					currentToken->attribute.integer = strtol(strBuffer, NULL, 8);
					currentToken->type = TOKEN_INTVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM_OCT_UNDERSCORE):
				if (currChar >= '0' && currChar <= '7')
				{
					ungetc(currChar, FILE_INPUT);
					state = SSTATE_NUM_OCT;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Single underscores can be only between digits, or prefix and a digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_HEX_PREFIX):
				if (isxdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_HEX;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_HEX_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Prefix 0x must be followed by a binary number with optional single underscore in front of it\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_HEX):
				if (isxdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_HEX_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					strBuffer[i] = '\0';
					currentToken->attribute.integer = strtol(strBuffer, NULL, 16);
					currentToken->type = TOKEN_INTVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM_HEX_UNDERSCORE):
				if (isxdigit(currChar))
				{
					ungetc(currChar, FILE_INPUT);
					state = SSTATE_NUM_HEX;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Single underscores can be only between digits, or prefix and a digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_EXPONENT):
				if (currChar == '+' || currChar == '-')
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT_SIGN;
				}
				else if (isdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT_AFTER;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Exponent must be followed by at least one digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_EXPONENT_SIGN):
				if (isdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_NUM_EXPONENT_AFTER;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Exponent sign must be followed by at least one digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_NUM_EXPONENT_AFTER):
				if (isdigit(currChar))
				{
					strBuffer[i] = currChar;
					i++;
				}
				else if (currChar == '_')
				{
					state = SSTATE_NUM_EXPONENT_UNDERSCORE;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					strBuffer[i] = '\0';
					currentToken->attribute.real = atof(strBuffer);
					currentToken->type = TOKEN_FLOATVALUE;
					return ALL_OK;
				}
				break;


			case(SSTATE_NUM_EXPONENT_UNDERSCORE):
				if (isdigit(currChar))
				{
					ungetc(currChar, FILE_INPUT);
					state = SSTATE_NUM_EXPONENT_AFTER;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Single underscores can be only between digits, or prefix and a digit\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_STRING):
				if (currChar == '\"')
				{
					strBuffer[i] = '\0';
					if( (currentToken->attribute.string = malloc(1 + strlen(strBuffer))) == NULL )
					{
						fprintf(FILE_ERROR, "Internal error: Memory allocation failed\n");
						return INTERNAL_ERROR;
					}
					strcpy(currentToken->attribute.string, strBuffer);
					currentToken->type = TOKEN_STRINGVALUE;
					return ALL_OK;
				}
				else if (currChar == '\\')
				{
					state = SSTATE_STRING_BACKSLASH;
				}
				else if (currChar == '\n' || currChar == EOF)
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: String must be written on a single line and cannot reach End Of File\n");
					return LEX_ERROR;
				}
				else if (currChar > 31 && currChar != 34)
				{
					strBuffer[i] = currChar;
					i++;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid character or character sequence inside string\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_STRING_BACKSLASH):
				if (currChar == '\"')
				{
					strBuffer[i] = '\"';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == 'n')
				{
					strBuffer[i] = '\n';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == 't')
				{
					strBuffer[i] = '\t';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == '\\')
				{
					strBuffer[i] = '\\';
					i++;
					state = SSTATE_STRING;
				}
				else if (currChar == 'x')
				{
					state = SSTATE_STRING_HEX1;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid escape sequence inside string\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_STRING_HEX1):
				if (isxdigit(currChar))
				{
					hexString[0] = tolower(currChar);
					state = SSTATE_STRING_HEX2;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid hexadecimal sequence inside string\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_STRING_HEX2):
				if (isxdigit(currChar))
				{
					hexString[1] = tolower(currChar);
					unsigned int x;
					sscanf(hexString, "%x", &x);
					currChar = x & 0xFF;
					strBuffer[i] = currChar;
					i++;
					state = SSTATE_STRING;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Invalid hexadecimal sequence inside string\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_SLASH):
				if (currChar == '/')
				{
					state = SSTATE_COMMENT;
				}
				else if (currChar == '*')
				{
					state = SSTATE_BLOCK_COMMENT;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_DIV;
					return ALL_OK;
				}
				break;


			case(SSTATE_EQUAL):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_EQ;
					return ALL_OK;
				}
				else
				{
					currentToken->type = TOKEN_ASSIGN;
					return ALL_OK;
				}
				break;


			case(SSTATE_EXMARK):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_NEQ;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Character \"!\" can only be used in not equal operator \"!=\"\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_INITVAR):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_INIT;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_EMPTY;
					fprintf(FILE_ERROR, "Lexical analysis error: Character \":\" can only be used as variable initialization \":=\"\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_LESS):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_LTE;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_LT;
					return ALL_OK;
				}
				break;


			case(SSTATE_MORE):
				if (currChar == '=')
				{
					currentToken->type = TOKEN_GTE;
					return ALL_OK;
				}
				else
				{
					ungetc(currChar, FILE_INPUT);
					currentToken->type = TOKEN_GT;
					return ALL_OK;
				}
				break;


			case(SSTATE_COMMENT):
				if (currChar == '\n')
				{
					currentToken->type = TOKEN_EOL;
					return ALL_OK;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					return ALL_OK;
				}
				break;


			case(SSTATE_BLOCK_COMMENT):
				if (currChar == '*')
				{
					state = SSTATE_BLOCK_COMMENT_END;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					fprintf(FILE_ERROR, "Lexical analysis error: Block comment has to be ended before reaching End of File\n");
					return LEX_ERROR;
				}
				break;


			case(SSTATE_BLOCK_COMMENT_END):
				if(currChar == '/')
				{
					state = SSTATE_START;
				}
				else if (currChar == EOF)
				{
					currentToken->type = TOKEN_EOF;
					fprintf(FILE_ERROR, "Lexical analysis error: Block comment has to be ended before reaching End of File\n");
					return LEX_ERROR;
				}
				else if (currChar != '*')
				{
					state = SSTATE_BLOCK_COMMENT;
				}
				break;


			default:
				fprintf(FILE_ERROR, "Internal error: Scanner reached undefined state\n");
				return INTERNAL_ERROR;

		} //konec switche stavu

	} //konec while(1)


	return ALL_OK;
} //konec funkce getToken


/****************************************************** SCANNER TOKEN LIST ******************************************************************************/

// Inicializace dvousmerne vazaneho linearniho seznamu tokenu
int scannerTokenListInit(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		tokenList->first = NULL;
		tokenList->active = NULL;
		tokenList->last = NULL;
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_INIT: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Pridani noveho prvku (tokenu) do seznamu
int scannerTokenListAdd(TokenList *tokenList, Token token, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		TokenListElementPtr added = (TokenListElementPtr)malloc(sizeof(struct TokenListElement));
		if(added != NULL){
			added->token = token;

			added->leftPtr = tokenList->last;
			added->rightPtr = NULL;

			if(tokenList->last != NULL){
				tokenList->last->rightPtr = added;
			} else {
				tokenList->first = added;
			}
			tokenList->last = added;
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_ADD: MALLOC_ERROR", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_ADD: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Smazani aktivniho prvku (tokenu) ze seznamu
int scannerTokenListDeleteActive(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->active != NULL){
			TokenListElementPtr destroy = tokenList->active;
			if(destroy != tokenList->first && destroy != tokenList->last){
				if(destroy != tokenList->first){
					destroy->leftPtr->rightPtr = destroy->rightPtr;
				} else {
					destroy->rightPtr->leftPtr = NULL;
					tokenList->first = destroy->rightPtr;
				}
				if(destroy != tokenList->last){
					destroy->rightPtr->leftPtr = destroy->leftPtr;
				} else {
					destroy->leftPtr->rightPtr = NULL;
					tokenList->last = destroy->leftPtr;
				}
			} else {
				handleFreeError(scannerTokenListFree(tokenList), __LINE__, __FILE__);
			}
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_DELETE_ACTIVE: NULL_ACTIVE", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_DELETE_ACTIVE: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Posunuti aktivity seznamu na predchozi prvek
int scannerTokenListMovePrev(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->active != NULL){
			if(tokenList->active != tokenList->first){
				tokenList->active = tokenList->active->leftPtr;
			} else {
				errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_MOVE_PREV: ACTIVE_FIRST", __FILE__, __LINE__, errorHandle);
			}
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_MOVE_PREV: NULL_ACTIVE", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_MOVE_PREV: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Posunuti aktivity seznamu na nasledujici prvek
int scannerTokenListMoveNext(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->active != NULL){
			if(tokenList->active != tokenList->last){
				tokenList->active = tokenList->active->rightPtr;
			} else {
				errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_MOVE_NEXT: ACTIVE_LAST", __FILE__, __LINE__, errorHandle);
			}
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_MOVE_NEXT: NULL_ACTIVE", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_MOVE_NEXT: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Nacteni predchoziho prvku do adresy tokenu
int scannerTokenListGetPrev(TokenList *tokenList, Token *token, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->active != NULL){
			if(tokenList->active != tokenList->first){
				(*token) = tokenList->active->leftPtr->token;
			} else {
				errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_PREV: ACTIVE_FIRST", __FILE__, __LINE__, errorHandle);
			}
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_PREV: NULL_ACTIVE", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_PREV: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Nacteni aktivniho prvku do adresy tokenu
int scannerTokenListGetActive(TokenList *tokenList, Token *token, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->active != NULL){
			(*token) = tokenList->active->token;
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_ACTIVE: NULL_ACTIVE", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_ACTIVE: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Nacteni nasledujiciho prvku do adresy tokenu
int scannerTokenListGetNext(TokenList *tokenList, Token *token, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->active != NULL){
			if(tokenList->active != tokenList->last){
				(*token) = tokenList->active->rightPtr->token;
			} else {
				errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_NEXT: ACTIVE_LAST", __FILE__, __LINE__, errorHandle);
			}
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_NEXT: NULL_ACTIVE", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_GET_NEXT: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Nastavi prvni prvek seznamu jako aktivni prvek
int scannerTokenListSetActiveFirst(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->first != NULL){
			tokenList->active = tokenList->first;
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_SET_ACTIVE_FIRST: FIRST_NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_SET_ACTIVE_FIRST: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Nastavi posledni prvek seznamu jako aktivni prvek
int scannerTokenListSetActiveLast(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		if(tokenList->last != NULL){
			tokenList->active = tokenList->last;
		} else {
			errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_SET_ACTIVE_LAST: LAST_NULL", __FILE__, __LINE__, errorHandle);
		}
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_TOKEN_LIST_SET_ACTIVE_LAST: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}

// Uvolni cely seznam i vsechny obsazene tokeny z pameti
int scannerTokenListFree(TokenList *tokenList)
{
	if(tokenList != NULL){
		TokenListElementPtr first = tokenList->first;
		TokenListElementPtr destroy;
		while(first != NULL){
			destroy = first;
			first = first->rightPtr;
			if(destroy->token.type == TOKEN_ID || destroy->token.type == TOKEN_STRINGVALUE)
			{
				free(destroy->token.attribute.string);
			}
			free(destroy);
		}
		tokenList->first = NULL;
		tokenList->active = NULL;
		tokenList->last = NULL;
		return ALL_OK;
	}
	return INTERNAL_ERROR;
}


/****************************************************** SCANNER GET TOKEN LIST ******************************************************************************/

// Naplni seznam tokeny pomoci opakovaneho volani funkce scannerGetToken
int scannerGetTokenList(TokenList *tokenList, ErrorHandle *errorHandle)
{
	if(errorExists(*errorHandle)){return ERROR_ALREADY_EXISTS;}

	if(tokenList != NULL){
		Token currentToken;
		currentToken.type = TOKEN_EMPTY;
		currentToken.pos_line = 1;
		currentToken.pos_number = 0;

		do{
			if(currentToken.type == TOKEN_EOL){
				currentToken.pos_line++;
				currentToken.pos_number = 1;
			} else {
				currentToken.pos_number++;
			}
			scannerTokenListSetActiveLast(tokenList, errorHandle); // vyhodí error, když je seznam prázdný
			errorHandleInit(errorHandle); // proto zde, přepíšeme na ALL_OK

			errorHandle->errorID = scannerGetToken(&currentToken);
			if(errorHandle->errorID != ALL_OK){
				errorSet(errorHandle->errorID, "scannerGetToken", __FILE__, __LINE__, errorHandle);
			}

			scannerTokenListAdd(tokenList, currentToken, errorHandle);
		}while(currentToken.type != TOKEN_EOF && errorHandle->errorID == ALL_OK);

		scannerTokenListSetActiveLast(tokenList, errorHandle);
	} else {
		errorSet(INTERNAL_ERROR, "SCANNER_GET_TOKEN_LIST: NULL_POINTER", __FILE__, __LINE__, errorHandle);
	}

	return errorHandle->errorID;
}
