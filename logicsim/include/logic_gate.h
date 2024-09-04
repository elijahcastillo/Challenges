#ifndef LOGIC_GATE_H
#define LOGIC_GATE_H

#include "draggable_rect.h"
#include "truth_table.h"
#include <stdbool.h>



typedef enum {
	HIGH,
	LOW
} TerminalState;

typedef enum {
	INPUT,
	OUTPUT
} TerminalType;


// Pre-declare LogicGate so it can be referenced in GateTerminal
typedef struct LogicGate LogicGate;

typedef struct {
	LogicGate* gate;
	TerminalState state;
	TerminalType type;
	int index;

	// Only Visual
	Vector2 pos;
	float radius; 
} GateTerminal;


typedef struct Connection {
    GateTerminal* from;
    GateTerminal* to;
    Vector2 controlPointStart;  // Bezier control point
    Vector2 controlPointEnd;    // Bezier control point
} Connection;



struct LogicGate{
	char* name;
	DraggableRect dr;

	int numInputs;
	int numOutputs;
	GateTerminal* inputs;
	GateTerminal* outputs;

	TruthTable* truthTable;

    bool isDraggingConnection;
    Vector2 connectionStart;
};









LogicGate* logicGateInit(char* name, int initialX, int initialY, int inputs, int outputs);
void logicGateRender(LogicGate* lg);
void fillTruthTableForStandardGates(LogicGate* lg);

GateTerminal gateTerminalInit(LogicGate* gate, TerminalType type, int index);
void updateTerminalPosition(GateTerminal* term);
void gateTerminalRender(GateTerminal* term);

#endif // LOGIC_GATE_H

