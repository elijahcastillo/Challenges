#ifndef CONNECTION_H
#define CONNECTION_H
#include "logic_gate.h"

typedef enum {
	HIGH,
	LOW
} TerminalState

typedef struct {
	LogicGate* gate;
	TerminalState state;
} GateTerminal;


GateTerminal gateTerminalInit(LogicGate* gate){
	GateTerminal term;
	term->gate = gate;
	term->state = LOW;
}

#endif // CONNECTION_H
