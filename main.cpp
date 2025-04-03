#include "WindowX11.h"
#include "TorusApp.h"
#include <iostream>

int main()
{
    WindowX11 window(1600, 1200, "Generic Vulkan Engine");
    TorusApp app(&window);

    try {
        app.init();
        while (!window.shouldClose()) {
            app.update();
            app.render();
        }
        app.cleanup();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
