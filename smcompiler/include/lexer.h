#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stddef.h>

enum TokenType {
	
	IDENTIFIER,
	FUNC,
	INTEGER,
	FLOAT,
	STRING,

	LBRACE,
	RBRACE,
	LPAREN,
	RPAREN,
	COMMA,
	DOT,

	// Operators
	ADD,
	SUB,
	MULT,
	DIV,
	MOD,

	TOKEN_EOF,
	TOKEN_ERROR,
	TOKEN_UNKNOWN,
};

typedef struct {
	enum TokenType type;
	const char* str;
	int hash;
} Token;

typedef struct {
	FILE*  file;	  // File handle to text being tokenized	
	char*  fileBuff;  // Chunk of file ready to parse
	size_t buffIndex; // Index into chunk to read next char 
	size_t buffSize;  // Current valid buffer size
	int    line;	  // Tracks current line to report errors	
} Lexer;


Lexer* lexerInit(const char* filepath);
void lexerFree(Lexer* lex);

size_t lexerReadChunk(Lexer* lex);
char lexerConsumeChar(Lexer* lex);
char lexerPeekChar(Lexer* lex);

Token lexerParseToken(Lexer* lex);
Token lexerNextToken(Lexer* lex);


#endif // LEXER_H
