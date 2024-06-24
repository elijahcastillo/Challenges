#include "lexer.h"
#include <stdlib.h>

#include <ctype.h> 

#define LEXER_FILE_BUFF_SIZE 10
#define INITIAL_IDENTIFIER_SIZE 64
#define INITIAL_NUMBER_SIZE 10
#define INITIAL_STRING_SIZE 10
#define DYNAMIC_GROWTH_FACTOR 2

Lexer* lexerInit(const char* filepath){
	Lexer* lex = malloc(sizeof(Lexer));
	if(!lex) return NULL;


	lex->fileBuff = malloc(sizeof(char) * LEXER_FILE_BUFF_SIZE + 1); // +1 for \0
	if(!lex->fileBuff){
		free(lex);
		return NULL;
	}

	lex->file = fopen(filepath ,"r");
	if(!lex->file){
		free(lex->fileBuff);
		free(lex);
		return NULL;
	}

	lex->line = 1;
	lex->buffIndex = 0;
	lex->buffSize = 0;
	lexerReadChunk(lex);

	return lex;
}

void lexerFree(Lexer* lex){
	if(lex == NULL) return;

	if(lex->fileBuff) free(lex->fileBuff);
	if(lex->file) fclose(lex->file);
	free(lex);
}

size_t lexerReadChunk(Lexer* lex){
	if (lex == NULL || lex->file == NULL || lex->fileBuff == NULL) {
        return 0;
    }

    size_t read_count = fread(lex->fileBuff, sizeof(char), LEXER_FILE_BUFF_SIZE, lex->file);

	// Null-terminate the buffer
	lex->fileBuff[read_count] = '\0'; 

	// Reset buffer index
	lex->buffIndex = 0;

    // Update the buffer size
    lex->buffSize = read_count;

    return read_count;
}

char lexerConsumeChar(Lexer* lex){
	if(lex->buffIndex >= lex->buffSize){
		int charsRead = lexerReadChunk(lex);	
		if(charsRead == 0){
			return '\0';
		}
	}

	return lex->fileBuff[lex->buffIndex++];
}

char lexerPeekChar(Lexer* lex){
    if (lex->buffIndex >= lex->buffSize) {
        int charsRead = lexerReadChunk(lex);
        if (charsRead == 0) {
            return '\0';
        }
    }

	return lex->fileBuff[lex->buffIndex];
}


void _lexerSkipWhiteSpace(Lexer* lex){
    char c = lexerPeekChar(lex);
    while (c != '\0' && isspace(c)) {
        if (c == '\n') lex->line++;
        lexerConsumeChar(lex);
        c = lexerPeekChar(lex);
    }
}

char* appendCharToString(char* str, size_t* size, size_t* capacity, char c) {
    if (*size >= *capacity - 1) {
        *capacity *= DYNAMIC_GROWTH_FACTOR;
        char* newStr = realloc(str, *capacity * sizeof(char));
        if (!newStr) {
            free(str);
            return NULL;
        }
        str = newStr;
    }
    str[(*size)++] = c;
    str[*size] = '\0';
    return str;
}

void _lexerError(Lexer* lex, const char* msg){
	printf("(Lexer Error) Line %d -> %s\n", lex->line, msg);
	exit(1);
}

Token _lexerConsumeIdentifier(Lexer* lex) {
    Token token;
    token.type = IDENTIFIER;
    token.str = NULL;
    token.hash = 0;

    size_t identSize = 0;
    size_t identCap = INITIAL_IDENTIFIER_SIZE;
    char* ident = malloc(identCap * sizeof(char));
    if (!ident) {
		_lexerError(lex, "Failed to allocate space for identifier");
    }

    char c = lexerPeekChar(lex);
    while (isalnum(c) || c == '_') {
        ident = appendCharToString(ident, &identSize, &identCap, lexerConsumeChar(lex));
        if (!ident) {
			_lexerError(lex, "Failed to allocate space for identifier");
        }
        c = lexerPeekChar(lex);
    }

    token.str = ident;
    return token;
}

Token _lexerConsumeNumber(Lexer* lex){
    Token token;
    token.type = INTEGER;
    token.str = NULL;
    token.hash = 0;

    size_t numSize = 0;
    size_t numCap = INITIAL_NUMBER_SIZE;
    char* num = malloc(numCap * sizeof(char));
    if (!num) {
		_lexerError(lex, "Failed to allocate space for Number");
    }

    char c = lexerPeekChar(lex);
    while (isdigit(c)) {
        num = appendCharToString(num, &numSize, &numCap, lexerConsumeChar(lex));
        if (!num) {
			_lexerError(lex, "Failed to allocate space for Number");
        }
        c = lexerPeekChar(lex);
    }

    token.str = num;
    return token;
}


Token _lexerConsumeString(Lexer* lex) {
    Token token;
    token.type = STRING;
    token.str = NULL;
    token.hash = 0;

    size_t strSize = 0;
    size_t strCap = INITIAL_STRING_SIZE;
    char* str = malloc(strCap * sizeof(char));
    if (!str) {
		_lexerError(lex, "Failed to allocate space for String");
    }

    // Consume the opening quote
    lexerConsumeChar(lex);

    char c = lexerConsumeChar(lex);
    while (c != '\"' && c != '\0') {
        if (c == '\\') { // Handle escape sequences
            char nextChar = lexerConsumeChar(lex);
            switch (nextChar) {
                case '\"': c = '\"'; break;
                case '\\': c = '\\'; break;
                case 'n': c = '\n'; break;
                case 't': c = '\t'; break;
                default: c = nextChar; break;
            }
        }
        str = appendCharToString(str, &strSize, &strCap, c);
        if (!str) {
			_lexerError(lex, "Failed to allocate space for String");
        }
        c = lexerConsumeChar(lex);
    }

    token.str = str;
    return token;
}

Token _lexerConsumeSingleCharacters(Lexer* lex){
    Token token;
    token.type = TOKEN_EOF;
    token.str = NULL;
    token.hash = 0;

    char c = lexerConsumeChar(lex);
	char next = lexerPeekChar(lex);

	if(c == '=' && next == '='){

	}

	if(c == '!' && next == '='){

	}

   switch (c) {
        case '+':
            token.type = ADD;
            token.str = "+";
            break;
        case '-':
            token.type = SUB;
            token.str = "-";
            break;
        default:
			token.type = TOKEN_UNKNOWN;
            break;
    }

	return token;
}



Token lexerParseToken(Lexer* lex){

	_lexerSkipWhiteSpace(lex);

    char c = lexerPeekChar(lex);
    if (c == '\0') {
        Token token = { TOKEN_EOF, NULL, 0 };
        return token;
    }

	if(isalpha(c) || c == '_'){
		return _lexerConsumeIdentifier(lex);
	}

	if(isdigit(c)){
		return _lexerConsumeNumber(lex);
	}

    if (c == '\"') {
        return _lexerConsumeString(lex);
    }

	if(c == '/'){
		c = lexerConsumeChar(lex);
		if(c == '/'){
			while(lexerPeekChar(lex) != '\n' && lexerPeekChar(lex) != '\0') lexerConsumeChar(lex);
		} else {
			_lexerError(lex, "Unexpected '/' comments are declared using '//'");
		}
	}

	return _lexerConsumeSingleCharacters(lex);

}

Token lexerNextToken(Lexer* lex){
	return lexerParseToken(lex);
}

