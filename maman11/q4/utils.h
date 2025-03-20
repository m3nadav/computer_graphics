#ifndef UTILS_H
#define UTILS_H

int getExitButtonX(int windowWidth, int exitButtonWidth);

void drawText(const char *text, float x, float y, float r, float g, float b, void *font);
void drawCircle(float x, float y, float radius);
void drawBodyTriangle(float pointyX, float pointyY, float bodyWidth, float bodyHeight, bool isPointingUp);
void drawRectangle(float x, float y, float length, float width);

#endif
