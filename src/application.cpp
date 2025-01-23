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
    // init raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(_config.windowWidth, _config.windowHeight, "Photo Viewer");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    InitUI();
    _viewport = std::make_unique<ImageViewport>(_config);
}

void Application::Cleanup() {
    _viewport->Cleanup();
    CleanupUI();
    // cleanup raylib
    CloseWindow();
}

void Application::Run() {
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        Draw();

        EndDrawing();

        ProcessInput();
        OnResize();
        OnFilesDropped();
    }
}

void Application::Draw() {
    _viewport->Display();

#ifdef _DEBUG
    DrawFPS(10, 10);
#endif

    BeginUI();
    OnUpdateUI();
    EndUI();
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

    _viewport->ProcessKeybindings();
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
    if (files.count > 0) {
        _config.imagePath = GetDirectoryPath(files.paths[0]);
    }

    UnloadDroppedFiles(files);
}
