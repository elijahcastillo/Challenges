#ifndef LOGIC_DRAGGABLE_SIM_H
#define LOGIC_DRAGGABLE_SIM_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
	Rectangle rect;
	Vector2 offset;
	bool isDragging;
} DraggableRect;

DraggableRect draggableRectInit(Rectangle rect);
void draggableRectUpdate(DraggableRect* dr);

#endif // LOGIC_DRAGGABLE_SIM_H
