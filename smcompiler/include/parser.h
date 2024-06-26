#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	Lexer* lex;
	Token next;
} Parser;


Parser* parserInit(Lexer* lex){
	// Parser* parser = malloc();
}

void parserParse(Parser* p){

}

void _parserMatch(Parser* par, TokenType type){
	if(par->next.type == type){
		par->next = lexerParseToken(par->lex);
	} else {
		printf("Unexpected Token: %s -> '%s' on line %d, expected %s \n", tokenTypeToString(par->next.type), par->next.str ? par->next.str : "NULL" , par->next.line, tokenTypeToString(type));
	}
	
}

#endif // PARSER_H
