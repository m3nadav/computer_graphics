#ifndef SHAPES_H
#define SHAPES_H

void drawSphere(double radius);
void drawEllipsoid(double rx, double ry, double rz);
void drawCylinder(double baseRadius, double topRadius, double height);

// Texture functions
unsigned int loadTexture(const char *filename);
void drawTexturedEllipsoid(double rx, double ry, double rz, unsigned int textureID, bool mirrored = false);

#endif // SHAPES_H