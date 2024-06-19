#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dyarray.h"
#include "piece_table.h"



Piece* createPiece(BufferType type, int start, int length){
	Piece* new_piece = malloc(sizeof(Piece));
	if(!new_piece) return NULL;
	new_piece->type = type;
	new_piece->start = start;
	new_piece->length = length;
	new_piece->next = NULL;

	// Cache line break positions in Piece
	dyarr_init(&new_piece->line_breaks, 2);
	return new_piece;
}

PieceTable* createPieceTable(const char* initial_text){
	PieceTable* table = malloc(sizeof(PieceTable));
	if(!table) return NULL;

	// Save original text
	table->original_buff = strdup(initial_text);
	if(!table->original_buff) return NULL;

	// Allocate add buffer memory
	table->add_buff = malloc(sizeof(char) * INITIAL_ADD_BUFF_CAP);
	if(!table->add_buff) return NULL;
	table->add_buff_cap = INITIAL_ADD_BUFF_CAP;
	table->add_buff_size = 0;



	// Initial Head of Linked List
	int initial_text_length = strlen(initial_text); 
	table->head = createPiece(ORIGINAL, 0, initial_text_length);
	if(!table->head) return NULL;
	pieceUpdateLineBreaks(table, table->head);

	return table;
}


void pieceFree(Piece* p){
	dyarr_free(&p->line_breaks);
	free(p);
}

void pieceUpdateLineBreaks(PieceTable* pt, Piece* piece){
	char* buff = PIECE_GET_BUFFER(pt, piece);

	for(int i = 0; i < piece->length; i++){
		char c = buff[piece->start + i];
		if(c == '\n'){
			dyarr_append(&piece->line_breaks, i);
		}
	}
}

void printPiece(PieceTable* pt, Piece* current){
		if(!current){
			printf("(NULL)\n");
			return;
		}

		printf("(type=%s, start=%d, len=%d, lb=%d, str='%.*s')\n", (current->type == ADD ? "ADD" : "ORIGINAL"), current->start, current->length, current->line_breaks.size, current->length, (current->type == ADD ? pt->add_buff + current->start : pt->original_buff + current->start));
}

void pieceTablePrintPieces(PieceTable* pt){
	
	Piece* current = pt->head;
	int index = 0;
	while(current != NULL){
		printf("%d: (type=%s, start=%d, len=%d, lb=%d,  str='%.*s')\n", index, (current->type == ADD ? "ADD" : "ORIGINAL"), current->start, current->length, current->line_breaks.size, current->length, (current->type == ADD ? pt->add_buff + current->start : pt->original_buff + current->start));
		index++;
		current = current->next;
	}
}

void pieceTablePrint(PieceTable* pt){
	
	Piece* current = pt->head;
	while(current != NULL){
		printf("%.*s", current->length, (current->type == ADD ? pt->add_buff + current->start : pt->original_buff + current->start));
		current = current->next;
	}
	printf("\n");
}


int pieceTableDelete(PieceTable* pt, int position, int length){
	// Find piece that conatins position 
	Piece* current = pt->head;
	Piece* prev = NULL;
	int current_position = 0;
	// printf("Check Condiftion: %d < %d\n", current_position + current->length, position);
	while(current != NULL && current_position + current->length < position){
		// printf("INSIDE\n");
		current_position += current->length;
		prev = current;
		current = current->next;
	}



	int position_offset_in_piece = position - current_position;
	// printf("Deleted Curr(%d) Off(%d) Pos(%d) Len(%d): ", current_position, position_offset_in_piece, position, length);	

	while(current != NULL && length > 0){

		// Deletetion removes entire current piece
		// possible there is still more to delete in next piece
		if(position_offset_in_piece == 0 && length >= current->length){
			// printf("Remove Entire Piece (MORE?)\n");

			if(prev == NULL){
				// Current == Head
				pt->head = current->next;
			} else {
				// Somewhere in Linked List
				prev->next = current->next;
			}


			// Remove deleted length from total length
			length -= current->length;

			// Reset offset in pice to 0 for next piece (if more to delete)
			position_offset_in_piece = 0;

			// Move onto next pice to see if we need to delete more from there
			Piece* tmp = current;
			current = current->next;
			pieceFree(tmp);

		} else if(position_offset_in_piece == 0 && length < current->length){

			// printf("Begin -> Middle (BREAK)\n");

			// Deletion starts at begining of piece, stops somewhere in the middle of it
			// Move up starting position of current pice to remove the text
			current->start += length;
			current->length = current->length - length;

			pieceUpdateLineBreaks(pt, current);
			break;

		} else if(position_offset_in_piece > 0 && position_offset_in_piece + length >= current->length){
			// Deletion happens from somewhere in the middle of the piece
			// and goes at least to the end of the piece maybe into the next



			// printf("MIDDLE -> END (MORE?) %d\n", length);
			// printPiece(pt, current);

			// Remove deleted length from total length
			length -= current->length - position_offset_in_piece; 


			// Move back length of current to delete that text
			current->length = position_offset_in_piece;




			// printf("MIDDLE -> END (MORE?) %d\n", length);

			// Reset offset in pice to 0 for next piece (if more to delete)
			position_offset_in_piece = 0;


			pieceUpdateLineBreaks(pt, current);

			// Move onto next pice to see if we need to delete more from there
			prev = current;
			current = current->next;


		} else if(position_offset_in_piece > 0 && position_offset_in_piece + length < current->length){
			// Deletion happens in the middle of a piece
			// This requires we split the piece up into two parts
			// Left: Before the delete text
			// Right: After the deleted text
			// printf("MIDDLE -> Middle (BREAK)\n");
			Piece* left = createPiece(current->type, current->start, position_offset_in_piece);
			Piece* right =  createPiece(current->type, position_offset_in_piece + length, current->length - length);
			if (!left || !right) return -1; // Memory allocation failed

			pieceUpdateLineBreaks(pt, left);
			pieceUpdateLineBreaks(pt, right);


			Piece* tmp = current; // Save so we can free
			left->next = right;
			right->next = current->next;

			if(prev == NULL){
				// current == head
				pt->head = left;
			} else {
				// Somewhere in Linked List
				prev->next = left;
			}

			pieceFree(tmp);
			break;
		}


	}

	return 0;
}


int pieceTableInsert(PieceTable* pt, int insert_position, char* text){
	// Ensure 'ADD' buffer has sufficient space for text
	int text_length = strlen(text);
	if(pt->add_buff_size + text_length >= pt->add_buff_cap){
		// Resize 'ADD' buffer
		pt->add_buff_cap *= 2;
		pt->add_buff = realloc(pt->add_buff, pt->add_buff_cap);
	}

	// Add text to 'ADD' buffer
	int new_piece_start = pt->add_buff_size;
	strncpy(pt->add_buff + pt->add_buff_size, text, text_length);
	pt->add_buff_size += text_length;

	// DEBUG_LOG("START -- PT Inserting: '%s' at %d\n", text, insert_position);

	// Find piece in table that contains position
	Piece* current = pt->head;
	Piece* prev = NULL;
	int total_position = 0;
	while(current != NULL && total_position + current->length <= insert_position){
		total_position += current->length;
		prev = current;
		current = current->next;
	}

	// DEBUG_LOG("Previous Piece: ");
	// printPiece(pt, prev);
	// DEBUG_LOG("Current Piece: ");
	// printPiece(pt, current);

	// Create new Piece to point to this new text
	Piece* new_piece = createPiece(ADD, new_piece_start, text_length);
	pieceUpdateLineBreaks(pt, new_piece);
	// DEBUG_LOG("New Piece: ");
	// printPiece(pt, new_piece);

	// Position is greater than total characters in table
    if (current == NULL) {
		// DEBUG_LOG("Current == NULL: Position greater than total characterss in table\n");
		if(prev == NULL){
			// Head of table is empty
            pt->head = new_piece;
		} else {
			// Add piece after previous
			// (Appends to end of linked list)
			new_piece->next = prev->next;
			prev->next = new_piece;
		} 

		return 0;
	}


	int position_offset_in_piece = insert_position - total_position;
	// DEBUG_LOG("META: total_pos=%d, offset=%d\n", total_position, position_offset_in_piece);


	// Insertion happens in the middle of a piece
	if(position_offset_in_piece > 0){
        // Split the current piece into left and right sections
        // Left section: everything before the new text position
        // Right section: everything after the new text position
        // New piece is inserted between the left and right sections
		// DEBUG_LOG("SPLITTING piece (left, right)\n");
		Piece* left  = createPiece(current->type, current->start, position_offset_in_piece);
		Piece* right = createPiece(current->type, current->start +  position_offset_in_piece, current->length - position_offset_in_piece);
        if (!left || !right) return -1; // Memory allocation failed
        pieceUpdateLineBreaks(pt, left);
        pieceUpdateLineBreaks(pt, right);

		// Update pointers to insert the 3 pieces (left, new, right)
		left->next = new_piece;
		new_piece->next = right;
		right->next = current->next;

        if (prev == NULL) {
			// Head of table is empty
            pt->head = left;    
        } else {
            prev->next = left;
        }


        pieceFree(current);

	} else {
		// No need to split, just insert the new piece before the current piece
		new_piece->next = current;
        if (prev == NULL) {
            pt->head = new_piece;
        } else {
            prev->next = new_piece;
        }
	}

}




