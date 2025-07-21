#ifndef COW_TEST_H
#define COW_TEST_H

// Test scene drawing functions
void drawTestSpot();
void drawTestProjectedSpot();
void drawTestCow();
void drawTestCowHead();

// OpenGL callback functions
void display();
void reshape(int w, int h);
void init();

#endif // COW_TEST_H