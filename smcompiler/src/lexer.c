#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> 


char* readFileToString(const char* filename, size_t* sizeOut) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    // Seek to the end of the file to determine the file size
    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    rewind(file);

    // Allocate memory for the entire content plus the null terminator
    char* buffer = malloc(size + 1);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    // Read the file into memory
    size_t bytesRead = fread(buffer, 1, size, file);
    if (bytesRead != size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Null-terminate the string
    buffer[size] = '\0';

    // Set the file size output if requested
    if (sizeOut != NULL) {
        *sizeOut = size;
    }

    // Close the file and return the buffer
    fclose(file);
    return buffer;
}

Lexer* lexerInit(const char* filepath){
	Lexer* lex = malloc(sizeof(Lexer));
	if(!lex) return NULL;

	lex->sourceLength = 0;
	lex->source = readFileToString(filepath, &lex->sourceLength);
	lex->current = lex->source; // Start at the begining of source
	lex->line = 1;

	return lex;
}

void lexerFree(Lexer* lex){
	if(lex == NULL) return;
	if(lex->source) free(lex->source);
	free(lex);
}

Token _createToken(Lexer* lex, TokenType type){
	Token tok;
    tok.type = type;
    tok.str = lex->tokenStart;
    tok.length = (int)(lex->current - lex->tokenStart);
    tok.line = lex->line;
	return tok;
}

Token _createEOFToken(Lexer* lex){
	Token tok;
    tok.type = TOKEN_EOF;
    tok.str = lex->current;
    tok.length = 0;
    tok.line = lex->line;
	return tok;
}

void tokenPrint(Token tok){
	printf("Token(type=%s str='%.*s' len=%d line=%d)\n", tokenTypeToString(tok.type), tok.length, tok.str, tok.length, tok.line);
}

int isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int isNum(char c) {
    return c >= '0' && c <= '9';
}
int isAlphaNum(char c) {
    return isAlpha(c) || isNum(c);
}

int lexerIsAtEnd(Lexer* lex){
	return lex->current >= lex->source + lex->sourceLength;
		
}



char lexerConsumeChar(Lexer* lex){
	char c =  *lex->current;
	if(c == '\n') lex->line++;
    lex->current++;
    return c;
}

char lexerPeekChar(Lexer* lex){
    if(lexerIsAtEnd(lex)) return '\0';
    return *lex->current;
}

char lexerPeekChar2(Lexer* lex) {
    if(lexerIsAtEnd(lex)) return '\0';
    return lex->current[1];
}


void _lexerSkipWhiteSpace(Lexer* lex){
    while (!lexerIsAtEnd(lex) && isspace(lexerPeekChar(lex))) {
        lexerConsumeChar(lex);
    }
}




void _lexerError(Lexer* lex, const char* msg){
	printf("(Lexer Error) Line %d -> %s\n", lex->line, msg);
	exit(1);
}

Token _lexerConsumeIdentifier(Lexer* lex) {
	while(isAlpha(lexerPeekChar(lex))){
		lexerConsumeChar(lex);
	}
	return _createToken(lex, IDENTIFIER);
}

Token _lexerConsumeNumber(Lexer* lex){
	while(isNum(lexerPeekChar(lex))){
		lexerConsumeChar(lex);
	}

	return _createToken(lex, INTEGER);
}


Token _lexerConsumeString(Lexer* lex) {
    lexerConsumeChar(lex);  // Consume the opening quote

    while (!lexerIsAtEnd(lex) && lexerPeekChar(lex) != '\"') {
        char c = lexerConsumeChar(lex);
        if (c == '\\') { // Handle escape sequences
            lexerConsumeChar(lex); // Skip the next character
        }
    }

    if (lexerPeekChar(lex) == '\"') lexerConsumeChar(lex); // Consume the closing quote

    return _createToken(lex, STRING);
}


void _lexerSkipComment(Lexer* lex) {
    while (lexerPeekChar(lex) != '\n' && !lexerIsAtEnd(lex)) {
            lexerConsumeChar(lex);
	}
}

Token _lexerConsumeSingleCharacters(Lexer* lex){

	Token tok;
    char c = lexerConsumeChar(lex);
	char next = lexerPeekChar(lex);

	// if(c == '=' && next == '='){
	//
	// }
	//
	// if(c == '!' && next == '='){
	//
	// }

   switch (c) {
        case '+':
		    tok = _createToken(lex, ADD); break;
        case '-':
		    tok = _createToken(lex, SUB); break;	
        case '/':
		    tok = _createToken(lex, DIV); break;	
        case '*':
		    tok = _createToken(lex, MULT); break;	
        case '%':
		    tok = _createToken(lex, MOD); break;	
        case '=':
		    tok = _createToken(lex, EQUAL);	break;
        case '{':
		    tok = _createToken(lex, LBRACE); break;
        case '}':
		    tok = _createToken(lex, RBRACE); break;
        case '(':
		    tok = _createToken(lex, LPAREN); break;
        case ')':
		    tok = _createToken(lex, RPAREN); break;
        case ',':
		    tok = _createToken(lex, COMMA); break;
        case '.':
		    tok = _createToken(lex, DOT); break;
        case ';':
		    tok = _createToken(lex, SEMICOLON); break;
        case '<':
		    tok = _createToken(lex, LESS); break;
        case '>':
		    tok = _createToken(lex, GREATER); break;
        case '\n':
		    tok = _createToken(lex, NEW_LINE); break;
        default:
			tok = _createToken(lex, TOKEN_UNKNOWN);
            break;
    }

	return tok;
}

char* tokenTypeToString(TokenType type){
	   switch (type) {
       case IDENTIFIER:
			return "IDENTIFIER";
       case INTEGER:
			return "INTEGER";
        case ADD:
			return "ADD";
        case SUB:
			return "SUB";
        case DIV:
			return "DIV";
        case MULT:
			return "MULT";
        case MOD:
			return "MOD";
        case EQUAL:
			return "EQUAL";
        case LBRACE:
			return "LBRACE";
        case RBRACE:
			return "RBRACE";
        case LPAREN:
			return "LPAREN";
        case RPAREN:
			return "RPAREN";
        case COMMA:
			return "COMMA";
        case DOT:
			return "DOT";
        case SEMICOLON:
			return "SEMICOLON";
        case LESS:
			return "LESS";
        case GREATER:
			return "GREATER";
        default:
			return "UNKNOWN";
    }
}


Token lexerParseToken(Lexer* lex){

	_lexerSkipWhiteSpace(lex);

	if(lexerPeekChar(lex) == '/' && lexerPeekChar2(lex) == '/'){
		// Skip '//'
        lexerConsumeChar(lex);
        lexerConsumeChar(lex);
        _lexerSkipComment(lex);
	}

    if (lexerIsAtEnd(lex)) {
		return _createEOFToken(lex);
    }

	// Store the start of lexer before parsing token
	// this allows us to calculate the length of the token
	lex->tokenStart = lex->current;

    char c = lexerPeekChar(lex);

	if(isAlpha(c)){
		return _lexerConsumeIdentifier(lex);
	}

	if(isdigit(c)){
		return _lexerConsumeNumber(lex);
	}

    if (c == '\"') {
        return _lexerConsumeString(lex);
    }

	return _lexerConsumeSingleCharacters(lex);

}

Token lexerNextToken(Lexer* lex){
	return lexerParseToken(lex);
}

