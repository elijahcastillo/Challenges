#include "draggable_rect.h"


DraggableRect draggableRectInit(Rectangle rect){
	DraggableRect dr;
	dr.rect = rect;
	dr.offset = (Vector2){ 0, 0 };
	dr.isDragging = false;
	return dr;
}

void draggableRectUpdate(DraggableRect* dr){
	Vector2 mousePos = GetMousePosition();

	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
		if(CheckCollisionPointRec(mousePos, dr->rect)){
			dr->isDragging = true;
			dr->offset.x = mousePos.x - dr->rect.x;
			dr->offset.y = mousePos.y - dr->rect.y;
		}
	}

	if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
		dr->isDragging = false;
	}

    if (dr->isDragging) {
        dr->rect.x = mousePos.x - dr->offset.x;
        dr->rect.y = mousePos.y - dr->offset.y;
    }
}
