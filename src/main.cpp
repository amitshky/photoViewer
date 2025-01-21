#include <GLFW/glfw3.h>
#include "raylib.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "imageViewport.hpp"
#include "utils.hpp"

// TODO: dont load files to memory on startup
// TODO: refactor this
//       - application class, input processing
// TODO: a way to input image directories
// TODO: display metadata in the viewport
// TODO: check orientation from EXIF data and rotate accordingly
// TODO: fit image to window after rotating

// TODO: multithreading
//       - load images in batches in the background and clear
//         the images accordingly
//       - store loaded images in a stack-like data structure tht will only store
//         a number of images, and when full will overwrite
//         the older images (use indices)
// TODO: hot reloading
// TODO: CTRL+C to copy image and CTRL+SHIFT+C to copy path
// TODO: drag the image to copy the image
// TODO: switch directory using TAB/SHIFT-TAB
//       (i.e., load images from sibling directory)
// TODO: make raw file extension, path, etc configurable (from a config file)


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height);
void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height);
void OnFilesDropped(ImageViewport& viewport, Config& config);
void InitUI();
void CleanupUI();
void BeginUI();
void EndUI();


int main(int argc, char* argv[]) {
    Config config{};
    utils::ParseArgs(argc, argv, config);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(config.windowWidth, config.windowHeight, "Photo Viewer");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    InitUI();
    ImageViewport viewport{ config };

    bool firstFrame = true;
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));

        viewport.Display();

#ifdef _DEBUG
        DrawFPS(10, 10);
#endif

        BeginUI();
        ImGui::ShowDemoWindow(nullptr);
        if (firstFrame) { // remove focus from imgui windows
            ImGui::SetWindowFocus(nullptr);
            firstFrame = false;
        }
        EndUI();

        EndDrawing();

        ProcessInput(viewport, config.windowWidth, config.windowHeight);
        OnResize(viewport, config.windowWidth, config.windowHeight);
        OnFilesDropped(viewport, config);
    }

    // Cleanup
    viewport.CleanupImages();
    CleanupUI();
    CloseWindow();
}


void ProcessInput(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
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
        OnResize(viewport, width, height);
    }

    // block input if UI is in focus
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        return;

    viewport.ProcessKeybindings();
}

void OnResize(ImageViewport& viewport, uint64_t& width, uint64_t& height) {
    if (!IsWindowResized())
        return;

    width = GetScreenWidth();
    height = GetScreenHeight();
    const float winAspectRatio =
        static_cast<float>(width) / static_cast<float>(height);
    viewport.Resize(width, height);
}

void OnFilesDropped(ImageViewport& viewport, Config& config) {
    if (!IsFileDropped()) {
        return;
    }

    const FilePathList files = LoadDroppedFiles();
    viewport.LoadFilesFromList(files);
    if (files.count > 0) {
        config.imagePath = GetDirectoryPath(files.paths[0]);
    }

    UnloadDroppedFiles(files);
}

void InitUI() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(GetWindowHandle()), true);
    ImGui_ImplOpenGL3_Init();
}

void CleanupUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void BeginUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EndUI() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
