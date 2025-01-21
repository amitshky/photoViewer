file(GLOB_RECURSE SRC_FILES "*.cpp")

add_executable(
    ${PROJECT_NAME}
    "${SRC_FILES}"

    # tinyexif
    "../ext/tinyexif/exif.cpp"

    # imgui
    "../ext/imgui/imgui.cpp"
    "../ext/imgui/imgui_demo.cpp"
    "../ext/imgui/imgui_draw.cpp"
    "../ext/imgui/imgui_tables.cpp"
    "../ext/imgui/imgui_widgets.cpp"
    # to easily use InputText with std::string
    "../ext/imgui/misc/cpp/imgui_stdlib.cpp"
    # imgui backends
    "../ext/imgui/backends/imgui_impl_opengl3.cpp"
    "../ext/imgui/backends/imgui_impl_glfw.cpp"
)
