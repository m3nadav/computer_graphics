#include "models.h"
#include <GLUT/glut.h>
#include <cmath>
#include <ctime>

extern void drawTrain2D(float wheelAngleL, float wheelAngleR);

// Animation state
double startTime = -1;
float worldLeft = -1.2f;
float worldRight = 1.2f;
float trainY = 0.0f;
float trainZ = 0.0f;
float duration = 10.0f; // seconds

// Pause/step state
bool paused = false;
int currentFrame = 0;
const int framesPerSecond = 60;
const int totalFrames = int(duration * framesPerSecond);
double pausedElapsed = 0.0;

void drawWorld2D()
{
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    if (startTime < 0)
        startTime = t;
    double elapsed;
    if (paused)
    {
        elapsed = pausedElapsed;
    }
    else
    {
        elapsed = t - startTime;
        currentFrame = int(fmod(elapsed, duration) * framesPerSecond);
    }
    // Compute train position (linear interpolation)
    float x = worldLeft + (worldRight - worldLeft) * fmod(elapsed, duration) / duration;
    // Wheel rotation: angle = distance / radius
    float wheelR = 0.13f;
    float distance = x - worldLeft;
    float circumference = 2.0f * 3.1415926f * wheelR;
    float angle = -360.0f * distance / circumference; // negative for correct direction
    glPushMatrix();
    glTranslatef(x, trainY, trainZ);
    drawTrain2D(angle, angle);
    glPopMatrix();
}

// Keyboard and special key handlers for pause/play/step
void handle2DKey(unsigned char key)
{
    if (key == 'p' || key == 'P')
    {
        if (!paused)
        {
            // Pausing: record elapsed
            double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
            pausedElapsed = t - startTime;
        }
        else
        {
            // Unpausing: adjust startTime so animation resumes smoothly
            double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
            startTime = t - pausedElapsed;
        }
        paused = !paused;
    }
}
void handle2DSpecial(int key)
{
    if (paused)
    {
        if (key == GLUT_KEY_RIGHT)
        {
            currentFrame = (currentFrame + 1) % totalFrames;
            pausedElapsed = double(currentFrame) / framesPerSecond;
            glutPostRedisplay();
        }
        else if (key == GLUT_KEY_LEFT)
        {
            currentFrame = (currentFrame + totalFrames - 1) % totalFrames;
            pausedElapsed = double(currentFrame) / framesPerSecond;
            glutPostRedisplay();
        }
    }
}