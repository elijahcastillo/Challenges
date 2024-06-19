#ifndef LOGIC_GATE_H
#define LOGIC_GATE_H

#include "draggable_rect.h"
#include <stdbool.h>

typedef struct {
	char* name;
	DraggableRect dr;
	int numInputs;
	int numOutputs;

	bool isDraggingConnection;
	Vector2 connectionStart;
} LogicGate;

typedef struct {
    bool isDrawingWire;
    Vector2 start;
    Vector2 end;
} WireState;





LogicGate* logicGateInit(char* name, int initialX, int initialY, int inputs, int outputs);

void logicGateRender(LogicGate* lg);




#endif // LOGIC_GATE_H

