#include "models.h"
#include <GLUT/glut.h>
#include <cmath>
#include <ctime>

extern void drawTrain3D(float wheelAngleL, float wheelAngleR);

// Animation state
double startTime3D = -1;
float worldLeft3D = -1.2f;
float worldRight3D = 1.2f;
float trainY3D = 0.0f;
float trainZ3D = 0.0f;
float duration3D = 10.0f; // seconds

// Pause/step state
bool paused3D = false;
int currentFrame3D = 0;
const int framesPerSecond3D = 60;
const int totalFrames3D = int(duration3D * framesPerSecond3D);
double pausedElapsed3D = 0.0;

void drawWorld3D()
{
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    if (startTime3D < 0)
        startTime3D = t;
    double elapsed;
    if (paused3D)
    {
        elapsed = pausedElapsed3D;
    }
    else
    {
        elapsed = t - startTime3D;
        currentFrame3D = int(fmod(elapsed, duration3D) * framesPerSecond3D);
    }
    // Compute train position (linear interpolation)
    float x = worldLeft3D + (worldRight3D - worldLeft3D) * fmod(elapsed, duration3D) / duration3D;
    // Wheel rotation: angle = distance / radius
    float wheelR = 0.15f;
    float distance = x - worldLeft3D;
    float circumference = 2.0f * 3.1415926f * wheelR;
    float angle = -360.0f * distance / circumference; // negative for correct direction
    glPushMatrix();
    glTranslatef(x, trainY3D, trainZ3D);
    drawTrain3D(angle, angle);
    glPopMatrix();
}

// Keyboard and special key handlers for pause/play/step
void handle3DKey(unsigned char key)
{
    if (key == 'p' || key == 'P')
    {
        if (!paused3D)
        {
            // Pausing: record elapsed
            double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
            pausedElapsed3D = t - startTime3D;
        }
        else
        {
            // Unpausing: adjust startTime so animation resumes smoothly
            double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
            startTime3D = t - pausedElapsed3D;
        }
        paused3D = !paused3D;
    }
}
void handle3DSpecial(int key)
{
    if (paused3D)
    {
        if (key == GLUT_KEY_RIGHT)
        {
            currentFrame3D = (currentFrame3D + 1) % totalFrames3D;
            pausedElapsed3D = double(currentFrame3D) / framesPerSecond3D;
            glutPostRedisplay();
        }
        else if (key == GLUT_KEY_LEFT)
        {
            currentFrame3D = (currentFrame3D - 1 + totalFrames3D) % totalFrames3D;
            pausedElapsed3D = double(currentFrame3D) / framesPerSecond3D;
            glutPostRedisplay();
        }
    }
}