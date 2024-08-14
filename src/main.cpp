#include <cstdint>
#include <iostream>
#include "raylib.h"

void OnResize(uint64_t& width, uint64_t& height, float& aspectRatio, Camera2D& camera);
void CameraOnUpdate(Camera2D& camera);

int main() {
    uint64_t width = 640;
    uint64_t height = 480;
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Photo Viewer");

    const Image imgData = LoadImage("test/raylib.jpg");
    // const Image imgData = LoadImage("test/DSC02031.JPG");
    Texture2D imgTex = LoadTextureFromImage(imgData);
    UnloadImage(imgData);
    const float imgAspectRatio = static_cast<float>(imgTex.width) / static_cast<float>(imgTex.height);

    float recX = 0.0f;
    float recY = 0.0f;
    float recWidth = 0.0f;
    float recHeight = 0.0f;

    Camera2D camera{
        .offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        // TODO: only change these on window resize
        if (imgAspectRatio < aspectRatio) {
            recX = -static_cast<float>(height) * 0.5f * imgAspectRatio;
            recY = -static_cast<float>(height) * 0.5f;
            recWidth = static_cast<float>(height) * imgAspectRatio;
            recHeight = static_cast<float>(height);
        } else {
            recX = -static_cast<float>(width) * 0.5f;
            recY = -static_cast<float>(width) * 0.5f * 1.0f / imgAspectRatio;
            recWidth = static_cast<float>(width);
            recHeight = static_cast<float>(width) * 1.0f / imgAspectRatio;
        }

        Rectangle imgSrcRec{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(imgTex.width),
            .height = static_cast<float>(imgTex.height),
        };

        Rectangle imgDstRec{
            .x = recX,
            .y = recY,
            .width = recWidth,
            .height = recHeight,
        };

        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));
        BeginMode2D(camera);

        DrawTexturePro(imgTex, imgSrcRec, imgDstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();

        OnResize(width, height, aspectRatio, camera);
        CameraOnUpdate(camera);
    }

    UnloadTexture(imgTex);
    CloseWindow();
}

void OnResize(uint64_t& width, uint64_t& height, float& aspectRatio, Camera2D& camera) {
    if (!IsWindowResized())
        return;

    width = GetScreenWidth();
    height = GetScreenHeight();
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };

    std::cout << "widht = " << width << ", height = " << height << ", aspect ratio = " << (float)width / (float)height << '\n';
}

void CameraOnUpdate(Camera2D& camera) {
    const float scroll = GetMouseWheelMove();

    if ((scroll < 0.0f || IsKeyDown(KEY_MINUS)) && camera.zoom > 0.5f) {
        camera.zoom -= 0.5f;
    } else if ((scroll > 0.0f || IsKeyDown(KEY_EQUAL)) && camera.zoom <= 100.0f) {
        camera.zoom += 0.5f;
    } else if (IsKeyPressed(KEY_ZERO)) { // "0" to reset zoom
        camera.zoom = 1.0f;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET)) { // "]" to rotate clockwise
        camera.rotation += 90.0f;
        camera.rotation = static_cast<float>(static_cast<int32_t>(camera.rotation) % 360);
    } else if (IsKeyPressed(KEY_LEFT_BRACKET)) { // "[" to rotate counter clockwise
        camera.rotation -= 90.0f;
        camera.rotation = static_cast<float>(static_cast<int32_t>(camera.rotation) % 360);
    }
}
