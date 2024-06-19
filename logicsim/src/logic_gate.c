#include "logic_gate.h"
#include "utils.h"
#include <stdlib.h>
#include <stdbool.h>




LogicGate* logicGateInit(char* name, int initialX, int initialY, int inputs, int outputs){
	LogicGate* lg = malloc(sizeof(LogicGate));
	if(!lg) return NULL;
	lg->name = name;
	lg->dr = draggableRectInit((Rectangle){initialX, initialY, 200, 100});
	lg->numInputs = inputs;
	lg->numOutputs = outputs;
	lg->isDraggingConnection = false;

    int maxConnections = inputs > outputs ? inputs : outputs;
    lg->dr.rect.height = 40 + maxConnections * 40; // Adjust the height based on the max connections
	

	return lg;
}






void logicGateRender(LogicGate* lg) {
    draggableRectUpdate(&lg->dr);
    DrawRectangleRec(lg->dr.rect, LIGHTGRAY);
    drawResponsiveText(lg->name, lg->dr.rect, 0.5, 0.4, 0.5, 0.5, 0.1);
}


// void gateTerminalRender(LogicGate* lg) {
//     float inputSpacing = lg->dr.rect.height / (lg->numInputs + 1);
//     float outputSpacing = lg->dr.rect.height / (lg->numOutputs + 1);
//     float circleRadius = lg->dr.rect.height / (lg->numInputs + lg->numOutputs + 6);
//     Vector2 mousePos = GetMousePosition();
//
//     for (int i = 0; i < lg->numInputs; i++) {
//         float y = lg->dr.rect.y + inputSpacing * (i + 1);
//         bool isHovering = CheckCollisionPointCircle(mousePos, (Vector2){lg->dr.rect.x, y}, circleRadius);
//         Color color = isHovering ? GREEN : RED;
//         DrawCircle(lg->dr.rect.x, y, circleRadius, color);
//
//         if (isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
// 			lg->isDraggingConnection = true;
//             lg->connectionStart = (Vector2){lg->dr.rect.x, y};
//         }
//     }
//
//     for (int i = 0; i < lg->numOutputs; i++) {
//         float y = lg->dr.rect.y + outputSpacing * (i + 1);
//         bool isHovering = CheckCollisionPointCircle(mousePos, (Vector2){lg->dr.rect.x + lg->dr.rect.width, y}, circleRadius);
//         Color color = isHovering ? GREEN : BLUE;
//         DrawCircle(lg->dr.rect.x + lg->dr.rect.width, y, circleRadius, color);
//
//         if (isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
// 			lg->isDraggingConnection = true;
//             lg->connectionStart = (Vector2){lg->dr.rect.x + lg->dr.rect.width, y};
//         }
//     }
//
// }
//



