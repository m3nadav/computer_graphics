#include "common/drawing.h"
#include "environment/environment.h"
#include "environment/lights.h"
#include <GLUT/glut.h>

namespace DrawingUtils
{
    /**
     * Renders a wireframe grid for visual reference and debugging.
     * Creates evenly spaced grid lines across a square area to help
     * with spatial orientation and object positioning during development.
     */
    void drawGrid(float size, float step)
    {
        // Save current OpenGL state
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        // Disable lighting and depth testing for grid
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        // Use a consistent grid color
        setGridMaterial();
        glLineWidth(1.0f);

        glBegin(GL_LINES);
        for (float i = -size; i <= size; i += step)
        {
            // Draw lines parallel to X axis
            glVertex3f(-size, 0.0f, i);
            glVertex3f(size, 0.0f, i);
            // Draw lines parallel to Z axis
            glVertex3f(i, 0.0f, -size);
            glVertex3f(i, 0.0f, size);
        }
        glEnd();

        // Restore OpenGL state
        glPopAttrib();
    }

    /**
     * Draws color-coded coordinate axes for 3D orientation reference.
     * Renders X (red), Y (green), and Z (blue) axes to provide visual
     * indication of coordinate system orientation and scale.
     */
    void drawAxes(float length)
    {
        glDisable(GL_LIGHTING);
        glLineWidth(2.0f);

        glBegin(GL_LINES);

        // X-axis (Red)
        setRedAxisMaterial();
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(length, 0.0f, 0.0f);

        // Y-axis (Green)
        setGreenAxisMaterial();
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, length, 0.0f);

        // Z-axis (Blue)
        setBlueAxisMaterial();
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, length);

        glEnd();

        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
    }

    /** Renders text at a 3D position using bitmap fonts for UI and debugging labels. */
    void renderText(const std::string &text, float x, float y, float z)
    {
        glDisable(GL_LIGHTING);
        setBlackTextMaterial();

        glRasterPos3f(x, y, z);
        for (char c : text)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }

        glEnable(GL_LIGHTING);
    }

    /** Sets a basic material color with default shininess and opacity values. */
    void setMaterial(float r, float g, float b)
    {
        setMaterialFromColor(r, g, b, 50.0f, 0.8f);
    }

    /** Initializes essential OpenGL settings for 3D rendering including depth testing and lighting. */
    void initOpenGL()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
    }

    /** Configures standard OpenGL lighting with a single light source and material properties. */
    void setupLighting()
    {
        // Set up standard lighting
        GLfloat light_position[] = {5.0f, 5.0f, 5.0f, 1.0f};
        GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
        GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    }
}