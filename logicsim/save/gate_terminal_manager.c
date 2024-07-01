
#include "gate_terminal_manager.h"
#include <stdlib.h>


GateTerminalManager* gateTerminalManagerInit(){
	GateTerminalManager* manager = malloc(sizeof(GateTerminalManager));
	if(!manager) return NULL;

	manager->terminalCount = 0;
	manager->terminalCap = TERMINAL_INIT_CAP;
	manager->terminals = malloc(sizeof(GateTerminal) * TERMINAL_INIT_CAP);
	if(!manager->terminals){
		free(manager);
		return NULL;
	}

	return manager;
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

void updateTerminalPositions(GateTerminalManager* manager) {
    for (int i = 0; i < manager->terminalCount; i++) {
        GateTerminal* term = &manager->terminals[i];
        LogicGate* gate = term->gate;

        float spacing = gate->dr.rect.height / (term->type == INPUT ? gate->numInputs + 1 : gate->numOutputs + 1);
        term->pos = (term->type == INPUT) ? 
            (Vector2){gate->dr.rect.x- 20, gate->dr.rect.y + spacing * (term->index + 1)} :
            (Vector2){gate->dr.rect.x + gate->dr.rect.width + 20, gate->dr.rect.y + spacing * (term->index + 1)};
	
		term->radius = gate->dr.rect.height / (gate->numInputs + gate->numOutputs + 6);
    }
}



void gateTerminalManagerAdd(GateTerminalManager* manager, LogicGate* gate){
	if(manager->terminalCount >= manager->terminalCap){
		manager->terminalCap *= 2;
		manager->terminals = realloc(manager->terminals, manager->terminalCap);
	}

	for(int i = 0; i < gate->numInputs; i++){
		manager->terminals[manager->terminalCount++] = gateTerminalInit(gate, INPUT, i);
	}

	for(int j = 0; j < gate->numOutputs; j++){
		manager->terminals[manager->terminalCount++] = gateTerminalInit(gate, OUTPUT, j);
	}

}


void gateTerminalManagerRender(GateTerminalManager* manager) {
	updateTerminalPositions(manager);
    for (int i = 0; i < manager->terminalCount; i++) {
		GateTerminal* term = &manager->terminals[i];
        Vector2 mousePos = GetMousePosition();
		bool isHovering = CheckCollisionPointCircle(mousePos, term->pos, term->radius);

		Color color = (term->type == INPUT) ? RED : BLUE;
		color = isHovering ? GREEN : color;
		DrawCircleV(term->pos, term->radius, color);

		if (isHovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			manager->isDrawingConnection = true;
			manager->connectionStart = term->pos;
		}
		 
		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			manager->isDrawingConnection = false;
		}
    }
}

