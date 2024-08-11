#include "raylib.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(640, 640, "Photo Viewer");

    Texture2D img = LoadTextureFromImage(LoadImage("test/raylib_logo.jpg"));

    Camera2D camera{
        .offset = Vector2{ static_cast<float>(img.width) * 0.5f, static_cast<float>(img.height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTextureV(img, Vector2{ 0.0f, 0.0f }, WHITE);
        DrawFPS(10, 10);

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
}
