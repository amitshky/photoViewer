#include <GLFW/glfw3.h>
#include "utils.hpp"
#include "application.hpp"


// TODO: add screenshots to readme
// TODO: create something like a UI manager
//       - register windows at application init
//       - store window handles in a hash map or some sort of a data structure
//       - show/hide windows by traversing the data structure
//           - so that you can only hide the windows that are visible
// TODO: check orientation from EXIF data and rotate accordingly
// TODO: fit image to window after rotating
// TODO: zoom in from where the mouse cursor is

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


int main(int argc, char* argv[]) {
    Config config{};
    utils::ParseArgs(argc, argv, config);

    Application* app = new Application(config);
    app->Run();
    delete app;
}
