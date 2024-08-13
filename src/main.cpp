#include <cstdint>
#include "raylib.h"

void OnResize(uint64_t& width, uint64_t& height);
void CameraOnUpdate(Camera2D& camera);

int main() {
    uint64_t width = 640;
    uint64_t height = 640;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Photo Viewer");

    const Image imgData = LoadImage("test/raylib.jpg");
    Texture2D imgTex = LoadTextureFromImage(imgData);
    UnloadImage(imgData);
    const float imgAspectRatioInv = static_cast<float>(imgTex.height) / static_cast<float>(imgTex.width);

    float recX = 0.0f;
    float recY = 0.0f;
    float recWidth = 0.0f;
    float recHeight = 0.0f;

    if (imgTex.width < imgTex.height) {
        recX = -static_cast<float>(height) * 0.5f * imgAspectRatioInv;
        recY = -static_cast<float>(height) * 0.5f;
        recWidth = static_cast<float>(height) * imgAspectRatioInv;
        recHeight = static_cast<float>(height);
    } else {
        recX = -static_cast<float>(width) * 0.5f;
        recY = -static_cast<float>(width) * 0.5f * imgAspectRatioInv;
        recWidth = static_cast<float>(width);
        recHeight = static_cast<float>(width) * imgAspectRatioInv;
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

    Camera2D camera{
        .offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));
        BeginMode2D(camera);

        DrawTexturePro(imgTex, imgSrcRec, imgDstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();

        OnResize(width, height);
        CameraOnUpdate(camera);
    }

    UnloadTexture(imgTex);
    CloseWindow();
}

void OnResize(uint64_t& width, uint64_t& height) {
    if (IsWindowResized()) {
        width = GetScreenWidth();
        height = GetScreenHeight();
    }
}

void CameraOnUpdate(Camera2D& camera) {
    const float scroll = GetMouseWheelMove();

    if ((scroll < 0.0f || IsKeyDown(KEY_MINUS)) && camera.zoom > 0.5f) {
        camera.zoom -= 0.5f;
    } else if ((scroll > 0.0f || IsKeyDown(KEY_EQUAL)) && camera.zoom <= 100.0f) {
        camera.zoom += 0.5f;
    }

    if (IsKeyPressed(KEY_ZERO)) { // "0" to reset zoom
        camera.zoom = 1.0f;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET)) { // "]" to rotate clockwise
        camera.rotation += 90.0f;
        camera.rotation = static_cast<float>(static_cast<int32_t>(camera.rotation) % 360);
    } else if (IsKeyPressed(KEY_LEFT_BRACKET)) { // "[" to rotate counter clockwise
        camera.rotation -= 90.0f;
        camera.rotation = static_cast<float>(static_cast<int32_t>(camera.rotation) % 360);
    }
}
