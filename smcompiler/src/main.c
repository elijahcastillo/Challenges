#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"


int main() {
    Lexer* lex = lexerInit("lexerTest.txt");
    if (lex == NULL) {
        perror("Failed to initialize lexer");
        return EXIT_FAILURE;
    }


    printf("Press any key to read the next token...\n");
    Token current;
    while ((current = lexerNextToken(lex)).type != TOKEN_EOF) {
        // Wait for a key press
        getchar();

        // Print the token information
        printf("Token: %d, String: %s\n", current.type, current.str ? current.str : "NULL");
    }

	printf("DONE\n");
	lexerFree(lex);

    return EXIT_SUCCESS;
}


