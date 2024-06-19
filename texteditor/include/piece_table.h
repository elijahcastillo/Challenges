#ifndef PIECE_TABLE_H
#define PIECE_TABLE_H

#define PIECE_TABLE_ADD_BUFF_CAP 10
#define PIECE_GET_BUFFER(table, piece) ((piece->type) == (ADD) ? (table->addBuffer) : (table->originalBuffer))

typedef enum {
	ORIGINAL,
	ADD
} BufferType;

typedef struct Piece {
	int start;			// 0-based index into buffer
	int length;			// # of characters in piece 
	struct Piece* next;	// Next piece in linked list
	BufferType type;
} Piece;

typedef struct {
	char* originalBuffer;
	char* addBuffer;
	int addBufferSize;
	int addBufferCap;
	Piece* head;
} PieceTable;



int pieceTableInit(PieceTable* pt, char* originalText);

void printPieceTableInfo(PieceTable* pt);

int pieceTableInsert(PieceTable* pt, int insertPosition, char* newText);

int pieceTableDelete(PieceTable* pt, int startPosition, int length);


#endif // PIECE_TABLE_H
