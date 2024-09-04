#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

int str2int(char *str, int length) {
    long result = 0; // use long to check for overflow
    int sign = 1;
    int current = 0;

    if ((*str) == '-') {
        sign = -1;
        str++;
        current++;
        if (current >= length) return STR2INT_INCONVERTIBLE;
    }

    while ((*str >= '0') && (*str <= '9') && current < length) {
        int digit = (*str) - '0';
        if (sign == 1 && result > (INT_MAX - digit) / 10) {
            return STR2INT_OVERFLOW; // overflow
        } else if (sign == -1 && result > -(INT_MIN + digit) / 10) {
            return STR2INT_UNDERFLOW; // underflow
        }

        result = (result * 10) + digit;
        str++;
        current++;
    }

    return sign * result; // apply the sign
}


typedef enum {
	NODE_NUMBER
	NODE_BINOP
} NodeType;

typedef struct {
	NodeTyoe type
	union {
		struct {struct ASTNode* left, struct ASTNode* right; char op} binop;
		int value;
	}
} ASTNode;

ASTNode* createNumberNode(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUMBER;
    node->data.value = value;
    return node;
}

ASTNode* createBinaryOpNode(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}



typedef struct {
	Lexer* lex;
	Token next;
} Parser;


Parser* parserInit(Lexer* lex){
	Parser* parser = malloc(sizeof(Parser));
	parser->lex = lex;
	parser->next = lexerParseToken(lex);
	return parser;
}

void _parserAdvance(Parser* par){
	if(par->next.type == EOF){
		printf("(_parserAdvance) Already reached EOF");
		exit(1);
	}
	par->next = lexerParseToken(par->lex);
}

void _parserEat(Parser* par, TokenType type, const char* msg){
	if(par->next.type == type){
		_parserAdvance(par);
	} else {
		printf("Unexpected Token: %s -> '%.*s' on line %d, expected %s \n", tokenTypeToString(par->next.type), par->next.length, par->next.str ? par->next.str : "NULL" , par->next.line, tokenTypeToString(type));
		printf("%s\n", msg);
		exit(1);
	}
}



int _parserMatch(Parser* par, TokenType type){
	return par->next.type == type;	
}

Token _parserPeek(Parser* par){
	return par->next;
}



int parseFactor(Parser* p) {
    int value = 0;
    if (_parserMatch(p, LPAREN)) {
        _parserAdvance(p); // Move past '('
        value = parseExpr(p); // Recursively parse the expression inside the parentheses
        _parserEat(p, RPAREN, "Expected ')' after expression"); // Ensure ')' follows the expression
    } else {
        value = str2int(_parserPeek(p).str, _parserPeek(p).length); // Convert the current token string to an integer
        _parserAdvance(p); // Move past the number
    }
    return value;
}

int parseTerm(Parser* p){
    int left = parseFactor(p);
    while(_parserMatch(p, MULT) || _parserMatch(p, DIV)){
        Token op = _parserPeek(p); // Store the current token for operator
        _parserAdvance(p); // Advance past the operator token
        int right = parseFactor(p);
        if(op.str[0] == '*'){ 
            left *= right;
        } else {
            left /= right;
        }
    }
    return left;
}

int parseExpr(Parser* p){
    int left = parseTerm(p);
    while(_parserMatch(p, ADD) || _parserMatch(p, SUB)){
        Token op = _parserPeek(p); // Store the current token for operator
        _parserAdvance(p); // Advance past the operator token
        int right = parseTerm(p);
        if(op.str[0] == '+') { 
            left += right; 
        } else { 
            left -= right;
        }
    }
    return left;
}

int parserParse(Parser* p){
	return parseExpr(p);
}



#endif // PARSER_H
