#ifndef MENU_H
#define MENU_H

// Menu system for OpenGL applications
class MenuSystem
{
public:
    MenuSystem();
    
    // Menu state management
    void toggleMenu() { menuVisible = !menuVisible; }
    bool isMenuVisible() const { return menuVisible; }
    void setMenuVisible(bool visible) { menuVisible = visible; }
    
    // Drawing functions
    void drawUI();
    
    // Input handling - returns true if click was handled by menu system
    bool handleMenuClick(int mouseX, int mouseY);

private:
    bool menuVisible;
    
    // UI drawing methods
    void drawMenuButton();
    void drawMenuBox();
    void drawButton(float x, float y, float width, float height, const char* text);
    void renderText2D(const char* text, float x, float y);
    
    // Helper methods
    bool isPointInButton(int mouseX, int mouseY, float buttonX, float buttonY, float width, float height);
    void setupUI2D();
    void restoreUI3D();
};

#endif // MENU_H