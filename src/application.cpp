#include "application.hpp"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "ui.hpp"


Application::Application(const Config& config)
    : _config{ config } {
    Init();
}

Application::~Application() {
    Cleanup();
}

void Application::Init() {
    _paths.imagePath = _config.imagePath;
    _paths.rawImagePath = _config.rawImagePath;
    _paths.trashDir = _config.trashDir;

    // init raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(_config.windowWidth, _config.windowHeight, "Photo Viewer");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    ui::InitUI();

    const ImageViewportInfo viewportInfo{
        .imagePath = _config.imagePath.c_str(),
        .rawImagePath = _config.rawImagePath.c_str(),
        .trashDir = _config.trashDir.c_str(),
        .rawImageExt = _config.rawImageExt.c_str(),
        .windowWidth = _config.windowWidth,
        .windowHeight = _config.windowHeight,
    };
    _viewport = std::make_unique<ImageViewport>(viewportInfo);
}

void Application::Cleanup() {
    _viewport->Cleanup();
    ui::CleanupUI();
    // cleanup raylib
    CloseWindow();
}

void Application::Run() {
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        Draw();

        ui::BeginUI();
        DrawUI();
        ui::EndUI();

        EndDrawing();

        ProcessInput();
        OnResize();
        OnFilesDropped();
    }
}

void Application::Draw() {
    _viewport->Draw();

#ifdef _DEBUG
    DrawFPS(10, 10);
#endif
}

void Application::DrawUI() {
    if (!_showUI) {
        return;
    }

    ui::ImageInfoWindow(_viewport->GetCurrentImageInfo(), _showImageInfo);

    if (ui::PathInputWindow(_paths)) {
        _config.imagePath = _paths.imagePath;
        _config.rawImagePath = _paths.rawImagePath;
        _config.trashDir = _paths.trashDir;

        _viewport->UpdateImagePath(_config.imagePath.c_str());
        _viewport->UpdateImagePath(_config.rawImagePath.c_str());
        _viewport->UpdateImagePath(_config.trashDir.c_str());
    }
}

void Application::ProcessInput() {
    ImGuiIO& io = ImGui::GetIO();
    // block input if TextInput is active
    if (io.WantTextInput)
        return;

    // global keybindings
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
            && IsKeyPressed(KEY_Q)) {
        // CTRL+Q to close the window
        glfwSetWindowShouldClose(
            static_cast<GLFWwindow*>(GetWindowHandle()), GLFW_TRUE);
        return;
    } else if (IsKeyPressed(KEY_F)) {
        // "F" to toggle fullscreen
        ToggleFullscreen();
        const int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        OnResize();
    }

    // block input if UI is in focus
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        return;

    const float scroll = GetMouseWheelMove();

    // "scroll down" or "-" or "S" to zoom out
    if ((scroll < 0.0f
        || IsKeyDown(KEY_MINUS)
        || IsKeyPressed(KEY_S)
        || IsKeyPressedRepeat(KEY_S))) {

        _viewport->ZoomOut();
    }
    // "scroll up" or "+" or "W" to zoom in
    else if ((scroll > 0.0f
        || IsKeyDown(KEY_EQUAL)
        || IsKeyPressed(KEY_W)
        || IsKeyPressedRepeat(KEY_W))) {

        _viewport->ZoomIn();
    }
    // "0" or "Z" to reset zoom
    else if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_Z)) {
        _viewport->ResetZoom();
    }
    // "]" or "E" to rotate clockwise
    else if (IsKeyPressed(KEY_RIGHT_BRACKET) || IsKeyPressed(KEY_E)) {
        _viewport->RotateCW();
    }
    // "[" or "Q" to rotate counter clockwise
    else if (IsKeyPressed(KEY_LEFT_BRACKET) || IsKeyPressed(KEY_Q)) {
        _viewport->RotateCCW();
    }
    // "R" to reset image
    else if (IsKeyPressed(KEY_R)) {
        _viewport->ResetImage();
    }
    // "D" or "Right arrow" to view next image
    else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))) {
        _viewport->NextImage();
    }
    // "A" or "Left arrow" to view previous image
    else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))) {
        _viewport->PrevImage();
    }
    // "Delete" or "X" to delete image as well as raw image (if exists)
    else if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_X)) {
        _viewport->DeleteImage();
    }
    // "I" to print EXIF data
    else if (IsKeyPressed(KEY_I)) {
        _showImageInfo = !_showImageInfo;
    }
    // "H" to show/hide UI
    else if (IsKeyPressed(KEY_H)) {
        _showUI = !_showUI;
    }
    // "Esc" to unfocus all windows
    else if (IsKeyPressed(KEY_ESCAPE)) {
        ui::UnFocusAllWindows();
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        _viewport->MoveCameraUsingMouse();
    }
}

void Application::OnResize() {
    if (!IsWindowResized())
        return;

    _config.windowWidth = GetScreenWidth();
    _config.windowHeight = GetScreenHeight();
    const float winAspectRatio =
        static_cast<float>(_config.windowWidth) / static_cast<float>(_config.windowHeight);
    _viewport->Resize(_config.windowWidth, _config.windowHeight);
}

void Application::OnFilesDropped() {
    if (!IsFileDropped()) {
        return;
    }

    const FilePathList files = LoadDroppedFiles();
    _viewport->LoadFilesFromList(files);
    _config.imagePath = GetDirectoryPath(files.paths[0]);
    if (files.count > 0) {
        _config.imagePath = GetDirectoryPath(files.paths[0]);
    }

    UnloadDroppedFiles(files);
}
