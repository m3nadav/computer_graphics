#ifndef COW_H
#define COW_H

// Main function to draw the cow
void drawCow();

// Cow movement functions
void initCowMovement();
void updateCowPosition();
void handleCowMovement(unsigned char key, int x, int y);
void handleCowSpecialKeys(int key, int x, int y);

// Cow state getters
float getCowX();
float getCowZ();
float getCowRotation();

// Head movement functions
float getHeadRotationX(); // Up/down rotation (around Z-axis)
float getHeadRotationY(); // Left/right rotation (around Y-axis)

// Cow body movement functions (unified head and tail controls)
void handleCowBodyMovement(unsigned char key, int x, int y);

// Tail movement functions
float getTailRotationZ(); // SHIFT+I/K rotation (around Z-axis)
float getTailRotationX(); // SHIFT+J/L rotation (around X-axis)

#endif // COW_H