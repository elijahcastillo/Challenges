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
	editor->ptx = 0;
	editor->pty = 0;
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

typedef struct {
	int pos;
	char ch;
} CursorInfo;

CursorInfo _getCursorInfo(Editor* e){
	
    Piece* current = e->table->head;
	int pieceTablePos = 0;
	int currentRow = 0;
	int currentCol = 0;

    while (current != NULL) {
        const char* txt = PIECE_GET_BUFFER(e->table, current);

        for (int i = 0; i < current->length; i++) {
            char c = txt[current->start + i];

			if(currentRow == e->pty && currentCol == e->ptx){
				return (CursorInfo){pieceTablePos, c};	
			}

            if (c == '\n') {
                currentRow++;
                currentCol = 0;
                pieceTablePos++;
            } else {
                currentCol++;
                pieceTablePos++;
            }
		}

        current = current->next;
	}

	return (CursorInfo){pieceTablePos, '\0'};	
}

int _getLineLength(Editor* e, int targetRow) {
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
                } else  {
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


int editorInsertCharacter(Editor* e, char c) {
    CursorInfo cursorInfo = _getCursorInfo(e);

    char temp[2] = {c, '\0'};
    pieceTableInsert(e->table, cursorInfo.pos, temp);
    editorUpdateCursorPosition(e, e->ptx + 1, e->pty);
    return 0;
}

int editorDeleteCharacter(Editor* e, int length) {
    CursorInfo cursorInfo = _getCursorInfo(e);



    if (cursorInfo.pos > 0) {
		editorUpdateCursorPosition(e, e->ptx - 1, e->pty);

		// Move the position back by 1 to delete the character before the cursor
		pieceTableDelete(e->table, cursorInfo.pos - 1, length);
    }


    return 0;
}

int editorUpdateCursorPosition(Editor* e, int x, int y) {


    CursorInfo cursorInfo = _getCursorInfo(e);
	int totalLines = _getTotalLines(e);
	int newPositionLineLength = _getLineLength(e, y);


	if(x < 0){
		// Prevent cursor from going before begining of text
		x = 0;
	} else if (x >= newPositionLineLength){
		// Prevent cursor from going beyond end of line
		x = newPositionLineLength;
	}

    e->ptx = x;
    e->pty = y;
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

	// if(cursorInfo.ch == '\t'){
	// 	if(x < e->cx){
	//
	// 		e->cx -= TAB_WIDTH - 1;
	// 	} else {
	//
	// 		e->cx += TAB_WIDTH - 1;
	// 	}
	// }
    return 0;
}


int editorRenderText(Editor* e){
	RenderdText buff;
	dyarr_init(&buff, 20);

	int charactersInLine = 0;

    // Set cursor to bar style and start blinking
    dyarr_append_multiple(&buff, "\033[5 q", strlen("\033[5 q"));



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
				charactersInLine = 0;

				// ANSI code to reset cursor postion to start of line
				dyarr_append_multiple(&buff, "\033[1G",strlen("\033[1G") );
				dyarr_append_multiple(&buff, "\033[1B",strlen("\033[1B") );

				
			}else {

                // Skip characters outside of the screen columns offset
                if(charactersInLine >= e->col_offset && charactersInLine < e->col_offset + e->screen_cols){
                    dyarr_append(&buff, c);
                }
                charactersInLine++;

			} 

		}		
		current = current->next;
	}

    // Append the ANSI code to reset the cursor to editor cx, cy
    char cursor_reset[32];
    // snprintf(cursor_reset, sizeof(cursor_reset), "\033[%d;%dH", e->cy + 1- e->row_offset, e->cx + 1-e->col_offset);
	// +1 cause ANSI is 1-based
    snprintf(cursor_reset, sizeof(cursor_reset), "\033[%d;%dH", e->cy + 1 , e->cx + 1 );
    dyarr_append_multiple(&buff, cursor_reset, strlen(cursor_reset));

    printf("%.*s", buff.size, buff.items);
    fflush(stdout);

	dyarr_free(&buff);

	return 0;
}
