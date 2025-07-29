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

#endif // COW_H