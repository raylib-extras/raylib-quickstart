/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include "math.h"

const int render_w = 320;
const int render_h = 240;
const int window_scale = 2;
const int window_w = render_w * window_scale;
const int window_h = render_h * window_scale;

int main()
{
	SetTraceLogLevel(LOG_WARNING);

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// SetTargetFPS(120);

	// Create the window and OpenGL context
	InitWindow(window_w, window_h, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Image baby = LoadImage("baby.png");
	Image baby_flipped = ImageCopy(baby);
	ImageFlipHorizontal(&baby_flipped);

	Image render = GenImageColor(render_w, render_h, BLACK);
	ImageFormat(&render, PIXELFORMAT_UNCOMPRESSED_R5G6B5);

	// game loop
	int ticks = 0;
	float spins = 0;
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		spins += GetFrameTime();
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// reset image
		ImageClearBackground(&render, SKYBLUE);

		ImageDrawText(&render, "Hello Raylib", 0, 0, 20, WHITE);

		// draw onto image
		float tlx = Remap(-fabsf(sinf(spins * PI)), -1, 1, 0, baby.width);
		float w = Remap(fabsf(sinf(spins * PI)), -1, 1, -baby.width, baby.width);
		if (w >= 5.0f)
		{
			ImageDraw(&render,
								(sinf(spins * PI) > 0 ? baby : baby_flipped),
								(Rectangle){0, 0, baby.width, baby.height},
								(Rectangle){
										tlx,
										0,
										w,
										baby.height},
								WHITE);
		}

		Texture2D tex = LoadTextureFromImage(render);
		SetTextureFilter(tex, TEXTURE_FILTER_POINT);
		DrawTextureEx(tex, (Vector2){0, 0}, 0, window_scale, WHITE);
		DrawFPS(0, 0);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
		UnloadTexture(tex);

		++ticks;
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadImage(render);
	UnloadImage(baby);
	UnloadImage(baby_flipped);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
