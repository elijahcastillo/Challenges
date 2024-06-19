#include "raylib.h"
#include "logic_gate.h"
#include "gate_terminal_manager.h"

int main(void) {
    const int screenWidth = 1900;
    const int screenHeight = 1250;

    InitWindow(screenWidth, screenHeight, "Logic Gate Simulator");

    GateTerminalManager* termManager = gateTerminalManagerInit();

    LogicGate* lg1 = logicGateInit("AND", 100, 100, 3, 1);
    LogicGate* lg2 = logicGateInit("OR", 400, 200, 2, 1);
    gateTerminalManagerAdd(termManager, lg1);
    gateTerminalManagerAdd(termManager, lg2);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
		Vector2 mousePos = GetMousePosition();

        logicGateRender(lg1);
        logicGateRender(lg2);
        gateTerminalManagerRender(termManager);

		if(termManager->isDrawingConnection){
			TraceLog(LOG_INFO, "NOPE");
			DrawLineV(termManager->connectionStart, mousePos, BLACK);	
		}

        EndDrawing();
		//test
    }

    CloseWindow(); // Close window and OpenGL context
    return 0;
}
