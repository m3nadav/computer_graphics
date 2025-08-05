#include "common/menu.h"
#include "environment/lights.h"
#include <GLUT/glut.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include "menu.h"

// UIButton method implementations
bool UIButton::isButtonClicked(int mouseX, int mouseY)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float adjustedMouseY = windowHeight - mouseY;
    return (mouseX >= x && mouseX <= x + width &&
            adjustedMouseY >= y && adjustedMouseY <= y + height);
}

void UIButton::draw()
{
    drawButton(*this);
}

void UIButton::toggleActive()
{
    isActive = !isActive;
}

bool UIButton::isVisible() const
{
    return (parent == nullptr || parent->isActive);
}

MenuSystem::MenuSystem() {}
void MenuSystem::drawUI()
{
    setupUI2D();

    // Draw menu button
    drawMenuButton();
    drawMenuBox();
    drawLightControls();
    // drawHelpWindow();

    restoreUI3D();
}

void MenuSystem::handleMenuClick(int mouseX, int mouseY)
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    if (menuButton.isButtonClicked(mouseX, mouseY))
        applyMenuClick();
    else if (lightButton.isButtonClicked(mouseX, mouseY))
        applyLightClick();
    else if (helpButton.isButtonClicked(mouseX, mouseY))
        applyHelpClick();
    else if (quitButton.isButtonClicked(mouseX, mouseY))
        applyQuitClick();
    else if (ambientButton.isButtonClicked(mouseX, mouseY))
        applyLightValueClick(mouseX, mouseY, ambientButton);
    else if (intensityButton.isButtonClicked(mouseX, mouseY))
        applyLightValueClick(mouseX, mouseY, intensityButton);
    else if (positionXButton.isButtonClicked(mouseX, mouseY))
        applyLightValueClick(mouseX, mouseY, positionXButton);
    else if (positionYButton.isButtonClicked(mouseX, mouseY))
        applyLightValueClick(mouseX, mouseY, positionYButton);
    else if (positionZButton.isButtonClicked(mouseX, mouseY))
        applyLightValueClick(mouseX, mouseY, positionZButton);
}

void MenuSystem::applyMenuClick()
{
    menuButton.toggleActive();
}

void MenuSystem::applyQuitClick()
{
    quitButton.toggleActive();
    if (quitButton.isActive)
        exit(0);
}

void MenuSystem::applyHelpClick()
{
    helpButton.toggleActive();
    if (helpButton.isActive)
        lightButton.isActive = false;
}

void MenuSystem::applyLightClick()
{
    lightButton.toggleActive();
    if (lightButton.isActive)
    {
        helpButton.isActive = false;
    }
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
    if (menuButton.width == 0)
    {
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);

        float buttonWidth = 80.0f;
        float buttonHeight = 30.0f;
        float buttonX = windowWidth - buttonWidth - 10.0f;
        float buttonY = 10.0f;

        menuButton = UIButton(buttonX, buttonY, buttonWidth, buttonHeight, "Menu");
    }
    menuButton.draw();
}

void MenuSystem::drawMenuBox()
{
    // Menu box is only visible when the menu button is active
    if (!menuButton.isActive)
        return;

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

    if (lightButton.width == 0)
    {
        lightButton = UIButton(itemX, menuY + 10.0f, itemWidth, itemHeight, "Light Controls", &menuButton);
    }
    lightButton.draw();

    if (helpButton.width == 0)
    {
        helpButton = UIButton(itemX, menuY + 50.0f, itemWidth, itemHeight, "Help (H)", &menuButton);
    }
    helpButton.draw();

    if (quitButton.width == 0)
    {
        quitButton = UIButton(itemX, menuY + 90.0f, itemWidth, itemHeight, "Quit (Esc)", &menuButton);
    }
    quitButton.draw();
}

void drawButton(UIButton button)
{
    if (!button.isVisible())
        return;

    // Draw button background
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Draw button border
    glColor3f(0.9f, 0.9f, 0.9f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Draw button text
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText2D(button.text, button.x + 10.0f, button.y + button.height / 2 - 5.0f);
}

void renderText2D(const char *text, float x, float y)
{
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void MenuSystem::drawLightControls()
{
    // Light controls are only visible when the light button is active
    if (!lightButton.isActive || !lightButton.isVisible())
        return;

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);

    float controlsWidth = 300.0f;
    float controlsHeight = 280.0f;
    float controlsX = windowWidth - controlsWidth - 220.0f; // Left of main menu
    float controlsY = 50.0f;                                // Same Y as main menu

    // Draw light controls background
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(controlsX, controlsY);
    glVertex2f(controlsX + controlsWidth, controlsY);
    glVertex2f(controlsX + controlsWidth, controlsY + controlsHeight);
    glVertex2f(controlsX, controlsY + controlsHeight);
    glEnd();

    // Draw light controls border
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(controlsX, controlsY);
    glVertex2f(controlsX + controlsWidth, controlsY);
    glVertex2f(controlsX + controlsWidth, controlsY + controlsHeight);
    glVertex2f(controlsX, controlsY + controlsHeight);
    glEnd();

    // Draw control items
    float itemHeight = 35.0f;
    float itemWidth = 280.0f;
    float itemX = controlsX + 10.0f;
    float currentY = controlsY + 15.0f;

    // Light Intensity
    if (intensityButton.width == 0)
    {
        intensityButton = UIButton(itemX, currentY, itemWidth, itemHeight, "Intensity", &lightButton);
    }
    drawValueButton(intensityButton, getLightIntensity());
    currentY += itemHeight + 5.0f;

    // Light Position X
    if (positionXButton.width == 0)
    {
        positionXButton = UIButton(itemX, currentY, itemWidth, itemHeight, "Position X", &lightButton);
    }
    drawValueButton(positionXButton, getLightPositionX());
    currentY += itemHeight + 5.0f;

    // Light Position Y
    if (positionYButton.width == 0)
    {
        positionYButton = UIButton(itemX, currentY, itemWidth, itemHeight, "Position Y", &lightButton);
    }
    drawValueButton(positionYButton, getLightPositionY());
    currentY += itemHeight + 5.0f;

    // Light Position Z
    if (positionZButton.width == 0)
    {
        positionZButton = UIButton(itemX, currentY, itemWidth, itemHeight, "Position Z", &lightButton);
    }
    drawValueButton(positionZButton, getLightPositionZ());
    currentY += itemHeight + 5.0f;

    // Ambient Level
    if (ambientButton.width == 0)
    {
        ambientButton = UIButton(itemX, currentY, itemWidth, itemHeight, "Ambient", &lightButton);
    }
    drawValueButton(ambientButton, getAmbientLevel());
    currentY += itemHeight + 5.0f;

    // Instructions
    glColor3f(0.9f, 0.9f, 0.9f);
    renderText2D("Click values to adjust", itemX, currentY + 10.0f);
    renderText2D("Keys 1-4: preset positions", itemX, currentY + 25.0f);
}

void MenuSystem::drawValueButton(UIButton button, float value)
{
    if (!button.isVisible())
        return;

    // Draw button background
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Draw button border
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Draw label and value
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText2D(button.text, button.x + 10.0f, button.y + button.height / 2 + 5.0f);

    char valueStr[32];
    snprintf(valueStr, sizeof(valueStr), "%.2f", value);
    renderText2D(valueStr, button.x + button.width - 60.0f, button.y + button.height / 2 + 5.0f);

    // Draw increment/decrement indicators
    glColor3f(0.8f, 0.8f, 0.8f);
    renderText2D("- +", button.x + button.width - 40.0f, button.y + button.height / 2 - 8.0f);
}

void MenuSystem::applyLightValueClick(int mouseX, int mouseY, UIButton button)
{
    if (!button.isVisible())
        return;

    // Determine if click was on left (decrease) or right (increase) side of button
    float clickX = mouseX - button.x;
    float clickY = mouseY - button.y;

    bool isIncrement = clickX > (button.width / 2.0f);

    // Get current values
    float ambient = getAmbientLevel();
    float intensity = getLightIntensity();
    float posX = getLightPositionX();
    float posY = getLightPositionY();
    float posZ = getLightPositionZ();

    // Adjust values based on type and increment/decrement
    if (strcmp(button.text, "Intensity") == 0)
    {
        float newIntensity = isIncrement ? intensity + 0.1f : intensity - 0.1f;
        setLightIntensity(newIntensity);
        enableCustomLightPosition(true); // Enable custom mode when adjusting
    }
    else if (strcmp(button.text, "Position X") == 0)
    {
        float newPosX = isIncrement ? posX + 2.0f : posX - 2.0f;
        setLightPosition(newPosX, posY, posZ);
    }
    else if (strcmp(button.text, "Position Y") == 0)
    {
        float newPosY = isIncrement ? posY + 2.0f : posY - 2.0f;
        // Clamp Y position to reasonable range
        if (newPosY < 1.0f)
            newPosY = 1.0f;
        if (newPosY > 50.0f)
            newPosY = 50.0f;
        setLightPosition(posX, newPosY, posZ);
    }
    else if (strcmp(button.text, "Position Z") == 0)
    {
        float newPosZ = isIncrement ? posZ + 2.0f : posZ - 2.0f;
        setLightPosition(posX, posY, newPosZ);
    }
    else if (strcmp(button.text, "Ambient") == 0)
    {
        float newAmbient = isIncrement ? ambient + 0.05f : ambient - 0.05f;
        setAmbientLevel(newAmbient);
        enableCustomLightPosition(true); // Enable custom mode when adjusting
    }

    glutPostRedisplay(); // Trigger a redraw to show changes
}
