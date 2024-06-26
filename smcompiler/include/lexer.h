#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stddef.h>

typedef enum {
	
	IDENTIFIER,
	FUNC,
	INTEGER,
	FLOAT,
	STRING,
	NEW_LINE,

	LBRACE,
	RBRACE,
	LPAREN,
	RPAREN,
	COMMA,
	DOT,
	SEMICOLON,
	OR,
	AND,


	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,

	EQUAL,
	EQUALITY,
	NOT_EQUAL,

	// Operators
	ADD,
	SUB,
	MULT,
	DIV,
	MOD,

	TOKEN_EOF,
	TOKEN_ERROR,
	TOKEN_UNKNOWN,
} TokenType;



typedef struct {
	TokenType type;
	char* str;	// Start of slice into lexer source
	int length;		// Length of slice
	int line;
} Token;

typedef struct {
	char* source;			// Stores the raw string from the file
	size_t sourceLength;	// Stores the raw string from the file
	char* tokenStart; // Pointer to start of token in source
	char* current;	// Pointer to the current character
	int    line;			// Tracks current line to report errors	
} Lexer;


char* readFileToString(const char* filename, size_t* sizeOut);
char* tokenTypeToString(TokenType type);
void tokenPrint(Token tok);

Lexer* lexerInit(const char* filepath);
void lexerFree(Lexer* lex);

Token lexerParseToken(Lexer* lex);
Token lexerNextToken(Lexer* lex);


#endif // LEXER_H
