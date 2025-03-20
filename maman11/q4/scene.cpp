#define GL_SILENCE_DEPRECATION

#include "utils.h"
#include "scene.h"
#include <GLUT/glut.h>
#include <cmath>

const float HEAD_RADIUS = 30;
const float BODY_HEIGHT = 70;
const float BODY_WIDTH = 40;

// Define the exit button constants
const int EXIT_BUTTON_WIDTH = 80;
const int EXIT_BUTTON_HEIGHT = 30;
const int EXIT_BUTTON_Y = 20;

void drawLeftChild(int windowWidth, int windowHeight)
{
    // Expected to start at x 200, y 200
    const float xAnchor = windowWidth / 3;
    const float yAnchor = windowHeight * 2 / 3;

    // Head
    glColor3f(0.8f, 0.6f, 0.4f); // Skin color
    drawCircle(xAnchor, yAnchor, HEAD_RADIUS);

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f); // Black eyes
    // Left eye
    const float eyeRadius = 5;
    drawCircle(xAnchor - 10, yAnchor + 5, eyeRadius);
    // Right eye
    drawCircle(xAnchor + 10, yAnchor + 5, eyeRadius);

    // Arms
    glColor3f(0.8f, 0.6f, 0.4f); // Skin color
    // Left arm
    drawRectangle(xAnchor - HEAD_RADIUS, yAnchor - BODY_HEIGHT, 10, 10);
    // Right arm
    drawRectangle(xAnchor + HEAD_RADIUS - 10, yAnchor - BODY_HEIGHT, 10, 10);

    // Legs
    glColor3f(0.4f, 0.2f, 0.0f); // Brown pants
    // Left leg
    drawRectangle(xAnchor - 10, yAnchor - (HEAD_RADIUS + 1.44 * BODY_HEIGHT), 5, 65);
    // Right leg
    drawRectangle(xAnchor + 5, yAnchor - (HEAD_RADIUS + 1.44 * BODY_HEIGHT), 5, 65);

    // Body
    glColor3f(0.2f, 0.5f, 0.8f); // Blue shirt
    // point down, right below the head
    drawBodyTriangle(xAnchor, yAnchor - (HEAD_RADIUS + BODY_HEIGHT), BODY_WIDTH, BODY_HEIGHT, false);
}

void drawRightChild(int windowWidth, int windowHeight)
{
    // Expected to start at x 400, y 200
    const float xAnchor = windowWidth * 2 / 3;
    const float yAnchor = windowHeight * 2 / 3;

    // Head
    glColor3f(0.8f, 0.6f, 0.4f); // Skin color
    drawCircle(xAnchor, yAnchor, HEAD_RADIUS);

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f); // Black eyes
    // Left eye
    const float eyeRadius = 5;
    drawCircle(xAnchor - 10, yAnchor + 5, eyeRadius);
    // Right eye
    drawCircle(xAnchor + 10, yAnchor + 5, eyeRadius);

    // Arms
    glColor3f(0.8f, 0.6f, 0.4f); // Skin color
    // Left arm
    drawRectangle(xAnchor - HEAD_RADIUS, yAnchor - BODY_HEIGHT, 10, 10);
    // Right arm
    drawRectangle(xAnchor + HEAD_RADIUS - 10, yAnchor - BODY_HEIGHT, 10, 10);

    // Legs
    glColor3f(0.4f, 0.2f, 0.0f); // Brown pants
    // Left leg
    drawRectangle(xAnchor - 10, yAnchor - (HEAD_RADIUS + 1.44 * BODY_HEIGHT), 5, 35);
    // Right leg
    drawRectangle(xAnchor + 5, yAnchor - (HEAD_RADIUS + 1.44 * BODY_HEIGHT), 5, 35);

    // Body
    glColor3f(0.8f, 0.2f, 0.5f); // Pink shirt
    // point up, right below the head
    drawBodyTriangle(xAnchor, yAnchor - HEAD_RADIUS, BODY_WIDTH, BODY_HEIGHT, true);
}

void drawChildren(int windowWidth, int windowHeight)
{
    drawLeftChild(windowWidth, windowHeight);
    drawRightChild(windowWidth, windowHeight);

    // a limbo stick
    glColor3f(0.54f, 0.27f, 0.07f); // Tree-bark brown
    // (1/3x+50) + (1/3x-100) = 2/3x - 50
    drawRectangle(windowWidth / 3 + 50, windowHeight * 2 / 3 - BODY_HEIGHT, windowWidth / 3 - 100, 10);
}

void drawExitButton(int windowWidth)
{
    glColor3f(0.8f, 0.2f, 0.2f); // Red color
    glBegin(GL_POLYGON);
    glVertex2f(getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH), EXIT_BUTTON_Y);
    glVertex2f(getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH) + EXIT_BUTTON_WIDTH, EXIT_BUTTON_Y);
    glVertex2f(getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH) + EXIT_BUTTON_WIDTH, EXIT_BUTTON_Y + EXIT_BUTTON_HEIGHT);
    glVertex2f(getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH), EXIT_BUTTON_Y + EXIT_BUTTON_HEIGHT);
    glEnd();

    // Calculate text dimensions
    const char *text = "EXIT";
    void *font = GLUT_BITMAP_HELVETICA_12;
    int text_width = glutBitmapLength(font, (const unsigned char *)text);
    int text_height = 12; // GLUT_BITMAP_HELVETICA_12 is 12 pixels high

    // Calculate centered position
    float text_x = getExitButtonX(windowWidth, EXIT_BUTTON_WIDTH) + (EXIT_BUTTON_WIDTH - text_width) / 2.0f;
    float text_y = EXIT_BUTTON_Y + (EXIT_BUTTON_HEIGHT - text_height) / 2.0f;

    // Draw "EXIT" text centered
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    drawText(text, text_x, text_y, 1.0f, 1.0f, 1.0f, font);
}

void drawSceneTitle(int windowWidth, int windowHeight)
{
    const char *text = "Children Playing Limbo";
    void *font = GLUT_BITMAP_TIMES_ROMAN_24;
    int text_width = glutBitmapLength(font, (const unsigned char *)text);
    int text_height = 24; // GLUT_BITMAP_TIMES_ROMAN_24 is 24 pixels high

    drawText(text, windowWidth / 2 - text_width / 2, windowHeight - (1.1 * text_height), 1.0f, 0.65f, 0.0f, font);
}

void drawMyName(int windowWidth, int windowHeight)
{
    const char *text = "Nadav Mermer";
    void *font = GLUT_BITMAP_HELVETICA_18;
    int text_width = glutBitmapLength(font, (const unsigned char *)text);
    int text_height = 18; // GLUT_BITMAP_HELVETICA_18 is 18 pixels high

    drawText(text, windowWidth - (1.1 * text_width), windowHeight - (1.1 * text_height), 0.0f, 0.0f, 0.0f, font);
}

void drawScene(int windowWidth, int windowHeight)
{
    drawChildren(windowWidth, windowHeight);
    drawExitButton(windowWidth);
    drawSceneTitle(windowWidth, windowHeight);
    drawMyName(windowWidth, windowHeight);
}