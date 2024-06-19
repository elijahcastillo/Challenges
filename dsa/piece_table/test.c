#include "piece_table.h"
#include <stdio.h>
#include <stdlib.h>



int main(){
	PieceTable pt;
	pieceTableInit(&pt, "This is really awesome\nSo Cool");
	printPieceTableInfo(&pt);


	pieceTableInsert(&pt, 5, "lol");
	pieceTableInsert(&pt, 8, "elijah");

	printf("\n");
	printPieceTableInfo(&pt);

	pieceTableDelete(&pt, 4, 1);

	printf("\n");
	printPieceTableInfo(&pt);

}
