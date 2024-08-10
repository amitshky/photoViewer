#include "raylib.h"

int main() {
    InitWindow(640, 640, "Photo Viewer");

    while(!WindowShouldClose())
    {
        BeginDrawing();
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
