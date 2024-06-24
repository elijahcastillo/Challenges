#include "editor.h"
#include "terminal.h"
#include "piece_table.h"
#include "dyarray.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LINE_NUMBER_OFFSET 4
#define TAB_WIDTH 4
Editor* createEditor(){
	Editor* editor = malloc(sizeof(Editor));
	editor->cx = 0;
	editor->cy = 0;
	editor->row_offset = 0;
	editor->col_offset = 0;
	editor->screen_rows = -1;
	editor->screen_cols = -1;
	editor->line_number_offset = LINE_NUMBER_OFFSET;
	editor->table = NULL;
	return editor;
}


int editorEnableRawMode(Editor* e){
	struct termios original = terminalEnableRawMode();
	e->original_attributes = original;
	return 0;
}

int editorDisableRawMode(Editor* e){
	terminalDisableRawMode(&e->original_attributes);
	return 0;
}

int editorUpdateScreenDimensions(Editor* e){
	if(terminalGetTermDim(&e->screen_rows, &e->screen_cols) == -1){
		return -1;
	}
	return 0;
}


int editorInitializeBuffer(Editor* e, char* text){
	e->table = malloc(sizeof(PieceTable));
	if(!e->table) return -1;
	pieceTableInit(e->table, text);
	return 0;
}



int _getTotalLines(Editor* e) {
    int totalLines = 0;
    Piece* current = e->table->head;
    while (current != NULL) {
        const char* txt = PIECE_GET_BUFFER(e->table, current);
        for (int i = 0; i < current->length; i++) {
            if (txt[current->start + i] == '\n') {
                totalLines++;
            }
        }
        current = current->next;
    }
    return totalLines;
}

int editorGetLineLength(Editor* e, int targetRow) {
    int lineLength = 0;
    int currentRow = 0;

    Piece* current = e->table->head;
    while (current != NULL) {
        const char* txt = PIECE_GET_BUFFER(e->table, current);

        for (int i = 0; i < current->length; i++) {
            char c = txt[current->start + i];

            if (currentRow == targetRow) {

                if (c == '\n') {

                    return lineLength; // End of the target row
                } else if (c == '\t'){
                    lineLength += TAB_WIDTH - (lineLength % TAB_WIDTH);
				} else {
                    lineLength++;
                }
            } else {
                if (c == '\n') {
                    currentRow++;
                }
            }
        }


        current = current->next;
    }

    // If the target row is beyond the last row of text
    return -1;
}






int _getCursorPieceTablePos(Editor* e, char* cursorChar) {
    int cursorPieceTablePos = 0;
    int currentRow = 0;
    int currentCol = e->line_number_offset;

    Piece* current = e->table->head;
    while (current != NULL) {
        const char* txt = PIECE_GET_BUFFER(e->table, current);
        LOG("Processing Piece: start=%d, length=%d\n", current->start, current->length);

        for (int i = 0; i < current->length; i++) {
            char c = txt[current->start + i];
            LOG("Character: '%c' (position in piece: %d, cursorPieceTablePos: %d)\n", c, i, cursorPieceTablePos);

            // Check if we've reached the cursor's position
            if (currentRow == e->cy && currentCol == e->cx) {
                if (cursorChar != NULL) {
                    *cursorChar = c;
                }
                return cursorPieceTablePos;
            }

            if (c == '\n') {
                currentRow++;
                currentCol = e->line_number_offset;
                cursorPieceTablePos++;
                LOG("Newline found. currentRow: %d, currentCol: %d\n", currentRow, currentCol);
            } else if (c == '\t') {
                int tabSpaces = TAB_WIDTH - (currentCol % TAB_WIDTH);
                currentCol += tabSpaces;
                cursorPieceTablePos++;
                LOG("Tab found. currentCol: %d, tabSpaces: %d\n", currentCol, tabSpaces);
            } else {
                currentCol++;
                cursorPieceTablePos++;
                LOG("Regular character. currentCol: %d\n", currentCol);
            }

            LOG("AFTER INNER LOOP cursorPieceTablePos=%d, character='%c'\n", cursorPieceTablePos, c);
        }

        current = current->next;
    }

    LOG("Cursor position not found, returning calculated position. cursorPieceTablePos=%d\n", cursorPieceTablePos);

    if (cursorChar != NULL) {
        *cursorChar = '\0'; // Set to null character if cursor is at the end
    }
    return cursorPieceTablePos; // Should match the text length if cursor is at the end
}



int editorInsertCharacter(Editor* e, char c){
	
	char cursorChar;
	int cursorPosition = _getCursorPieceTablePos(e, &cursorChar);

	LOG("Before Insert Character: Cursor(%d, %d) InsertPos=%d\n", e->cx, e->cy, cursorPosition);
    char temp[2] = {c, '\0'};
    pieceTableInsert(e->table, cursorPosition, temp);
	editorUpdateCursorPosition(e, e->cx + 1, e->cy);

	LOG("After Insert Character: Cursor(%d, %d)\n", e->cx, e->cy);
	return 0;
}
int editorDeleteCharacter(Editor* e, int length) {
	char cursorChar;
	int cursorPosition = _getCursorPieceTablePos(e, &cursorChar);

    // Ensure we do not delete beyond the beginning of the piece table
    if (cursorPosition > 0) {
        cursorPosition--;
    }
    pieceTableDelete(e->table, cursorPosition, length);
    editorUpdateCursorPosition(e, e->cx - 1, e->cy);
    return 0;
}

int editorUpdateCursorPosition(Editor* e, int x, int y){



    LOG("START: Updating Cursor Position: newX=%d  newY = %d,  currX=%d  currY=%d, colOff=%d\n", x,y,e->cx, e->cy,e->col_offset);



    int totalLines = _getTotalLines(e);
    LOG("TOTAL LINES = %d\n", totalLines);

    // Prevent cursor from going beyond the last line
    if (y >= totalLines) {
        y = totalLines - 1;
    }
	// Prevent cursor from going before the first line
	if(y <= 0) y = 0;



	int newPositionLineLength = editorGetLineLength(e, y);
	LOG("NEW POS LINE LENGTH = %d\n", newPositionLineLength);

	if(x < e->line_number_offset || newPositionLineLength == 0 ){
		// Prevent cursor from going before begining of text
		x = e->line_number_offset;
	} else if (x >= newPositionLineLength + e->line_number_offset){
		// Prevent cursor from going beyond end of line
		x = newPositionLineLength + e->line_number_offset;
	}
	
	char cursorChar;
	int cursorPosition = _getCursorPieceTablePos(e, &cursorChar);

	if(cursorChar == '\t'){
		if(x < e->cx){

			x -= TAB_WIDTH - 1;
		} else {

			x += TAB_WIDTH - 1;
		}
	}



	e->cx = x;
	e->cy = y;


    // Adjust col_offset based on the cursor position
    if (e->cx >= e->col_offset + e->screen_cols) {
        e->col_offset = e->cx - e->screen_cols + 1;
    }
    if (e->cx < e->col_offset + e->line_number_offset) {
        e->col_offset = e->cx - e->line_number_offset;
        if (e->col_offset < 0) e->col_offset = 0; // Ensure col_offset does not go negative
    }


	// cx=11  off=1
	//
	// 1 2 3 4 5 6 7 8 9 10
	// - - - - - - - - - -
	// x x x


    LOG("END: Updating Cursor Position: newX=%d  newY = %d,  currX=%d  currY=%d, colOff=%d\n\n", x,y,e->cx, e->cy,e->col_offset);

}

int editorRenderText(Editor* e){
	RenderdText buff;
	dyarr_init(&buff, 20);

    int lineNumber = e->row_offset + 1;
	int charactersInLine = 0;

    // Set cursor to bar style and start blinking
    dyarr_append_multiple(&buff, "\033[5 q", strlen("\033[5 q"));

    // Add line number 
    char lineNumberBuff[32];
	snprintf(lineNumberBuff, sizeof(lineNumberBuff), "\033[48;5;236;38;5;248m  %d \033[0m", lineNumber);
    dyarr_append_multiple(&buff, lineNumberBuff, strlen(lineNumberBuff));

	// Loop over each piece in table
	Piece* current = e->table->head;
	while(current != NULL){
		const char* txt = PIECE_GET_BUFFER(e->table, current);

		for(int i = 0; i < current->length; i++){
			char c = txt[current->start + i];

			if(c == '\t'){
                // Handle tabs by adding the appropriate number of spaces
                int tabSpaces = TAB_WIDTH - (charactersInLine % TAB_WIDTH);
                for (int j = 0; j < tabSpaces; j++) {
                    dyarr_append(&buff, ' ');
                    charactersInLine++;
                }
			}
			else if( c == '\n'){
				lineNumber++;
				charactersInLine = 0;

				// ANSI code to reset cursor postion to start of line
				dyarr_append_multiple(&buff, "\033[1G",strlen("\033[1G") );
				dyarr_append_multiple(&buff, "\033[1B",strlen("\033[1B") );

				// Add line number for the new row
				char lineNumberBuff[32];

				if(lineNumber >= 10){
					snprintf(lineNumberBuff, sizeof(lineNumberBuff), "\033[48;5;236;38;5;248m %d \033[0m", lineNumber);
				} else {
					snprintf(lineNumberBuff, sizeof(lineNumberBuff), "\033[48;5;236;38;5;248m  %d \033[0m", lineNumber);
				}
				dyarr_append_multiple(&buff, lineNumberBuff, strlen(lineNumberBuff));
				
			}else {

                // Skip characters outside of the screen columns offset
                if(charactersInLine >= e->col_offset && charactersInLine < e->col_offset + e->screen_cols - e->line_number_offset){
                    dyarr_append(&buff, c);
                }
                charactersInLine++;
			} 

		}		
		current = current->next;
	}

    // Append the ANSI code to reset the cursor to editor cx, cy
    char cursor_reset[32];
    snprintf(cursor_reset, sizeof(cursor_reset), "\033[%d;%dH", e->cy + 1- e->row_offset, e->cx + 1-e->col_offset);
    dyarr_append_multiple(&buff, cursor_reset, strlen(cursor_reset));

    printf("%.*s", buff.size, buff.items);
    fflush(stdout);

	dyarr_free(&buff);

	return 0;
}


