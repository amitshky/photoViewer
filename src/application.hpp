#pragma once

#include <memory>
#include "types.hpp"
#include "imageViewport.hpp"


class Application {
public:
    explicit Application(const Config& config);
    ~Application();
    
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void Run();

private:
    void Init();
    void Cleanup();

    void Draw();
    void DrawUI();
    void ProcessInput();
    void OnResize();
    void OnFilesDropped();

private:
    Config _config;
    std::unique_ptr<ImageViewport> _viewport;
    bool _showExifInfo = false;
};
