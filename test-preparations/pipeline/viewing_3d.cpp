#include <GLUT/glut.h>
#include <cmath>
#include <cstdio>

// Camera modes
enum CameraMode
{
    CAMERA_SIMPLE,
    CAMERA_ORBIT
};
CameraMode cameraMode = CAMERA_SIMPLE;

// Camera state for simple mode
float eyeX = 3.0f, eyeY = 2.0f, eyeZ = 4.0f;
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;

// Camera state for orbit mode
float orbitAzimuth = 45.0f;   // degrees
float orbitElevation = 30.0f; // degrees
float orbitRadius = 5.0f;

void setView3D()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (cameraMode == CAMERA_SIMPLE)
    {
        gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
    }
    else
    {
        // Orbit mode: calculate eye position from spherical coordinates
        float az = orbitAzimuth * 3.1415926f / 180.0f;
        float el = orbitElevation * 3.1415926f / 180.0f;
        float ex = orbitRadius * cosf(el) * cosf(az);
        float ey = orbitRadius * sinf(el);
        float ez = orbitRadius * cosf(el) * sinf(az);
        gluLookAt(ex, ey, ez, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
}

// Camera control functions
void handle3DCameraKey(unsigned char key)
{
    float step = 0.2f;
    float orbitStep = 5.0f;
    float elevStep = 3.0f;
    if (key == 'c' || key == 'C')
    {
        cameraMode = (cameraMode == CAMERA_SIMPLE) ? CAMERA_ORBIT : CAMERA_SIMPLE;
    }
    if (cameraMode == CAMERA_SIMPLE)
    {
        if (key == 'w' || key == 'W')
            eyeZ -= step;
        if (key == 's' || key == 'S')
            eyeZ += step;
        if (key == 'a' || key == 'A')
            eyeX -= step;
        if (key == 'd' || key == 'D')
            eyeX += step;
        if (key == 'q' || key == 'Q')
            eyeY += step;
        if (key == 'e' || key == 'E')
            eyeY -= step;
    }
    else
    {
        if (key == 'w' || key == 'W')
            orbitElevation += elevStep;
        if (key == 's' || key == 'S')
            orbitElevation -= elevStep;
        if (key == 'a' || key == 'A')
            orbitAzimuth -= orbitStep;
        if (key == 'd' || key == 'D')
            orbitAzimuth += orbitStep;
    }
}
void handle3DCameraSpecial(int key)
{
    float step = 0.2f;
    float orbitStep = 5.0f;
    float elevStep = 3.0f;
    if (cameraMode == CAMERA_SIMPLE)
    {
        if (key == GLUT_KEY_UP)
            eyeZ -= step;
        if (key == GLUT_KEY_DOWN)
            eyeZ += step;
        if (key == GLUT_KEY_LEFT)
            eyeX -= step;
        if (key == GLUT_KEY_RIGHT)
            eyeX += step;
    }
    else
    {
        if (key == GLUT_KEY_UP)
            orbitElevation += elevStep;
        if (key == GLUT_KEY_DOWN)
            orbitElevation -= elevStep;
        if (key == GLUT_KEY_LEFT)
            orbitAzimuth -= orbitStep;
        if (key == GLUT_KEY_RIGHT)
            orbitAzimuth += orbitStep;
    }
}

// For overlay in main.cpp
const char *getCameraModeName()
{
    return cameraMode == CAMERA_SIMPLE ? "Simple" : "Orbit";
}