#include <GLFW/glfw3.h>
#include "raylib.h"

#include "imageViewport.hpp"
#include "utils.hpp"


// TODO: fix args parser
// TODO: refactor this
//       - application class, input processing
// TODO: multithreading
//       - load images in batches in the background and clear the images accordingly
//       - store loaded images on a stack-like data structure tht will only store a
//         number of images, and when full will overwrite the older images (use indices)
// TODO: hot reloading
// TODO: CTRL+C to copy image and CTRL+SHIFT+C to copy path
// TODO: drag the image to copy image to copy the image
// TODO: switch directory using TAB/SHIFT-TAB (i.e., load images from sibling directory)
// TODO: make raw file extension, path, etc configurable (config file)
// TODO: display metadata in the viewport
// TODO: fit image to window after rotating

// FIXME: the image from camera are rotated; read file metadata (exif data)
// FIXME: the images take a long time to load
//        - (no improvement) maybe load textures from memory only on image change


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height);
void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height);
void OnFilesDropped(ImageViewport& viewport);


int main(int argc, char* argv[]) {
    Config config{};
    utils::ParseArgs(argc, argv, config);

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

#ifdef _DEBUG
        DrawFPS(10, 10);
#endif
        EndDrawing();

        ProcessInput(viewport, config.windowWidth, config.windowHeight);
        OnResize(viewport, config.windowWidth, config.windowHeight);
        OnFilesDropped(viewport);
    }

    viewport.CleanupImages();
    CloseWindow();
}


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
    // CTRL+Q to close the window
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_Q)) {
        glfwSetWindowShouldClose(static_cast<GLFWwindow*>(GetWindowHandle()), GLFW_TRUE);
        return;
    }
    // "F" to toggle fullscreen
    else if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
        const int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        OnResize(viewport, width, height);
    }

    viewport.ProcessKeybindings();
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
