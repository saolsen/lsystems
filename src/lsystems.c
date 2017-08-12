// Setting up this project was a shit load of work. I need a better way.
// I have the raylib project referenced by this one and in the solution. That way it will be linked.
// I have to manually link to glfw3, openal doesn't even seem to be in the thing so I can't like that.
// I have to set includes and stuff so I can get at the source.
// The biggest issue I think is that I had to modify the raylib project to get it to see the right files.
// I also think they are like direct references not relative so this aint portable.
// Honestly vs projects suck a lot anyway.

// Maybe don't worry about it too much, I'm not really going for a releaseable artifact, just playing with code.

#include "raylib.h"
#include "raymath.h"

static int foo = 0;

// what is that splice thingy that is going on in the dudes stuff.
// is that about splicing buffers together? Building up big results? That's what I need to do.

void draw()
{

    //DrawText("What a beauty! holy frack!", 200, 300, 48, DARKGREEN);
    DrawFPS(GetScreenWidth()-80, 10);
    //DrawRectangle(200, 200, 100, 100, DARKBLUE);
    DrawLineEx((Vector2) { 0, 0 }, (Vector2) { 100, 100 }, 3, RED);
    DrawText(FormatText("Hello %s %i", "steben", foo), 300, 300, 32, GREEN);
    
}

int main()
{
    int ScreenWidth = 1024;
    int ScreenHeight = 768;

    InitWindow(ScreenWidth, ScreenHeight, "lsystems");
    
    ShowLogo();
    TraceLog(INFO, "screen is %ix%i\n", GetScreenWidth(), GetScreenHeight());
    
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            draw();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}