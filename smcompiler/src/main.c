#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

int main() {
    Lexer* lex = lexerInit("test.txt");
    if (lex == NULL) {
        perror("Failed to initialize lexer");
        return EXIT_FAILURE;
    }

	Parser* par = parserInit(lex);
	printf("%d\n", parserParse(par));



  //   printf("Press any key to read the next token...\n");
  //   Token current;
  //   while ((current = lexerNextToken(lex)).type != TOKEN_EOF) {
  //       // Wait for a key press
  //       getchar();
		// tokenPrint(current);
  //   }

	lexerFree(lex);

    return EXIT_SUCCESS;
}


