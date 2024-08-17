#include <cstdint>
#include <string>
#include <filesystem>
#include <vector>
#include "raylib.h"
#include "utils.h"

// TODO: hot reloading
// TODO: drag and drop image and view all images in tht directory
// TODO: config file
// TODO: the image from camera are rotated; fix this
// TODO: read file metadata
// TODO: multithreading (load images in batches in the background and clear the images accordingly)

struct ImageDetails {
public:
    ImageDetails(const char* path) {
        // FIXME: the images take a long time to load
        const Image imgData = LoadImage(path);

        texture      = LoadTextureFromImage(imgData);
        aspectRatio  = static_cast<float>(imgData.width) / static_cast<float>(imgData.height);
        srcRectangle = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(imgData.width),
            .height = static_cast<float>(imgData.height),
        };

        UnloadImage(imgData);
    }

public:
    Texture2D texture;
    float aspectRatio;
    Rectangle srcRectangle;
};

void OnResize(Camera2D& camera, Rectangle& imgDstRec, uint64_t& width, uint64_t& height, float& winAspectRatio, const float imgAspectRatio);
void CameraOnUpdate(Camera2D& camera);
void CalcDstImageAspects(Rectangle& imgDstRec, const float imgAspectRatio, const float winAspectRatio, const float width, const float height);
void OnFilesDropped(std::vector<ImageDetails>& imgTextures, int64_t& currImgIdx);

int main() {
    uint64_t width = 640;
    uint64_t height = 480;
    float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Photo Viewer");

    std::vector<ImageDetails> imgTextures{};
    std::string path = "test/";
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(file)) {
            continue;
        } else if (!std::filesystem::is_regular_file(file)) {
            continue;
        }
        // TODO: check if the file is an image or not
        imgTextures.push_back(ImageDetails{ file.path().c_str() });
    }

    int64_t currentImageIdx = 0;
    Rectangle imgDstRec{ 0.0f, 0.0f, 0.0f, 0.0f };
    CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);

    Camera2D camera{
        .offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        OnResize(camera, imgDstRec, width, height, winAspectRatio, imgTextures[currentImageIdx].aspectRatio);
        CameraOnUpdate(camera);

        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));
        BeginMode2D(camera);

        DrawTexturePro(imgTextures[currentImageIdx].texture, imgTextures[currentImageIdx].srcRectangle, imgDstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();

        OnFilesDropped(imgTextures, currentImageIdx);

        if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && currentImageIdx + 1 < imgTextures.size()) {
            ++currentImageIdx;
            CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);
        } else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && currentImageIdx - 1 >= 0) {
            --currentImageIdx;
            CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);
        }
    }

    for (const auto& imgTex : imgTextures)
        UnloadTexture(imgTex.texture);

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
        // TODO: rotate the image not the camera
        // TODO: fit to window after rotating
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

void OnFilesDropped(std::vector<ImageDetails>& imgTextures, int64_t& currImgIdx) {
    if (!IsFileDropped()) {
        return;
    }

    FilePathList files = LoadDroppedFiles();
    if (files.count > 0) {
        for (const auto& tex : imgTextures) {
            UnloadTexture(tex.texture);
        }
        imgTextures.clear();
        imgTextures.reserve(files.count);

        for (uint64_t i = 0; i < files.count; ++i) {
            imgTextures.push_back(ImageDetails{ files.paths[i] });
        }

        currImgIdx = 0;
    }

    UnloadDroppedFiles(files);
}
