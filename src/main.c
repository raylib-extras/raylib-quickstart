/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// Utility header for SearchAndSetResourceDir

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");
	
	// Game loop
	while (!WindowShouldClose())		// Run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// Drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// Draw some text using the default font
		DrawText("Hello Raylib", 200,200,20,WHITE);

		// Draw our texture to the screen
		DrawTexture(wabbit, 400, 200, WHITE);
		
		// End the frame and get ready for the next one (display frame, poll input, etc...)
		EndDrawing();
	}

	// Cleanup
	// Unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// Destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
