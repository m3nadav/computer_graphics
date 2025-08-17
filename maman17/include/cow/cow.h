#ifndef COW_H
#define COW_H

// Main function to draw the cow
void drawCow();

// Cow movement functions
void initCowMovement();
void updateCowPosition();
bool handleCowMovement(unsigned char key, int x, int y);

// Cow state getters
float getCowX();
float getCowZ();
float getCowRotation();

// Head movement functions
float getHeadRotationX(); // Up/down rotation (around Z-axis)
float getHeadRotationY(); // Left/right rotation (around Y-axis)

// Cow body movement functions (unified head and tail controls)
bool handleCowBodyMovement(unsigned char key, int x, int y);

// Tail movement functions
float getTailRotationZ(); // Up/down rotation (around Z-axis)
float getTailRotationX(); // Left/right rotation (around X-axis)

// Smart cow controls - auto-enable controls when drawing
class InputHandler;
void setCowInputHandler(InputHandler* handler);

#endif // COW_H