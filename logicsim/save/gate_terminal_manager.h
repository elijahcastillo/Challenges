#ifndef GATE_TERMINAL_MANAGER_H
#define GATE_TERMINAL_MANAGER_H

#include "raylib.h"
#include "logic_gate.h"

typedef enum {
	HIGH,
	LOW
} TerminalState;

typedef enum {
	INPUT,
	OUTPUT
} TerminalType;

typedef struct {
	LogicGate* gate;
	TerminalState state;
	TerminalType type;
	Vector2 pos;
	int index;
	float radius; // Only Visual
} GateTerminal;



typedef struct {
	GateTerminal* terminals;	
	int terminalCount;
	int terminalCap;

	bool isDrawingConnection;
	Vector2 connectionStart;
} GateTerminalManager;

#define TERMINAL_INIT_CAP 10


GateTerminalManager* gateTerminalManagerInit();

GateTerminal gateTerminalInit(LogicGate* gate, TerminalType type, int index);

void gateTerminalRender(GateTerminal* term);

void gateTerminalManagerAdd(GateTerminalManager* manager, LogicGate* gate);

void gateTerminalManagerRender(GateTerminalManager* manager);


#endif // GATE_TERMINAL_MANAGER_H
