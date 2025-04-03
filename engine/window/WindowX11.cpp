#include "WindowX11.h"
#include <iostream>
#include <cstdlib>
#include <X11/keysym.h>

WindowX11::WindowX11(uint32_t width, uint32_t height, const std::string &title)
    : width(width), height(height), closeRequested(false),
      mouseX(0), mouseY(0), leftButtonDown(false), rightButtonDown(false)
{
    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 10, 10, width, height, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask |
                             ButtonReleaseMask | PointerMotionMask);
    XStoreName(display, window, title.c_str());
    XMapWindow(display, window);
}

WindowX11::~WindowX11()
{
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

bool WindowX11::pollEvents()
{
    // Processa gli eventi X e aggiorna lo stato del mouse
    while (XPending(display)) {
        XEvent event;
        XNextEvent(display, &event);
        switch (event.type) {
            case KeyPress: {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_Escape)
                    closeRequested = true;
                break;
            }
            case ButtonPress: {
                if (event.xbutton.button == Button1)
                    leftButtonDown = true;
                else if (event.xbutton.button == Button3)
                    rightButtonDown = true;
                // Aggiorna la posizione del mouse
                mouseX = event.xbutton.x;
                mouseY = event.xbutton.y;
                break;
            }
            case ButtonRelease: {
                if (event.xbutton.button == Button1)
                    leftButtonDown = false;
                else if (event.xbutton.button == Button3)
                    rightButtonDown = false;
                break;
            }
            case MotionNotify: {
                mouseX = event.xmotion.x;
                mouseY = event.xmotion.y;
                break;
            }
            case ClientMessage: {
                closeRequested = true;
                break;
            }
            default:
                break;
        }
    }
    return !closeRequested;
}
