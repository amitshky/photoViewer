#include <string>
#include <cstring>
#include "raylib.h"
#include "imageViewport.hpp"

// TODO: pass arguments and make the app open even if no images are present
// TODO: drag and drop folders
// TODO: hot reloading
// TODO: show the original size of the image when the size of the window is bigger than the image
// TODO: multithreading (load images in batches in the background and clear the images accordingly)
// TODO: scroll zoom to mouse position
// TODO: switch directory using TAB/SHIFT-TAB (i.e., load images from sibling directory)
// TODO: make raw file extension, path configurable
// TODO: config file

// FIXME: the image from camera are rotated; read file metadata
// FIXME: the images take a long time to load (maybe load textures from memory only on image change)


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height, float& winAspectRatio, const std::string& rawFilePath);
void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height, float& winAspectRatio);
void OnFilesDropped(ImageViewport& viewport);


int main(int argc, char* argv[]) {
    uint64_t width = 640;
    uint64_t height = 480;
    float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    std::string path = "test/pic/";
    std::string rawFilePath = "test/raw/";

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Photo Viewer");
    SetExitKey(KEY_Q); // "Q" to exit
    SetTargetFPS(60);

    ImageViewport viewport{ path.c_str(), width, height };

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        viewport.Display();

        DrawFPS(10, 10);
        EndDrawing();

        viewport.ProcessKeybindings(width, height, winAspectRatio, rawFilePath);
        OnResize(viewport, width, height, winAspectRatio);
        OnFilesDropped(viewport);
    }

    viewport.CleanupImages();
    CloseWindow();
}

void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height, float& winAspectRatio, const std::string& rawFilePath) {
    viewport.ProcessKeybindings(width, height, winAspectRatio, rawFilePath);

    if (IsKeyPressed(KEY_F)) { // "F" to toggle fullscreen
        ToggleFullscreen();
        const int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        OnResize(viewport, width, height, winAspectRatio);
    }
}

void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height, float& winAspectRatio) {
    if (!IsWindowResized())
        return;

    width = GetScreenWidth();
    height = GetScreenHeight();
    winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    viewport.Resize(width, height, winAspectRatio);
}

void OnFilesDropped(ImageViewport& viewport) {
    if (!IsFileDropped()) {
        return;
    }

    FilePathList files = LoadDroppedFiles();
    viewport.LoadFiles(files);
    UnloadDroppedFiles(files);
}
