#include "ui.hpp"

#include "raylib.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"


namespace ui {

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

void UnFocusAllWindows() {
    ImGui::SetWindowFocus(nullptr);
}

void ImageInfoWindow(const ImageDetails& imgInfo, bool show) {
    if (!show)
        return;

    ImGui::Begin("Image Info", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::SetWindowSize(ImVec2{ 365, 195 });
    ImGui::Text("Name         : %s", imgInfo.filename.c_str());

    if (imgInfo.exifInfo.has_value()) {
        const tinyexif::EXIFInfo exifInfo = imgInfo.exifInfo.value();

        ImGui::Text("Camera       : %s %s", exifInfo.Make.c_str(), exifInfo.Model.c_str());
        ImGui::Text("Date-time    : %s", exifInfo.DateTime.c_str());

        if (exifInfo.ExposureTime < 1.0) {
            ImGui::Text("Shutter speed: 1/%ds",
                static_cast<int>(1.0f / exifInfo.ExposureTime));
        } else {
            ImGui::Text("Shutter speed: %.2fs", exifInfo.ExposureTime);
        }

        ImGui::Text("Aperture     : f/%.1f", exifInfo.FNumber);
        ImGui::Text("ISO          : %hu", exifInfo.ISOSpeedRatings);
        ImGui::Text("Focal length : %dmm", static_cast<int>(exifInfo.FocalLength));
        ImGui::Text("Orientation  : %hu", exifInfo.Orientation);
    } else if (imgInfo.extension != ".JPG"
            && imgInfo.extension != ".jpg"
            && imgInfo.extension != ".JPEG"
            && imgInfo.extension != ".jpeg") {
        ImGui::Text("** Cannot parse EXIF data for non-JPEG images! **");
    } else {
        ImGui::Text("** EXIF data not found! **");
    }

    ImGui::End();
}

bool PathInputWindow(ImagePaths& paths) {
    ImGui::Begin("paths", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::SetWindowSize(ImVec2{ 460.0f, 125.0f });

    ImGui::InputTextWithHint("Image path", "enter image path here", 
        &paths.imagePath);
    ImGui::InputTextWithHint("Raw image path", "enter raw image path here",
        &paths.rawImagePath);
    ImGui::InputTextWithHint("Trash directory", "enter trash directory path here",
        &paths.trashDir);

    bool apply = ImGui::Button("Apply");
    
    ImGui::End();

    return apply;
}

} // namespace ui
