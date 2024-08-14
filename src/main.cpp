#include <cstdint>
#include <iostream>
#include "raylib.h"

void OnResize(Camera2D& camera, Rectangle& imgDstRec, uint64_t& width, uint64_t& height, float& winAspectRatio, const float imgAspectRatio);
void CameraOnUpdate(Camera2D& camera);
void CalcDstImageAspects(Rectangle& imgDstRec, const float imgAspectRatio, const float winAspectRatio, const float width, const float height);

int main() {
    uint64_t width = 640;
    uint64_t height = 480;
    float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Photo Viewer");

    // const Image imgData = LoadImage("test/raylib.jpg");
    const Image imgData = LoadImage("test/DSC02031.JPG");
    Texture2D imgTex = LoadTextureFromImage(imgData);
    UnloadImage(imgData);
    const float imgAspectRatio = static_cast<float>(imgTex.width) / static_cast<float>(imgTex.height);
    Rectangle imgSrcRec{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(imgTex.width),
        .height = static_cast<float>(imgTex.height),
    };

    Rectangle imgDstRec{
        .x = 0.0f,
        .y = 0.0f,
        .width = 0.0f,
        .height = 0.0f,
    };

    CalcDstImageAspects(imgDstRec, imgAspectRatio, winAspectRatio, width, height);

    Camera2D camera{
        .offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        OnResize(camera, imgDstRec, width, height, winAspectRatio, imgAspectRatio);
        CameraOnUpdate(camera);

        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));
        BeginMode2D(camera);

        DrawTexturePro(imgTex, imgSrcRec, imgDstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(imgTex);
    CloseWindow();
}

void OnResize(Camera2D& camera, Rectangle& imgDstRec, uint64_t& width, uint64_t& height, float& winAspectRatio, const float imgAspectRatio) {
    if (!IsWindowResized())
        return;

    width = GetScreenWidth();
    height = GetScreenHeight();
    winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
    CalcDstImageAspects(imgDstRec, imgAspectRatio, winAspectRatio, width, height);
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

void CalcDstImageAspects(Rectangle& imgDstRec, const float imgAspectRatio, const float winAspectRatio, const float width, const float height) {
    if (imgAspectRatio < winAspectRatio) {
        imgDstRec.x = -static_cast<float>(height) * 0.5f * imgAspectRatio;
        imgDstRec.y = -static_cast<float>(height) * 0.5f;
        imgDstRec.width = static_cast<float>(height) * imgAspectRatio;
        imgDstRec.height = static_cast<float>(height);
    } else {
        imgDstRec.x = -static_cast<float>(width) * 0.5f;
        imgDstRec.y = -static_cast<float>(width) * 0.5f * 1.0f / imgAspectRatio;
        imgDstRec.width = static_cast<float>(width);
        imgDstRec.height = static_cast<float>(width) * 1.0f / imgAspectRatio;
    }
}
