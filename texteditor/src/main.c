
#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "piece_table.h"
#include "editor.h"
#include "utils.h"




int main(int argc, char** argv){

	// PieceTable* tb = createPieceTable(readFileToString("test2.txt"));
	// printf("\n");
	// printf("Initial Table:\n");
	// pieceTablePrintPieces(tb);
	// printf("\n");
	// pieceTableInsert(tb, 0, " test");
	// printf("\n");
	// printf("After Insert Table:\n");
	// pieceTablePrintPieces(tb);
	//
	// return 0;

	Editor* editor = createEditor();
	// editor->cx += 3; // Line numbers padding start

	if(editorEnableRawMode(editor) == -1) die("Unable To Enable Raw Mode");
	if(editorUpdateScreenDimensions(editor) == -1) die("Cannot get terminal dimensions");
	CLEAR_SCREEN();
	CURSOR_RESET();


	editorInitializeBuffer(editor, readFileToString("test2.txt"));
	editorRenderText(editor);
	char c;
    while (1) {
        if (terminalIsKeyHit()) {
            read(STDIN_FILENO, &c, 1);
            if (c == 'q') {

                CLEAR_SCREEN();
                CURSOR_RESET();
                editorDisableRawMode(editor);
                exit(1);
            }

            // Check for escape character
            if (c == '\x1b') {
                char seq[3];
                // Read the next two characters
                if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
                if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

                // Check for arrow keys
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A':
                            // Handle up arrow key press
							editorUpdateCursorPosition(editor, editor->ptx, editor->pty - 1);
                            break;
                        case 'B':
                            // Handle down arrow key press
							editorUpdateCursorPosition(editor, editor->ptx, editor->pty + 1);
                            break;
                        case 'C':
                            // Handle right arrow key press
							editorUpdateCursorPosition(editor, editor->ptx + 1, editor->pty);
                            break;
                        case 'D':
                            // Handle left arrow key press

							editorUpdateCursorPosition(editor, editor->ptx - 1, editor->pty);
                            break;
                    }
                }
			} else if (c == 127 || c == 8){
				// Backspace
				editorDeleteCharacter(editor, 1);

			} else {
                // Handle regular character input
				editorInsertCharacter(editor, c);

            }

                CLEAR_SCREEN();
                CURSOR_RESET();
                editorRenderText(editor);
        }
    }
}
