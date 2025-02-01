#include "ui.hpp"

#include <optional>

#include "imgui_internal.h"
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
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

void UnFocusAllWindows() {
    ImGui::SetWindowFocus(nullptr);
}

ImGuiWindow* CreateImageInfoWindow(const std::optional<ImageDetails>& imgInfo, bool show) {
    if (!show)
        return nullptr;

    ImGui::Begin("Image Info", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGuiWindow* handle = ImGui::GetCurrentWindow();
    ImGui::SetWindowSize(ImVec2{ 365.0f, 195.0f });

    if (!imgInfo.has_value()) {
        ImGui::Text("** No images found! **");
        ImGui::End();
        return nullptr;
    }

    ImGui::Text("File name    : %s", imgInfo.value().filename.c_str());

    if (imgInfo.value().exifInfo.has_value()) {
        const tinyexif::EXIFInfo exifInfo = imgInfo.value().exifInfo.value();

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
    } else if (imgInfo.value().extension != ".JPG"
            && imgInfo.value().extension != ".jpg"
            && imgInfo.value().extension != ".JPEG"
            && imgInfo.value().extension != ".jpeg") {
        ImGui::Text("** Cannot parse EXIF data for non-JPEG images! **");
    } else {
        ImGui::Text("** EXIF data not found! **");
    }

    ImGui::End();
    return handle;
}

ImGuiWindow* CreateConfigWindow(
    TextFields& textFields,
    bool show,
    std::function<void(void)> fnOnApply,
    std::function<void(void)> fnOnLoadFiles
) {
    if (!show)
        return nullptr;

    ImGui::Begin("Configuration", nullptr);
    ImGuiWindow* handle = ImGui::GetCurrentWindow();
    ImGui::SetWindowSize(ImVec2{ 460.0f, 170.0f });
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 160);

    ImGui::Text("Image path");
    ImGui::NextColumn();
    ImGui::InputTextWithHint(
        "##image_path",
        "Enter image path here",
        &textFields.imagePath
    );

    ImGui::NextColumn();
    ImGui::Text("Raw image path");
    ImGui::SameLine();
    ImGui::NextColumn();
    ImGui::InputTextWithHint(
        "##raw_image_path",
        "Enter raw image path here",
        &textFields.rawImagePath
    );

    ImGui::NextColumn();
    ImGui::Text("Trash directory path");
    ImGui::SameLine();
    ImGui::NextColumn();
    ImGui::InputTextWithHint(
        "##trash_directory",
        "Enter trash directory path here",
        &textFields.trashDir
    );

    ImGui::NextColumn();
    ImGui::Text("Raw file extension");
    ImGui::SameLine();
    ImGui::NextColumn();
    ImGui::InputTextWithHint(
        "##raw_file_ext",
        "Enter raw file extension here",
        &textFields.rawImageExt
    );

    ImGui::Columns(1);
    ImGui::NewLine();
    const bool apply = ImGui::Button("Apply");
    ImGui::SameLine();
    const bool load = ImGui::Button("Load files");

    if (ImGui::IsWindowFocused()) {
        // press CTRL+ENTER when window is in focus to trigger `apply`
        if (apply || ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
                && IsKeyPressed(KEY_ENTER))) {
            fnOnApply();
        }
        // press SHIFT+ENTER when window is in focus to trigger `load files`
        else if (load || ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
                && IsKeyPressed(KEY_ENTER))) {
            fnOnLoadFiles();
        }
    }

    ImGui::End();
    return handle;
}

} // namespace ui
