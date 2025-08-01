#ifndef INPUT_H
#define INPUT_H

#include "common/camera.h"
#include <functional>

// Input handler system with configurable callbacks
class InputHandler
{
public:
    // Callback types
    using KeyboardCallback = std::function<void(unsigned char, int, int)>;
    using SpecialKeyCallback = std::function<void(int, int, int)>;
    using DisplayCallback = std::function<void()>;

    InputHandler(CameraController &camera);

    // Set up GLUT callbacks
    void setupGLUTCallbacks();

    // Callback configuration
    void setKeyboardCallback(KeyboardCallback callback) { keyboardCallback = callback; }
    void setSpecialKeyCallback(SpecialKeyCallback callback) { specialKeyCallback = callback; }
    void setDisplayCallback(DisplayCallback callback) { displayCallback = callback; }

    // Cow controls support
    void enableCowControls(bool enable = true) { cowControlsEnabled = enable; }

    // Timer control
    void startTimer(int intervalMs = 50);
    void stopTimer() { timerActive = false; }

    // Command line argument handling
    void handleCommandLineArgs(int argc, char **argv, int maxScenes = 0);
    int getStartingScene() const { return startingScene; }

private:
    CameraController &cameraController;
    KeyboardCallback keyboardCallback;
    SpecialKeyCallback specialKeyCallback;
    DisplayCallback displayCallback;

    // Cow controls
    bool cowControlsEnabled;

    // Mouse state
    bool rightMouseDown;
    int lastMouseX;
    int lastMouseY;

    // Timer state
    bool timerActive;
    int timerInterval;

    // Command line
    int startingScene;

    // Static callback wrappers for GLUT
    static void keyboardWrapper(unsigned char key, int x, int y);
    static void mouseWrapper(int button, int state, int x, int y);
    static void motionWrapper(int x, int y);
    static void timerWrapper(int value);
    static void reshapeWrapper(int w, int h);

    // Static instance pointer for callbacks
    static InputHandler *instance;

    // Keyboard handling methods
    void handleKeyboard(unsigned char key, int x, int y);
    void handleCowControls(unsigned char key, int x, int y);

    // Mouse handling methods
    void handleMouse(int button, int state, int x, int y);
    void handleMotion(int x, int y);

    // Other handlers
    void handleTimer(int value);
    void handleReshape(int w, int h);
};

#endif // INPUT_H