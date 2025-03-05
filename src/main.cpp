#include "raylib.h"
#include "Mesh.h"

void DrawCoordSystem(void);

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "3D Cloth Simulation");

    Camera camera = { { 15.0f, 5.0f, 15.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };

    mesh3d::Mesh cloth(10, 10, 1.0f);  // 10x10 cloth

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        //UpdateCamera(&camera, CAMERA_FREE);
        cloth.Update(GetFrameTime() * 0.1f);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
		DrawCoordSystem();
        cloth.Draw();
        EndMode3D();
        EndDrawing();
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

