#ifndef COW_H
#define COW_H

// Main function to draw the cow
void drawCow();

// Cow movement functions
void initCowMovement();
bool handleCowMovement(unsigned char key, int x, int y);

/**
 * Cow state getters.
 * There are only x/z getters because the cow is always at the same height (y)
 */
float getCowX();
float getCowZ();
float getCowRotation();

// Head movement functions
float getHeadRotationZ(); // Up/down rotation (around Z-axis)
float getHeadRotationY(); // Left/right rotation (around Y-axis)

// Cow head and tail movement functions
bool handleCowHeadAndTailMovement(unsigned char key, int x, int y);

// Tail movement functions
float getTailRotationZ(); // Up/down rotation (around Z-axis)
float getTailRotationY(); // Left/right rotation (around X-axis)

// Auto-enable controls when drawing
class InputHandler;
void setCowInputHandler(InputHandler *handler);

#endif