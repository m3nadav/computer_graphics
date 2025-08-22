#ifndef MENU_H
#define MENU_H

#include <tuple>

// Generic UI button component
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

    bool isButtonClicked(int mouseX, int mouseY);
    void draw();
    void toggleActive();
    bool isVisible() const;
    void updateButton(float x, float y, float width, float height, const char *text, UIButton *parent = nullptr);
    void updateWindowDimensions(float width, float height);
};

// Menu system for the main scene
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

    // Handles menu clicks
    void handleMenuClick(int mouseX, int mouseY);

    // Applies menu clicks
    void applyMenuClick();
    void applyQuitClick();
    void applyHelpClick();
    void applyLightClick();

private:
    // UI drawing methods for the menu
    void drawMenuButton();
    void drawMenuBox();
    void drawLightControls();
    void drawHelpWindow();
    void drawValueButton(UIButton button, float value);

    void applyLightValueClick(int mouseX, int mouseY, UIButton button);

    // Helper methods
    void setupUI2D();
    void restoreUI3D();
};

void drawButton(UIButton button);
void renderText2D(const char *text, float x, float y);

#endif