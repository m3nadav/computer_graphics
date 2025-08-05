#ifndef MENU_H
#define MENU_H

#include <tuple>

class UIButton
{
public:
    float x;
    float y;
    float width;
    float height;
    const char *text;
    bool isActive;
    UIButton *parent;

    // Default constructor
    UIButton() : x(0), y(0), width(0), height(0), text(""), isActive(false), parent(nullptr) {}

    UIButton(float x_, float y_, float width_, float height_, const char *text_, UIButton *parent_)
        : x(x_), y(y_), width(width_), height(height_), text(text_), isActive(false), parent(parent_) {}

    UIButton(float x_, float y_, float width_, float height_, const char *text_)
        : x(x_), y(y_), width(width_), height(height_), text(text_), isActive(false), parent(nullptr) {}

    bool isPointInButton(int mouseX, int mouseY) const;
    void draw();
    void handleClick(int mouseX, int mouseY);
    bool isVisible() const;
};

// Menu system for OpenGL applications
class MenuSystem
{
public:
    MenuSystem();
    UIButton menuButton;
    UIButton lightButton;
    UIButton helpButton;
    UIButton quitButton;
    UIButton intensityButton;
    UIButton positionXButton;
    UIButton positionYButton;
    UIButton positionZButton;
    UIButton ambientButton;

    // Drawing functions
    void drawUI();

    // Input handling - returns true if click was handled by menu system
    bool handleMenuClick(int mouseX, int mouseY);

private:
    // UI drawing methods
    void drawMenuButton();
    void drawMenuBox();
    void drawLightControls();
    void drawValueButton(UIButton button, float value);

    void handleLightValueClick(int mouseX, int mouseY, UIButton button);

    // Helper methods
    void setupUI2D();
    void restoreUI3D();
};

void drawButton(UIButton button);
void renderText2D(const char *text, float x, float y);

#endif // MENU_H