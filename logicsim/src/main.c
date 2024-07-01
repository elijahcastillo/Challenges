#include "raylib.h"
#include "logic_gate.h"

int main(void) {
    const int screenWidth = 1900;
    const int screenHeight = 1250;

    InitWindow(screenWidth, screenHeight, "Logic Gate Simulator");


    LogicGate* lg1 = logicGateInit("AND", 100, 100, 2, 1);
    LogicGate* lg2 = logicGateInit("OR", 400, 200, 2, 1);

	// Logic gates should be in linked list, when you click on logic gate
	// move that node to the top of the linked list and render the linked list in order

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
		Vector2 mousePos = GetMousePosition();

        logicGateRender(lg1);
        logicGateRender(lg2);



        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
    return 0;
}
