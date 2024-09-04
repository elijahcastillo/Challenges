#include "logic_gate.h"
#include "utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


// void checkMouseInteractionWithTerminals(LogicGate* lg) {
//     Vector2 mousePos = GetMousePosition();
//
//     // Check for terminal interaction
//     for (int i = 0; i < lg->numInputs; i++) {
//         if (CheckCollisionPointCircle(mousePos, lg->inputs[i].pos, lg->inputs[i].radius) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//             lg->tempConnection.from = &lg->inputs[i];
//             lg->isDraggingConnection = true;
//             break;
//         }
//     }
//
//     // If dragging, update the temporary connection
//     if (lg->isDraggingConnection) {
//         lg->tempConnection.controlPointStart = mousePos;
//         lg->tempConnection.controlPointEnd = mousePos;
//
//         if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
//             for (int i = 0; i < lg->numOutputs; i++) {
//                 if (CheckCollisionPointCircle(mousePos, lg->outputs[i].pos, lg->outputs[i].radius)) {
//                     lg->tempConnection.to = &lg->outputs[i];
//                     // Add the connection to the list of connections
//                     addConnection(lg, lg->tempConnection);
//                     break;
//                 }
//             }
//             lg->isDraggingConnection = false;
//         }
//     }
// }
//
// void logicGateRenderConnections(LogicGate* lg) {
//     for (int i = 0; i < lg->numConnections; i++) {
//         Connection conn = lg->connections[i];
//         Vector2 points[4] = {conn.from->pos, conn.controlPointStart, conn.controlPointEnd, conn.to->pos};
//         DrawSplineBezierCubic(points, 4, 2.0f, RED);  // Adjust color and thickness as needed
//     }
//
//     // Draw temporary connection if dragging
//     if (lg->isDraggingConnection) {
//         Vector2 tempPoints[4] = {lg->tempConnection.from->pos, lg->tempConnection.controlPointStart, lg->tempConnection.controlPointEnd, GetMousePosition()};
//         DrawSplineBezierCubic(tempPoints, 4, 2.0f, DARKGRAY);  // Temporary connection visual
//     }
// }

// =============================


LogicGate* logicGateInit(char* name, int initialX, int initialY, int inputs, int outputs){
	LogicGate* lg = malloc(sizeof(LogicGate));
	if(!lg) return NULL;
	lg->name = name;
	lg->dr = draggableRectInit((Rectangle){initialX, initialY, 200, 100});

	lg->numInputs = inputs;
	lg->numOutputs = outputs;
	lg->inputs = malloc(sizeof(GateTerminal) * inputs);
	lg->outputs = malloc(sizeof(GateTerminal) * outputs);
	if(!lg->inputs || ! lg->outputs) return NULL;

	// Create Terminals for Logic Gate
	for(int i = 0; i < inputs; i++){
		lg->inputs[i] = gateTerminalInit(lg, INPUT, i);
	}

	for(int i = 0; i < outputs; i++){
		lg->outputs[i] = gateTerminalInit(lg, OUTPUT, i);
	}

    lg->truthTable = truthTableInit(inputs, outputs);
    if (!lg->truthTable) {
        free(lg->inputs);
        free(lg->outputs);
        free(lg);
        return NULL;
    }
	fillTruthTableForStandardGates(lg);

    int maxConnections = inputs > outputs ? inputs : outputs;
    lg->dr.rect.height = 40 + maxConnections * 40; // Adjust the height based on the max connections
	lg->isDraggingConnection = false;

	return lg;
}

void logicGateFillTruthTable(LogicGate* lg, TTEntry* entries, size_t entryCount){
    for (size_t i = 0; i < entryCount; i++) {
        truthTableAddEntry(lg->truthTable, entries[i].inp, entries[i].out);
    }
}


void fillTruthTableForStandardGates(LogicGate* lg) {
    TTEntry* entries = NULL;
    size_t entryCount = 0;

    if (strcmp(lg->name, "AND") == 0 && lg->numInputs == 2) {
        static TTEntry andEntries[] = {
            {0b00, 0b0},
            {0b01, 0b0},
            {0b10, 0b0},
            {0b11, 0b1}
        };
        entries = andEntries;
        entryCount = sizeof(andEntries) / sizeof(andEntries[0]);
    } else if (strcmp(lg->name, "OR") == 0 && lg->numInputs == 2) {
        static TTEntry orEntries[] = {
            {0b00, 0b0},
            {0b01, 0b1},
            {0b10, 0b1},
            {0b11, 0b1}
        };
        entries = orEntries;
        entryCount = sizeof(orEntries) / sizeof(orEntries[0]);
    } else if (strcmp(lg->name, "NOT") == 0 && lg->numInputs == 1) {
        static TTEntry notEntries[] = {
            {0b0, 0b1},
            {0b1, 0b0}
        };
        entries = notEntries;
        entryCount = sizeof(notEntries) / sizeof(notEntries[0]);
    } else if (strcmp(lg->name, "XOR") == 0 && lg->numInputs == 2) {
        static TTEntry xorEntries[] = {
            {0b00, 0b0},
            {0b01, 0b1},
            {0b10, 0b1},
            {0b11, 0b0}
        };
        entries = xorEntries;
        entryCount = sizeof(xorEntries) / sizeof(xorEntries[0]);
    } else if (strcmp(lg->name, "NAND") == 0 && lg->numInputs == 2) {
        static TTEntry nandEntries[] = {
            {0b00, 0b1},
            {0b01, 0b1},
            {0b10, 0b1},
            {0b11, 0b0}
        };
        entries = nandEntries;
        entryCount = sizeof(nandEntries) / sizeof(nandEntries[0]);
    }

    // If entries are assigned, add them to the truth table
    if (entries && entryCount > 0) {
		logicGateFillTruthTable(lg, entries, entryCount);
    }
}

void logicGateUpdateState(LogicGate* lg){
	uint32_t inputs = 0;	
	for(int i = 0; i < lg->numInputs; i++){
		if(lg->inputs[i].state == HIGH){
			inputs |= (1U << i); 
		}
	}

    TraceLog(LOG_INFO, "Updating Logic Gate: %s", lg->name);
    TraceLog(LOG_INFO, "Input States: 0x%X", inputs);

    uint32_t outputState = truthTableFind(lg->truthTable, inputs);

TraceLog(LOG_INFO, "Output States Before Update: 0x%X", outputState);

    // Update the state of each output terminal based on the output bits
    for (int i = 0; i < lg->numOutputs; i++) {
        lg->outputs[i].state = (outputState & (1U << i)) ? HIGH : LOW;
TraceLog(LOG_INFO, "Output %d changed to:", i);
    }
}


void logicGateRender(LogicGate* lg) {
    draggableRectUpdate(&lg->dr);
    DrawRectangleRec(lg->dr.rect, LIGHTGRAY);
    drawResponsiveText(lg->name, lg->dr.rect, 0.5, 0.4, 0.5, 0.5, 0.1);

	for(int i = 0; i < lg->numInputs; i++){
		updateTerminalPosition(&lg->inputs[i]);
		gateTerminalRender(&lg->inputs[i]);
	}

	for(int i = 0; i < lg->numOutputs; i++){

		updateTerminalPosition(&lg->outputs[i]);
		gateTerminalRender(&lg->outputs[i]);
	}



}





GateTerminal gateTerminalInit(LogicGate* gate, TerminalType type, int index){
	GateTerminal term;
	term.gate = gate;
	term.type = type;
	term.index = index;
	term.state = LOW;

	float spacing = gate->dr.rect.height / (type == INPUT ? gate->numInputs : gate->numOutputs + 1);
    term.pos = (type == INPUT) ? 
        (Vector2){gate->dr.rect.x, gate->dr.rect.y + spacing * (index + 1)} :
        (Vector2){gate->dr.rect.x + gate->dr.rect.width, gate->dr.rect.y + spacing * (index + 1)};

	return term;
}

void updateTerminalPosition(GateTerminal* term) {
        LogicGate* gate = term->gate;

        float spacing = gate->dr.rect.height / (term->type == INPUT ? gate->numInputs + 1 : gate->numOutputs + 1);
        term->pos = (term->type == INPUT) ? 
            (Vector2){gate->dr.rect.x- 10, gate->dr.rect.y + spacing * (term->index + 1)} :
            (Vector2){gate->dr.rect.x + gate->dr.rect.width + 10, gate->dr.rect.y + spacing * (term->index + 1)};
	
		term->radius = gate->dr.rect.height / (gate->numInputs + gate->numOutputs + 6);
}


void gateTerminalRender(GateTerminal* term){
        Vector2 mousePos = GetMousePosition();
		bool isHovering = CheckCollisionPointCircle(mousePos, term->pos, term->radius);

		Color color = term->state == HIGH ? GREEN : RED;
		// color = isHovering ? BLUE : color;
		DrawCircleV(term->pos, term->radius, color);


		if (isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if(term->type == INPUT) term->state = term->state == HIGH ? LOW : HIGH;
			logicGateUpdateState(term->gate);
		}
		//  
		// if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
		// 	manager->isDrawingConnection = false;
		// }
}



