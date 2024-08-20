#include <cstring>
#include <GLFW/glfw3.h>
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


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height);
void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height);
void OnFilesDropped(ImageViewport& viewport);


int main(int argc, char* argv[]) {
    Config config{
        .imagePath = "test/pic/",
        .rawImagePath = "test/raw/",
        .trashPath = "test/trash/",
        .windowWidth = 1280,
        .windowHeight = 960,
    };

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(config.windowWidth, config.windowHeight, "Photo Viewer");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    ImageViewport viewport{ config };

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        viewport.Display();

        // TODO: do this only in debug mode
        DrawFPS(10, 10);
        EndDrawing();

        ProcessInput(viewport, config.windowWidth, config.windowHeight);
        OnResize(viewport, config.windowWidth, config.windowHeight);
        OnFilesDropped(viewport);
    }

    viewport.CleanupImages();
    CloseWindow();
}


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
    viewport.ProcessKeybindings();

    // CTRL+Q to close the window
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_Q)) {
        glfwSetWindowShouldClose(static_cast<GLFWwindow*>(GetWindowHandle()), GLFW_TRUE);
    }
    // "F" to toggle fullscreen
    else if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
        const int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        OnResize(viewport, width, height);
    }
}

void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
    if (!IsWindowResized())
        return;

    width = GetScreenWidth();
    height = GetScreenHeight();
    const float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    viewport.Resize(width, height);
}

void OnFilesDropped(ImageViewport& viewport) {
    if (!IsFileDropped()) {
        return;
    }

    FilePathList files = LoadDroppedFiles();
    viewport.LoadFiles(files);
    UnloadDroppedFiles(files);
}
