#include "raylib.h"
#include "Mesh.h"
#include <string>

const float ANIMATION_SPEED_STEP = 0.05f;

void DrawCoordSystem(void);

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
	float animationSpeed = 0.05f;
	std::string msg = "Press r to restart simulation";

	bool isRunning = false;

    InitWindow(screenWidth, screenHeight, "3D Cloth Simulation");

    Camera camera = { { 15.0f, 5.0f, 15.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };

    mesh3d::Mesh cloth(10, 10, 1.0f);  // 10x10 cloth

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) { isRunning = !isRunning; };

		// increase or decrease animation speed
		if (IsKeyPressed(KEY_UP)) { animationSpeed += ANIMATION_SPEED_STEP; }
		if (IsKeyPressed(KEY_DOWN)) { 
			if (animationSpeed > ANIMATION_SPEED_STEP) animationSpeed -= ANIMATION_SPEED_STEP;
		}
		
		// if window moves, then stop simulation
		if (IsWindowResized() || !IsWindowFocused) { isRunning = false; }

		// restart simulation
		if (IsKeyPressed(KEY_R)) { 
			cloth = mesh3d::Mesh(10, 10, 1.0f); 
			msg = "Reseted!";
		}

		if (isRunning) {
			//UpdateCamera(&camera, CAMERA_FREE);
			float dt = GetFrameTime() * animationSpeed;
			cloth.Update( dt );
		}

		//#region Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
		DrawCoordSystem();
        cloth.Draw();
        EndMode3D();

        // Text
		DrawText(isRunning ? "Running..." : "Paused (Press Enter/Space to continue)", 20, 20, 20, BLACK);
		DrawText(msg.c_str(), 20, 40, 20, BLACK);
		DrawText(("FPS: " + std::to_string(GetFPS())).c_str(), 20, 60, 20, BLACK);
		DrawText(std::to_string(animationSpeed).c_str(), 20, 80, 20, BLACK);

		EndDrawing();
		//#endregion
    }

    CloseWindow();
    return 0;
}

void DrawCoordSystem() {
    const float  AXIS_LENGTH = 10.0f;

	DrawGrid(10, 1.0f);

	DrawLine3D({ 0, 0, 0 }, { AXIS_LENGTH, 0, 0 }, MAROON); // x
    DrawLine3D({ 0, 0, 0 }, { -AXIS_LENGTH, 0, 0 }, LIGHTGRAY); // -x
	DrawLine3D({ 0, 0, 0 }, { 0, AXIS_LENGTH, 0 }, DARKGREEN); // y
	DrawLine3D({ 0, 0, 0 }, { 0, -AXIS_LENGTH, 0 }, LIGHTGRAY); // -y
	DrawLine3D({ 0, 0, 0 }, { 0, 0, AXIS_LENGTH }, DARKBLUE); // z
	DrawLine3D({ 0, 0, 0 }, { 0, 0, -AXIS_LENGTH }, LIGHTGRAY); // -z
}
