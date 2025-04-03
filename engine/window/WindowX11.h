#ifndef WINDOW_X11_H
#define WINDOW_X11_H

#include <cstdint>
#include <string>
#include <X11/Xlib.h>

class WindowX11 {
public:
    WindowX11(uint32_t width, uint32_t height, const std::string &title);
    ~WindowX11();

    // Processa gli eventi X; ritorna false se la finestra deve chiudersi
    bool pollEvents();

    Display* getDisplay() const { return display; }
    Window getWindow() const { return window; }
    bool shouldClose() const { return closeRequested; }

    // Getters per lo stato del mouse
    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }
    bool isLeftButtonDown() const { return leftButtonDown; }
    bool isRightButtonDown() const { return rightButtonDown; }

private:
    Display* display;
    Window window;
    uint32_t width, height;
    bool closeRequested;

    // Stato del mouse
    int mouseX;
    int mouseY;
    bool leftButtonDown;
    bool rightButtonDown;
};

#endif // WINDOW_X11_H
