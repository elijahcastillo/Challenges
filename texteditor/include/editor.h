#ifndef EDITOR_H
#define EDITOR_H

#include <termios.h>
#include "piece_table.h"


typedef enum {
	NORMAL,
	INSERT
} EditorMode; 

typedef struct {
	struct termios original_attributes;
	int cx, cy;						// Position of cursor on screen
	int row_offset, col_offset;		// Offset of rows and columns displayed on screen
	int screen_rows, screen_cols;	// # of rows and columns that we can show on sceren
	int line_number_offset;         // Offset from left of screen taken up by line numbers
	PieceTable* table				// Store the text on screen
} Editor;




typedef struct {
	char* items;
	int capacity;
	int size;
} RenderdText;

Editor* createEditor();
int editorEnableRawMode(Editor* e);
int editorDisableRawMode(Editor* e);
int editorUpdateScreenDimensions(Editor* e);
int editorRenderText(Editor* e);
int editorInitializeBuffer(Editor* e, char* text);
int editorInsertCharacter(Editor* e, char c);
int editorDeleteCharacter(Editor* e, int length);
int editorUpdateCursorPosition(Editor* e, int x, int y);

#endif // EDITOR_H

