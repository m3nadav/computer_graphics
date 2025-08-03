#include "common/menu.h"
#include <GLUT/glut.h>
#include <iostream>
#include <cstdlib>

MenuSystem::MenuSystem() : menuVisible(false)
{
}

void MenuSystem::drawUI()
{
    setupUI2D();
    
    // Draw menu button
    drawMenuButton();
    
    // Draw menu box if visible
    if (menuVisible)
    {
        drawMenuBox();
    }
    
    restoreUI3D();
}

bool MenuSystem::handleMenuClick(int mouseX, int mouseY)
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    // Check if menu button was clicked (top-right corner)
    float buttonWidth = 80.0f;
    float buttonHeight = 30.0f;
    float buttonX = windowWidth - buttonWidth - 10.0f;
    float buttonY = 10.0f;
    
    if (isPointInButton(mouseX, mouseY, buttonX, buttonY, buttonWidth, buttonHeight))
    {
        toggleMenu();
        glutPostRedisplay();
        return true;
    }
    
    // Check menu item clicks if menu is visible
    if (menuVisible)
    {
        float menuX = windowWidth - 200.0f;
        float menuY = 50.0f;
        float itemHeight = 35.0f;
        float itemWidth = 180.0f;
        
        // Adjust coordinates for OpenGL (flip Y)
        int adjustedY = windowHeight - mouseY;
        
        // Check "Adjust Ambient Light" button
        if (isPointInButton(mouseX, adjustedY, menuX, menuY, itemWidth, itemHeight))
        {
            std::cout << "Adjust Ambient Light clicked" << std::endl;
            return true;
        }
        
        // Check "Help" button
        if (isPointInButton(mouseX, adjustedY, menuX, menuY + itemHeight + 5, itemWidth, itemHeight))
        {
            std::cout << "Help clicked - Use H for help, Esc to quit, M to toggle menu" << std::endl;
            return true;
        }
        
        // Check "Quit" button
        if (isPointInButton(mouseX, adjustedY, menuX, menuY + 2*(itemHeight + 5), itemWidth, itemHeight))
        {
            std::cout << "Quit clicked" << std::endl;
            exit(0);
        }
    }
    
    return false; // Click was not handled by menu system
}

void MenuSystem::setupUI2D()
{
    // Save current OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    
    // Setup 2D orthographic projection for UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Disable depth testing and lighting for UI
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void MenuSystem::restoreUI3D()
{
    // Restore OpenGL state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

void MenuSystem::drawMenuButton()
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    
    float buttonWidth = 80.0f;
    float buttonHeight = 30.0f;
    float buttonX = windowWidth - buttonWidth - 10.0f;
    float buttonY = 10.0f;
    
    drawButton(buttonX, buttonY, buttonWidth, buttonHeight, "Menu");
}

void MenuSystem::drawMenuBox()
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    
    float menuWidth = 200.0f;
    float menuHeight = 130.0f;
    float menuX = windowWidth - menuWidth - 10.0f;
    float menuY = 50.0f;
    
    // Draw menu background
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(menuX, menuY);
    glVertex2f(menuX + menuWidth, menuY);
    glVertex2f(menuX + menuWidth, menuY + menuHeight);
    glVertex2f(menuX, menuY + menuHeight);
    glEnd();
    
    // Draw menu border
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(menuX, menuY);
    glVertex2f(menuX + menuWidth, menuY);
    glVertex2f(menuX + menuWidth, menuY + menuHeight);
    glVertex2f(menuX, menuY + menuHeight);
    glEnd();
    
    // Draw menu items
    float itemHeight = 35.0f;
    float itemWidth = 180.0f;
    float itemX = menuX + 10.0f;
    
    // Adjust Ambient Light button
    drawButton(itemX, menuY + 10.0f, itemWidth, itemHeight, "Adjust Ambient Light");
    
    // Help button with keyboard shortcut
    drawButton(itemX, menuY + 50.0f, itemWidth, itemHeight, "Help (H)");
    
    // Quit button with keyboard shortcut
    drawButton(itemX, menuY + 90.0f, itemWidth, itemHeight, "Quit (Esc)");
}

void MenuSystem::drawButton(float x, float y, float width, float height, const char* text)
{
    // Draw button background
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    // Draw button border
    glColor3f(0.9f, 0.9f, 0.9f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    // Draw button text
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText2D(text, x + 10.0f, y + height/2 - 5.0f);
}

void MenuSystem::renderText2D(const char* text, float x, float y)
{
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

bool MenuSystem::isPointInButton(int mouseX, int mouseY, float buttonX, float buttonY, float width, float height)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    // Convert mouse Y coordinate (GLUT uses top-left origin, OpenGL uses bottom-left)
    float adjustedMouseY = windowHeight - mouseY;
    
    return (mouseX >= buttonX && mouseX <= buttonX + width &&
            adjustedMouseY >= buttonY && adjustedMouseY <= buttonY + height);
}