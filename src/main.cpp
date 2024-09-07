#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

#include <GLFW/glfw3.h>
#include "raylib.h"
#include "tinyexif/exif.h"

#include "imageViewport.hpp"
#include "utils.hpp"


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

//
// void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height);
// void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height);
// void OnFilesDropped(ImageViewport& viewport);


int main(int argc, char* argv[]) {
    // Read the JPEG file into a buffer
    std::ifstream stream{ "sandbox/DSC02031.JPG", std::ios::binary };
    std::vector<unsigned char> buffer{ std::istreambuf_iterator<char>{ stream }, std::istreambuf_iterator<char>{} };

    tinyexif::EXIFInfo data;
    int code = data.parseFrom(const_cast<const unsigned char*>(buffer.data()), buffer.size());
    if (code) {
        std::cerr << "Error reading EXIF data\n";
        return -1;
    }

    std::cout << "Exif data:\n";
    std::cout << "    Camera       : " << data.Make << " (" << data.Model << ")\n";
    std::cout << "    Date-time    : " << data.DateTime << '\n';
    std::cout << "    Shutter speed: " << data.ExposureTime << "s\n";
    std::cout << "    Aperture     : f" << data.FNumber << '\n';
    std::cout << "    ISO          : " << data.ISOSpeedRatings << '\n';
    std::cout << "    Focal length : " << data.FocalLength << "mm (" << data.FocalLengthIn35mm << "mm equivalent)\n";
    std::cout << "    Orientation  : " << data.Orientation << '\n';

    // switch (info.orientation) {
    // case 1:
    //     break;
    // case 2: // Flip X
    //     t.scale(-1.0, 1.0);
    //     t.translate(-info.width, 0);
    //     break;
    // case 3: // PI rotation 
    //     t.translate(info.width, info.height);
    //     t.rotate(Math.PI);
    //     break;
    // case 4: // Flip Y
    //     t.scale(1.0, -1.0);
    //     t.translate(0, -info.height);
    //     break;
    // case 5: // - PI/2 and Flip X
    //     t.rotate(-Math.PI / 2);
    //     t.scale(-1.0, 1.0);
    //     break;
    // case 6: // -PI/2 and -width
    //     t.translate(info.height, 0);
    //     t.rotate(Math.PI / 2);
    //     break;
    // case 7: // PI/2 and Flip
    //     t.scale(-1.0, 1.0);
    //     t.translate(-info.height, 0);
    //     t.translate(0, info.width);
    //     t.rotate(  3 * Math.PI / 2);
    //     break;
    // case 8: // PI / 2
    //     t.translate(0, info.width);
    //     t.rotate(  3 * Math.PI / 2);
    //     break;
    // }

    return 0;
//
//     Config config{ "" };
//     utils::ParseArgs(argc, argv, config);
//
//     SetConfigFlags(FLAG_WINDOW_RESIZABLE);
//     InitWindow(config.windowWidth, config.windowHeight, "Photo Viewer");
//     SetExitKey(KEY_NULL);
//     SetTargetFPS(60);
//
//     ImageViewport viewport{ config };
//
//     while(!WindowShouldClose())
//     {
//         BeginDrawing();
//         ClearBackground(GetColor(0x282828FF));
//
//         viewport.Display();
//
// #ifdef _DEBUG
//         DrawFPS(10, 10);
// #endif
//         EndDrawing();
//
//         ProcessInput(viewport, config.windowWidth, config.windowHeight);
//         OnResize(viewport, config.windowWidth, config.windowHeight);
//         OnFilesDropped(viewport);
//     }
//
//     viewport.CleanupImages();
//     CloseWindow();
}


// void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
//     // CTRL+Q to close the window
//     if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_Q)) {
//         glfwSetWindowShouldClose(static_cast<GLFWwindow*>(GetWindowHandle()), GLFW_TRUE);
//         return;
//     }
//     // "F" to toggle fullscreen
//     else if (IsKeyPressed(KEY_F)) {
//         ToggleFullscreen();
//         const int monitor = GetCurrentMonitor();
//         SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
//         OnResize(viewport, width, height);
//     }
//
//     viewport.ProcessKeybindings();
// }
//
// void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
//     if (!IsWindowResized())
//         return;
//
//     width = GetScreenWidth();
//     height = GetScreenHeight();
//     const float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
//     viewport.Resize(width, height);
// }
//
// void OnFilesDropped(ImageViewport& viewport) {
//     if (!IsFileDropped()) {
//         return;
//     }
//
//     FilePathList files = LoadDroppedFiles();
//     viewport.LoadFiles(files);
//     UnloadDroppedFiles(files);
// }
