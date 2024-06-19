#ifndef EDITOR_PIECE_TABLE_H
#define EDITOR_PIECE_TABLE_H



typedef enum {
	ORIGINAL,
	ADD
} BufferType;


#define INITIAL_ADD_BUFF_CAP 10
#define PIECE_GET_BUFFER(table, piece) ((piece->type) == (ADD) ? (table->add_buff) : (table->original_buff))


typedef struct {
	int* items;
	int capacity;
	int size;
} LineBreaks;

typedef struct {
	BufferType type;		// Which Buffer slice is part of
	int start;				// Index of first charcter (0 based)
	int length;				// Length of the slice (number of characters)
	struct Piece* next;		// Next Piece in Linked List
	LineBreaks line_breaks;
} Piece;

typedef struct {
	char* original_buff;	// Original Text loaded in
	char* add_buff;			// Append-only buffer for new text
	int add_buff_cap;
	int add_buff_size;	
	Piece* head;			// Start of Linked List of Pieces
} PieceTable;


PieceTable* createPieceTable(const char* initial_text);
Piece* createPiece(BufferType type, int start, int length);

void pieceTablePrintPieces(PieceTable* pt);
void pieceTablePrint(PieceTable* pt);

int pieceTableInsert(PieceTable* pt, int position, char* text);
int pieceTableDelete(PieceTable* pt, int position, int length);

int insert_pt(PieceTable* pt, int inser_position, char* text);

#endif // EDITOR_PIECE_TABLE_H
