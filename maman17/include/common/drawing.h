#ifndef DRAWING_H
#define DRAWING_H

#include <string>

// Common drawing utilities used across test files
namespace DrawingUtils
{
    // Grid drawing functions
    void drawGrid(float size, float step);
    void drawGrid(float size = 10.0f, float step = 1.0f); // Default overload

    // Coordinate axes drawing (X=red, Y=green, Z=blue)
    void drawAxes(float length = 2.0f);

    // Text rendering in 3D space
    void renderText(const std::string &text, float x, float y, float z);

    // Material setup helper
    void setMaterial(float r, float g, float b);

    // Common OpenGL initialization
    void initOpenGL();
    void setupLighting();
}

#endif // DRAWING_H