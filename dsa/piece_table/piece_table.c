#include "piece_table.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



// =====================
//   Private Functions
// =====================

// Function to escape and print special characters
void _escapeAndPrint(const char* str, int length) {
    for (int i = 0; i < length; i++) {
        if (str[i] == '\n') {
            printf("\\n");
        } else if (str[i] == '\t') {
            printf("\\t");
        } else if (str[i] == '\\') {
            printf("\\\\");
        } else {
            putchar(str[i]);
        }
    }
}

int _initPiece(Piece* p, BufferType type, int start, int length){
		p->type = type;
		p->start = start;
		p->length = length;
		p->next = NULL;
		return 0;
}

void _printPieceContent(PieceTable* pt, struct Piece* p) {
    if (p == NULL) {
        printf("'EMPTY'\n");
    } else {
        printf("'");
        _escapeAndPrint(PIECE_GET_BUFFER(pt, p) + p->start, p->length);
        printf("'\n");
    }
}

void _printTableContent(PieceTable* pt) {
	Piece* current = pt->head;

    printf("'");
	while(current != NULL){
        _escapeAndPrint(PIECE_GET_BUFFER(pt, current) + current->start, current->length);
		current = current->next;
	}
    printf("'\n\n");
}

void _printPieceInfo(Piece* p){
	p == NULL ? printf("Piece(NULL)\n") : printf("Piece(type=%s, start='%d', length='%d')\n", p->type == ADD ? "ADD" : "ORIGINAL", p->start, p->length);

}




// =====================
//   Public Functions
// =====================


int pieceTableInit(PieceTable* pt, char* originalText){
	// Add original text to buffer
	pt->originalBuffer = strdup(originalText);
	if(!pt->originalBuffer) return -1;

	// Initialize 'add' buffer
	pt->addBuffer = malloc(sizeof(char) * PIECE_TABLE_ADD_BUFF_CAP);
	if(!pt->addBuffer) return -1;
	pt->addBufferSize = 0;
	pt->addBufferCap = PIECE_TABLE_ADD_BUFF_CAP;

	// Create head of linked list
	pt->head = malloc(sizeof(Piece));
	if(!pt->head) return -1;
	_initPiece(pt->head, ORIGINAL, 0, strlen(originalText));
	
	return 0;
}

void printPieceTableInfo(PieceTable* pt){
	Piece* current = pt->head;
	int index = 0;

	printf("Piece Table Info\n");
	printf("================\n");
	while(current != NULL){
		printf("Index %d: ", index);
		_printPieceInfo(current);
		_printPieceContent(pt, current);
		printf("\n");
		current = current->next;
		index++;
	}
	printf("All Content = ");
	_printTableContent(pt);
}


int pieceTableDelete(PieceTable* pt, int startPosition, int length){
	// Find piece in linked list to delete from
	Piece* current = pt->head;
	Piece* prev = NULL;
	int totalPieceTableIndex = 0;
	while(current != NULL && totalPieceTableIndex + current->length < startPosition){
		totalPieceTableIndex += current->length;
		prev = current;
		current = current->next;
	}

	int relativeIndexInsidePiece = startPosition - totalPieceTableIndex;


	while(current != NULL && length > 0){

		// Case 1: Remove entire current piece (Might be more in next piece)
		if(relativeIndexInsidePiece == 0 && length >= current->length){
			if(prev == NULL){
				// current == head
				pt->head = current->next;
			} else {
				// somewhere in linked list
				prev->next = current->next;
			}

			// Remove deleted length from total length
			length -= current->length;

			// Reset offset in pice to 0 for next piece (if more to delete)
			relativeIndexInsidePiece = 0;

			// Delete piece
			Piece* removedPiece = current;	
			free(removedPiece);
			current = current->next;

		} else if (relativeIndexInsidePiece == 0 && length < current->length){

			// Case 2: Deletion starts at begining of piece, stops somewhere in the middle of it
			// Move current piece start up to soft delete that text
			current->start += length;
			current->length -= length;
			return 0;
		} else if(relativeIndexInsidePiece > 0 &&  relativeIndexInsidePiece + length >= current->length){
			// Case 3: Deletion happens from somewhere in the middle of the piece
			//	Goes at least to the end of the piece maybe into the next

            // Move the length of the current piece down to soft remove text
            int remainingLength = current->length - relativeIndexInsidePiece;
            length -= remainingLength;
            current->length = relativeIndexInsidePiece;

			// Reset offset in pice to 0 for next piece (if more to delete)
			relativeIndexInsidePiece = 0;

			// Move to next piece in table
			prev = current;
			current = current->next;
		} else if (relativeIndexInsidePiece > 0 &&  relativeIndexInsidePiece + length < current->length){

			// Case 4: Deletion happens within in the middle of a piece
			// This requires we split the piece up into two parts
			//  Left: Before the delete text
			//  Right: After the deleted text
			Piece* left = malloc(sizeof(Piece));
			Piece* right = malloc(sizeof(Piece));
			if (!left || !right) return -1;
			_initPiece(left, current->type, current->start, relativeIndexInsidePiece);
			_initPiece(right, current->type, current->start + relativeIndexInsidePiece + length, current->length - (relativeIndexInsidePiece + length));


			Piece* removedPiece = current;	
			left->next = right;
			right->next = current->next;

			if(prev == NULL){
				// current == head
				pt->head = left;
			} else {
				// Somewhere in Linked List
				prev->next = left;
			}

			free(removedPiece);
			return 0;

		}

	}

	return 0;
}



// Inserts AFTER character at insertPosition (if 1-based)
// Inserts BEFORE character at insertPosition (if 0-based)
int pieceTableInsert(PieceTable* pt, int insertPosition, const char* newText){

	int newTextLength = strlen(newText);

	// Check if 'ADD' buffer needs to be resized
	if(pt->addBufferSize + newTextLength >= pt->addBufferCap){
		pt->addBufferCap *= 2;
		pt->addBuffer = realloc(pt->addBuffer, pt->addBufferCap);	
		if(!pt->addBuffer) return -1; // realloc failed
	}

	// Add newText to 'ADD' buffer
	int newPieceStartIndex = pt->addBufferSize;
	strncpy(pt->addBuffer + pt->addBufferSize, newText, newTextLength);
	pt->addBufferSize += newTextLength;


	// Initialize new piece for table
	Piece* newPiece = malloc(sizeof(Piece));
	if(!newPiece) return -1;
	_initPiece(newPiece, ADD, newPieceStartIndex, newTextLength);


	
	// Find spot in linked list to insert newPiece 
	Piece* current = pt->head;
	Piece* prev = NULL;
	int totalPieceTableIndex = 0;
	while(current != NULL && totalPieceTableIndex + current->length < insertPosition){
		totalPieceTableIndex += current->length;
		prev = current;
		current = current->next;
	}

	// printf("Finding Spot in linked list: \n");
	// printf("Prev = ");
	// _printPieceInfo(prev);
	// _printPieceContent(pt, prev);
	//
	// printf("\nCurrent = ");
	// _printPieceInfo(current);
	// _printPieceContent(pt, current);


	// Case: insertPostion at end of Piece Table text
	if(current == NULL){
		if(prev != NULL) prev->next = newPiece;
		return 0;
	}


	// Case: insertPostion is inside of a Piece 
	int relativeIndexInsidePiece = insertPosition - totalPieceTableIndex;
	if(relativeIndexInsidePiece > 0){

		// Edge Case: insertPosition is at end of current piece, no need to split
		if(relativeIndexInsidePiece == current->length){
			newPiece->next = current->next;
			current->next = newPiece;
			return 0;
		}


		// Create new piece for text in current piece after insertPosition
		Piece* splitPiece = malloc(sizeof(Piece));
		if(!splitPiece) {
			free(newPiece);
			return -1;
		}
		_initPiece(splitPiece, current->type, current->start + relativeIndexInsidePiece, current->length - relativeIndexInsidePiece);

		// Reduce current Piece length to text before insert position
		current->length = relativeIndexInsidePiece;

		// Update points to insert newPiece between current and splitPiece (current -> newPiece -> splitPiece)
		newPiece->next = splitPiece;
		splitPiece->next = current->next;
		current->next = newPiece;


		return 0;
	}

    // Case: insert at the beginning or exact match with the start of a piece
    if(prev != NULL) {
        prev->next = newPiece;
        newPiece->next = current;
    } else {
        newPiece->next = pt->head;
        pt->head = newPiece;
    }

	return 0;

}





