#include <string>
#include <cstring>
#include <filesystem>
#include <vector>
#include "raylib.h"
#include "logger.h"

// TODO: delete (move to a folder like ".trash" or something) corresponding raw file if available
// TODO: pass arguments and make the app open even if no images are present
// TODO: hot reloading
// TODO: show the original size of the image when the size of the window is bigger than the image
// TODO: multithreading (load images in batches in the background and clear the images accordingly)
// TODO: scroll zoom to mouse position
// TODO: switch directory using TAB/SHIFT-TAB (i.e., load images from sibling directory)
// TODO: make raw file extension, path configurable
// TODO: config file

// FIXME: the image from camera are rotated; read file metadata
// FIXME: the images take a long time to load

struct ImageDetails {
public:
    ImageDetails(const char* path)
        : filepath{ path },
          directory{ GetDirectoryPath(path) },
          fileExt{GetFileExtension(path)},
          filenameNoExt{ "" } {
        const Image imgData = LoadImage(path);

        texture = LoadTextureFromImage(imgData);
        aspectRatio = static_cast<float>(imgData.width) / static_cast<float>(imgData.height);
        srcRectangle = {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(imgData.width),
            .height = static_cast<float>(imgData.height),
        };

        UnloadImage(imgData);

        filename = GetFileName(path);
        for (const auto& ch : filename) {
            if (ch == '.')
                break;

            filenameNoExt += ch;
        }
    }

public:
    std::string filepath;
    std::string directory;
    std::string filename; // filename with extension
    std::string filenameNoExt; // filename without extension
    std::string fileExt; // file extension
    Texture2D texture;
    float aspectRatio;
    Rectangle srcRectangle;
};


void ProcessKeybindings(std::vector<ImageDetails>& imgTextures,
        Camera2D& camera,
        Rectangle& imgDstRec,
        int64_t& currentImageIdx,
        uint64_t& width,
        uint64_t& height,
        float& winAspectRatio,
        const std::string& rawFilePath);
void OnResize(Camera2D& camera, Rectangle& imgDstRec, uint64_t& width, uint64_t& height, float& winAspectRatio, const float imgAspectRatio);
void CalcDstImageAspects(Rectangle& imgDstRec, const float imgAspectRatio, const float winAspectRatio, const float width, const float height);
void OnFilesDropped(std::vector<ImageDetails>& imgTextures, int64_t& currImgIdx);


int main(int argc, char* argv[]) {
    uint64_t width = 640;
    uint64_t height = 480;
    float winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    std::string path = "test/pic/";
    std::string rawFilePath = "test/raw/";

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Photo Viewer");
    SetExitKey(KEY_Q); // "Q" to exit
    SetTargetFPS(60);

    std::vector<ImageDetails> imgTextures{};
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(file)) {
            continue;
        } else if (!std::filesystem::is_regular_file(file)) {
            continue;
        }
        // check if the file is png/jpg or not
        const char* path = file.path().c_str();
        const std::string ext = GetFileExtension(path);
        if (ext != ".png"  &&
            ext != ".PNG"  &&
            ext != ".jpg"  &&
            ext != ".JPG"  &&
            ext != ".jpeg" &&
            ext != ".JPEG"
        ) {
            continue;
        }

        imgTextures.emplace_back(path);
    }

    // TODO: temporary (remove this later when you can open the app on its own without needing an image in the directory)
    if (imgTextures.size() == 0) {
        logger::error("No images found in the current directory!");
        CloseWindow();
        std::exit(-1);
    }

    int64_t currentImageIdx = 0;
    Rectangle imgDstRec{ 0.0f, 0.0f, 0.0f, 0.0f };
    CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);

    Camera2D camera{
        .offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f },
        .target = Vector2{ 0.0f, 0.0f },
        .rotation = 0.0f,
        .zoom = 1.0f,
    };

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(0x282828FF));
        BeginMode2D(camera);

        // TODO: do something when there are no images
        DrawTexturePro(imgTextures[currentImageIdx].texture, imgTextures[currentImageIdx].srcRectangle, imgDstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        EndMode2D();

        DrawFPS(10, 10);
        EndDrawing();

        ProcessKeybindings(imgTextures, camera, imgDstRec, currentImageIdx, width, height, winAspectRatio, rawFilePath);
        OnResize(camera, imgDstRec, width, height, winAspectRatio, imgTextures[currentImageIdx].aspectRatio);
        OnFilesDropped(imgTextures, currentImageIdx);
    }

    for (const auto& imgTex : imgTextures)
        UnloadTexture(imgTex.texture);

    CloseWindow();
}

void OnResize(Camera2D& camera, Rectangle& imgDstRec, uint64_t& width, uint64_t& height, float& winAspectRatio, const float imgAspectRatio) {
    if (!IsWindowResized())
        return;

    width = GetScreenWidth();
    height = GetScreenHeight();
    winAspectRatio = static_cast<float>(width) / static_cast<float>(height);
    camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
    CalcDstImageAspects(imgDstRec, imgAspectRatio, winAspectRatio, width, height);
}

void ProcessKeybindings(std::vector<ImageDetails>& imgTextures,
        Camera2D& camera,
        Rectangle& imgDstRec,
        int64_t& currentImageIdx,
        uint64_t& width,
        uint64_t& height,
        float& winAspectRatio,
        const std::string& rawFilePath) {
    const float scroll = GetMouseWheelMove();

    if ((scroll < 0.0f || IsKeyDown(KEY_MINUS)) && camera.zoom > 0.5f) { // "scroll down" or "-" to zoom out
        camera.zoom -= 0.5f;
    } else if ((scroll > 0.0f || IsKeyDown(KEY_EQUAL)) && camera.zoom <= 100.0f) { // "scroll up" or "+" to zoom in
        camera.zoom += 0.5f;
    } else if (IsKeyPressed(KEY_ZERO)) { // "0" to reset zoom
        camera.zoom = 1.0f;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET)) { // "]" to rotate clockwise
        // TODO: rotate the image not the camera
        // TODO: fit to window after rotating
        camera.rotation += 90.0f;
        camera.rotation = static_cast<float>(static_cast<int32_t>(camera.rotation) % 360);
    } else if (IsKeyPressed(KEY_LEFT_BRACKET)) { // "[" to rotate counter clockwise
        camera.rotation -= 90.0f;
        camera.rotation = static_cast<float>(static_cast<int32_t>(camera.rotation) % 360);
    } else if (IsKeyPressed(KEY_R)) { // "R" to reset camera
        camera.offset = Vector2{ static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f };
        camera.target = Vector2{ 0.0f, 0.0f };
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    } else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && currentImageIdx + 1 < imgTextures.size()) { // "D" or "Right arrow" to view next image
        ++currentImageIdx;
        CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);
    } else if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && currentImageIdx - 1 >= 0) { // "A" or "Left arrow" to view previous image
        --currentImageIdx;
        CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);
    } else if (IsKeyPressed(KEY_F)) { // "F" to toggle fullscreen
        ToggleFullscreen();
        const int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        OnResize(camera, imgDstRec, width, height, winAspectRatio, imgTextures[currentImageIdx].aspectRatio);
    } else if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_X)) { // "Delete" or "X" to delete image as well as raw image (if exists)
        const char* trash = "test/.trash/";
        // TODO: do not hardcode
        if (!std::filesystem::exists(trash)) {
            std::filesystem::create_directory(trash);
        }

        std::filesystem::path imagePath{ imgTextures[currentImageIdx].filepath };
        std::filesystem::path rawImagePath{ rawFilePath + imgTextures[currentImageIdx].filenameNoExt + ".ARW" };

        // move the files to `.trash`
        std::filesystem::rename(imagePath, trash + imgTextures[currentImageIdx].filename);
        logger::log("deleting: \"%s\"", imagePath.c_str());
        if (std::filesystem::exists(rawImagePath)) {
            logger::log("deleting: \"%s\"", rawImagePath.c_str());
            std::filesystem::rename(rawImagePath, trash + imgTextures[currentImageIdx].filenameNoExt + ".ARW");
        }

        // FIXME: after deleting, the next image's aspect ratio is not loading correctly
        imgTextures.erase(imgTextures.begin() + currentImageIdx);
        if (currentImageIdx - 1 >= 0) {
            --currentImageIdx;
        } else {
            currentImageIdx = 0;
        }
        CalcDstImageAspects(imgDstRec, imgTextures[currentImageIdx].aspectRatio, winAspectRatio, width, height);
    }
}

void CalcDstImageAspects(Rectangle& imgDstRec, const float imgAspectRatio, const float winAspectRatio, const float width, const float height) {
    if (imgAspectRatio < winAspectRatio) {
        imgDstRec.x = -static_cast<float>(height) * 0.5f * imgAspectRatio;
        imgDstRec.y = -static_cast<float>(height) * 0.5f;
        imgDstRec.width = static_cast<float>(height) * imgAspectRatio;
        imgDstRec.height = static_cast<float>(height);
    } else {
        imgDstRec.x = -static_cast<float>(width) * 0.5f;
        imgDstRec.y = -static_cast<float>(width) * 0.5f * 1.0f / imgAspectRatio;
        imgDstRec.width = static_cast<float>(width);
        imgDstRec.height = static_cast<float>(width) * 1.0f / imgAspectRatio;
    }
}

void OnFilesDropped(std::vector<ImageDetails>& imgTextures, int64_t& currImgIdx) {
    if (!IsFileDropped()) {
        return;
    }

    FilePathList files = LoadDroppedFiles();
    if (files.count > 0) {
        for (const auto& tex : imgTextures) {
            UnloadTexture(tex.texture);
        }
        imgTextures.clear();
        imgTextures.reserve(files.count);

        for (uint64_t i = 0; i < files.count; ++i) {
            imgTextures.push_back(ImageDetails{ files.paths[i] });
        }

        currImgIdx = 0;
    }

    UnloadDroppedFiles(files);
}
