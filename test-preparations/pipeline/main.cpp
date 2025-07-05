#include <GLUT/glut.h>
#include <cstdio>
#include <cstdlib>

// Pipeline stage headers
void setProjection2D(int w, int h);
void setProjection3D(int w, int h);
void setView2D();
void setView3D();
void drawWorld2D();
void drawWorld3D();
void handle2DKey(unsigned char key);
void handle2DSpecial(int key);
void handle3DKey(unsigned char key);
void handle3DSpecial(int key);
void handle3DCameraKey(unsigned char key);
void handle3DCameraSpecial(int key);
const char *getCameraModeName();

// State
enum SceneMode
{
    MODE_2D,
    MODE_3D
};
SceneMode mode = MODE_2D;
int winWidth = 800, winHeight = 600;

// Forward declaration for status text
typedef void (*DrawTextFunc)(float, float, const char *);
void drawText(float x, float y, const char *str)
{
    glRasterPos2f(x, y);
    for (const char *c = str; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

extern bool paused;
extern bool paused3D;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // --- Projection Stage ---
    if (mode == MODE_2D)
        setProjection2D(winWidth, winHeight);
    else
        setProjection3D(winWidth, winHeight);
    // --- Viewing Stage ---
    if (mode == MODE_2D)
        setView2D();
    else
        setView3D();
    // --- World & Modeling Stages ---
    if (mode == MODE_2D)
        drawWorld2D();
    else
        drawWorld3D();
    // --- Status Overlay ---
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    char buf[128];
    snprintf(buf, sizeof(buf), "Mode: %s", mode == MODE_2D ? "2D" : "3D");
    glColor3f(1, 1, 1);
    drawText(10, winHeight - 30, buf);
    snprintf(buf, sizeof(buf), "%s", (mode == MODE_2D ? (paused ? "Paused (p to play)" : "Playing (p to pause)") : (paused3D ? "Paused (p to play)" : "Playing (p to pause)")));
    glColor3f(1, 1, 0);
    drawText(10, winHeight - 60, buf);
    if (mode == MODE_3D)
    {
        snprintf(buf, sizeof(buf), "Camera: %s (c to switch)", getCameraModeName());
        glColor3f(0.5, 1, 1);
        drawText(10, winHeight - 90, buf);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
}

void timer(int v)
{
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == ' ')
    {
        mode = (mode == MODE_2D) ? MODE_3D : MODE_2D;
        printf("Switched to %s mode\n", mode == MODE_2D ? "2D" : "3D");
    }
    if (key == 27)
        exit(0); // ESC to quit
    if (mode == MODE_2D)
        handle2DKey(key);
    else
    {
        handle3DKey(key);
        handle3DCameraKey(key);
    }
}

void special(int key, int x, int y)
{
    if (mode == MODE_2D)
        handle2DSpecial(key);
    else
    {
        handle3DSpecial(key);
        handle3DCameraSpecial(key);
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("3D Viewing Pipeline: 2D & 3D Train Scene");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(16, timer, 0);
    printf("Press SPACE to toggle between 2D and 3D scenes.\n");
    printf("Press 'p' to pause/play. Use left/right arrows to step when paused.\n");
    glutMainLoop();
    return 0;
}