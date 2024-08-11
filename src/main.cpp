#include "raylib.h"

int main() {
    InitWindow(640, 640, "Photo Viewer");

    Image img = LoadImage("test/raylib_logo.jpg");
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureV(texture, Vector2{ 0.0f, 0.0f }, WHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
