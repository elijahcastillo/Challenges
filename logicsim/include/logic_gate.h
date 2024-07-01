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

	// Only Visual
	Vector2 pos;
	int index;
	float radius; 
} GateTerminal;





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

