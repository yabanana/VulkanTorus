#include "InputHandler.h"
#include <iostream>

InputHandler::InputHandler()
    : rotationX(0.0f), rotationY(0.0f), translationX(0.0f), translationY(0.0f), zoom(1.0f)
{
}

void InputHandler::update(int x, int y, bool leftButton, bool rightButton)
{
    // Usa variabili statiche per memorizzare la posizione precedente
    static bool firstCall = true;
    static int lastX = 0, lastY = 0;
    if (firstCall) {
        lastX = x;
        lastY = y;
        firstCall = false;
    }
    
    int dx = x - lastX;
    int dy = y - lastY;

    if (leftButton) {
        rotationY += dx * 0.005f;
        rotationX += dy * 0.005f;
    }
    if (rightButton) {
        translationX += dx * 0.01f;
        translationY -= dy * 0.01f;
    }
    
    lastX = x;
    lastY = y;
    
    std::cout << "Input updated: "
              << "Rotation(" << rotationX << ", " << rotationY << ") "
              << "Translation(" << translationX << ", " << translationY << ") "
              << "Zoom(" << zoom << ")" << std::endl;
}
