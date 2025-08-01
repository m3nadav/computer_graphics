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
void handleHeadMovement(unsigned char key, int x, int y);
float getHeadRotationX(); // Up/down rotation (around Z-axis)
float getHeadRotationY(); // Left/right rotation (around Y-axis)

#endif // COW_H