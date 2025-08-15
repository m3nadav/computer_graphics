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

void UIButton::updateButton(float x, float y, float width, float height, const char *text, UIButton *parent)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->text = text;
    this->parent = parent;
}

MenuSystem::MenuSystem() {}
void MenuSystem::drawUI()
{
    setupUI2D();

    // Draw menu button
    drawMenuButton();
    drawMenuBox();
    drawLightControls();
    drawHelpWindow();

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

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);

    float buttonWidth = 80.0f;
    float buttonHeight = 30.0f;
    float buttonX = windowWidth - buttonWidth - 10.0f;
    float buttonY = 10.0f;

    menuButton.updateButton(buttonX, buttonY, buttonWidth, buttonHeight, "Menu");
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

    lightButton.updateButton(itemX, menuY + 10.0f, itemWidth, itemHeight, "Light Controls", &menuButton);
    lightButton.draw();

    helpButton.updateButton(itemX, menuY + 50.0f, itemWidth, itemHeight, "Help (H)", &menuButton);
    helpButton.draw();

    quitButton.updateButton(itemX, menuY + 90.0f, itemWidth, itemHeight, "Quit (Esc)", &menuButton);
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

    intensityButton.updateButton(itemX, currentY, itemWidth, itemHeight, "Intensity", &lightButton);
    drawValueButton(intensityButton, getLightIntensity());
    currentY += itemHeight + 5.0f;

    positionXButton.updateButton(itemX, currentY, itemWidth, itemHeight, "Position X", &lightButton);
    drawValueButton(positionXButton, getLightPositionX());
    currentY += itemHeight + 5.0f;

    positionYButton.updateButton(itemX, currentY, itemWidth, itemHeight, "Position Y", &lightButton);
    drawValueButton(positionYButton, getLightPositionY());
    currentY += itemHeight + 5.0f;

    positionZButton.updateButton(itemX, currentY, itemWidth, itemHeight, "Position Z", &lightButton);
    drawValueButton(positionZButton, getLightPositionZ());
    currentY += itemHeight + 5.0f;

    // Ambient Level - always recalculate position
    ambientButton.updateButton(itemX, currentY, itemWidth, itemHeight, "Ambient", &lightButton);
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

    // Calculate dimensions for sub-buttons
    float buttonSize = button.height - 4.0f; // Square buttons slightly smaller than main button height
    float leftButtonX = button.x + 2.0f;
    float rightButtonX = button.x + button.width - buttonSize - 2.0f;
    float buttonY = button.y + 2.0f;

    // Draw main button background
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Draw main button border
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Draw decrease button (left side) - light colored square
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(leftButtonX, buttonY);
    glVertex2f(leftButtonX + buttonSize, buttonY);
    glVertex2f(leftButtonX + buttonSize, buttonY + buttonSize);
    glVertex2f(leftButtonX, buttonY + buttonSize);
    glEnd();

    // Draw decrease button border
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(leftButtonX, buttonY);
    glVertex2f(leftButtonX + buttonSize, buttonY);
    glVertex2f(leftButtonX + buttonSize, buttonY + buttonSize);
    glVertex2f(leftButtonX, buttonY + buttonSize);
    glEnd();

    // Draw increase button (right side) - light colored square
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(rightButtonX, buttonY);
    glVertex2f(rightButtonX + buttonSize, buttonY);
    glVertex2f(rightButtonX + buttonSize, buttonY + buttonSize);
    glVertex2f(rightButtonX, buttonY + buttonSize);
    glEnd();

    // Draw increase button border
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(rightButtonX, buttonY);
    glVertex2f(rightButtonX + buttonSize, buttonY);
    glVertex2f(rightButtonX + buttonSize, buttonY + buttonSize);
    glVertex2f(rightButtonX, buttonY + buttonSize);
    glEnd();

    // Draw - symbol in left button
    glColor3f(0.2f, 0.2f, 0.2f);
    renderText2D("-", leftButtonX + buttonSize / 2 - 4.0f, buttonY + buttonSize / 2 + 5.0f);

    // Draw + symbol in right button
    glColor3f(0.2f, 0.2f, 0.2f);
    renderText2D("+", rightButtonX + buttonSize / 2 - 4.0f, buttonY + buttonSize / 2 + 5.0f);

    // Draw label at center-left of main button
    glColor3f(1.0f, 1.0f, 1.0f);
    float centerX = button.x + button.width / 2.0f;
    float labelWidth = strlen(button.text) * 6.0f; // Estimate text width
    renderText2D(button.text, centerX - labelWidth / 2.0f, button.y + button.height / 2 + 5.0f);

    // Draw value below the label
    char valueStr[32];
    snprintf(valueStr, sizeof(valueStr), "%.2f", value);
    float valueWidth = strlen(valueStr) * 6.0f; // Estimate text width
    renderText2D(valueStr, centerX - valueWidth / 2.0f, button.y + button.height / 2 - 8.0f);
}

void MenuSystem::applyLightValueClick(int mouseX, int mouseY, UIButton button)
{
    if (!button.isVisible())
        return;

    // Calculate dimensions for sub-buttons (same as in drawValueButton)
    float buttonSize = button.height - 4.0f;
    float leftButtonX = button.x + 2.0f;
    float rightButtonX = button.x + button.width - buttonSize - 2.0f;
    float buttonY = button.y + 2.0f;

    // Convert mouse Y coordinate (GLUT uses inverted Y)
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float adjustedMouseY = windowHeight - mouseY;

    // Check if click was specifically on the decrease button (left)
    bool clickedDecrease = (mouseX >= leftButtonX && mouseX <= leftButtonX + buttonSize &&
                            adjustedMouseY >= buttonY && adjustedMouseY <= buttonY + buttonSize);

    // Check if click was specifically on the increase button (right)
    bool clickedIncrease = (mouseX >= rightButtonX && mouseX <= rightButtonX + buttonSize &&
                            adjustedMouseY >= buttonY && adjustedMouseY <= buttonY + buttonSize);

    // Only proceed if one of the specific buttons was clicked
    if (!clickedDecrease && !clickedIncrease)
        return;

    // Get current values
    float ambient = getAmbientLevel();
    float intensity = getLightIntensity();
    float posX = getLightPositionX();
    float posY = getLightPositionY();
    float posZ = getLightPositionZ();

    // Adjust values based on type and which button was clicked
    if (strcmp(button.text, "Intensity") == 0)
    {
        float newIntensity = clickedIncrease ? intensity + 0.1f : intensity - 0.1f;
        setLightIntensity(newIntensity);
        enableCustomLightPosition(true); // Enable custom mode when adjusting
    }
    else if (strcmp(button.text, "Position X") == 0)
    {
        float newPosX = clickedIncrease ? posX + 2.0f : posX - 2.0f;
        setLightPosition(newPosX, posY, posZ);
    }
    else if (strcmp(button.text, "Position Y") == 0)
    {
        float newPosY = clickedIncrease ? posY + 2.0f : posY - 2.0f;
        // Clamp Y position to reasonable range
        if (newPosY < 1.0f)
            newPosY = 1.0f;
        if (newPosY > 50.0f)
            newPosY = 50.0f;
        setLightPosition(posX, newPosY, posZ);
    }
    else if (strcmp(button.text, "Position Z") == 0)
    {
        float newPosZ = clickedIncrease ? posZ + 2.0f : posZ - 2.0f;
        setLightPosition(posX, posY, newPosZ);
    }
    else if (strcmp(button.text, "Ambient") == 0)
    {
        float newAmbient = clickedIncrease ? ambient + 0.05f : ambient - 0.05f;
        setAmbientLevel(newAmbient);
        enableCustomLightPosition(true); // Enable custom mode when adjusting
    }

    glutPostRedisplay(); // Trigger a redraw to show changes
}

void MenuSystem::drawHelpWindow()
{
    // Help window is only visible when the help button is active
    if (!helpButton.isActive)
        return;

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    float helpWidth = 510.0f;
    float helpHeight = 450.0f;
    float helpX = (windowWidth - helpWidth) / 2.0f;   // Center horizontally
    float helpY = (windowHeight - helpHeight) / 2.0f; // Center vertically

    // Draw help window background
    glColor3f(0.1f, 0.1f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(helpX, helpY);
    glVertex2f(helpX + helpWidth, helpY);
    glVertex2f(helpX + helpWidth, helpY + helpHeight);
    glVertex2f(helpX, helpY + helpHeight);
    glEnd();

    // Draw help window border
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(helpX, helpY);
    glVertex2f(helpX + helpWidth, helpY);
    glVertex2f(helpX + helpWidth, helpY + helpHeight);
    glVertex2f(helpX, helpY + helpHeight);
    glEnd();

    // Title
    glColor3f(1.0f, 1.0f, 0.3f);
    float titleY = helpY + helpHeight - 30.0f;
    renderText2D("CONTROLS HELP", helpX + helpWidth / 2 - 65.0f, titleY);

    // Help content
    glColor3f(0.9f, 0.9f, 0.9f);
    float currentY = titleY - 30.0f;
    float lineHeight = 15.0f;
    float leftMargin = helpX + 20.0f;
    float rightMargin = helpX + helpWidth / 2 + 10.0f;

    // Left column - General & Cow Controls
    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("GENERAL", leftMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("ESC...............Exit", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("M....................Menu", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("H.....................Help", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("R.....................Reset scene", leftMargin, currentY);
    currentY -= lineHeight + 5.0f;

    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("COW MOVEMENT", leftMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("W....................Forward", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("S.....................Backward", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("A.....................Turn left", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("D.....................Turn right", leftMargin, currentY);
    currentY -= lineHeight + 5.0f;

    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("COW HEAD", leftMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("SHIFT+W......Head up", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("SHIFT+S.......Head down", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("SHIFT+A.......Head left", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("SHIFT+D.......Head right", leftMargin, currentY);
    currentY -= lineHeight + 5.0f;

    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("COW TAIL", leftMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("ALT+W..........Tail up", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("ALT+S...........Tail down", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("ALT+A...........Tail left", leftMargin, currentY);
    currentY -= lineHeight;
    renderText2D("ALT+D...........Tail right", leftMargin, currentY);

    // Right column - Camera & Lighting
    currentY = titleY - 30.0f;
    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("CAMERA", rightMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("+/=......................Zoom in", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("-/_......................Zoom out", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("C.........................Toggle mode", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("Right Mouse.....Rotate view", rightMargin, currentY);
    currentY -= lineHeight + 5.0f;

    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("LAMP", rightMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("T.........................Toggle light", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("I...........................Direction up", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("K.........................Direction down", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("J.........................Direction left", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("L.........................Direction right", rightMargin, currentY);
    currentY -= lineHeight + 5.0f;

    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("SUN POSITION", rightMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("1..........................Top-left", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("2..........................Top-right", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("3..........................Bottom-left", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("4..........................Bottom-right", rightMargin, currentY);
    currentY -= lineHeight + 5.0f;

    glColor3f(0.8f, 0.9f, 1.0f);
    renderText2D("MENU", rightMargin, currentY);
    glColor3f(0.9f, 0.9f, 0.9f);
    currentY -= lineHeight + 3.0f;
    renderText2D("Left Click...........Menu buttons", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("Click the light control +/- to adjust:", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("• Light intensity", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("• Light position", rightMargin, currentY);
    currentY -= lineHeight;
    renderText2D("• Ambient level", rightMargin, currentY);

    // Close instruction at bottom
    glColor3f(1.0f, 0.8f, 0.3f);
    renderText2D("Press H to close", helpX + helpWidth / 2 - 50.0f, helpY + 15.0f);
}
