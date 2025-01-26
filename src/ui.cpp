#include "ui.hpp"

#include "raylib.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


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
    // dont focus on any window at startup
    static bool firstFrame = true;
    if (firstFrame) { // remove focus from imgui windows
        ImGui::SetWindowFocus(nullptr);
        firstFrame = false;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

