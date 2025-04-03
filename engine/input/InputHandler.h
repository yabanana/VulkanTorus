#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class InputHandler {
public:
    InputHandler();

    // Aggiorna lo stato degli input in base alle coordinate del mouse e allo stato dei pulsanti
    void update(int x, int y, bool leftButton, bool rightButton);

    float getRotationX() const { return rotationX; }
    float getRotationY() const { return rotationY; }
    float getTranslationX() const { return translationX; }
    float getTranslationY() const { return translationY; }
    float getZoom() const { return zoom; }

private:
    float rotationX, rotationY;
    float translationX, translationY;
    float zoom;
};

#endif // INPUT_HANDLER_H
