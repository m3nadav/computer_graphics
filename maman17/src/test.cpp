// Sphere with a directional "Sun" light and a tight-cone spotlight aimed at it.
//
// Controls:
//  - Camera: j/l (azimuth), i/k (elevation), +/- (zoom)
//  - Directional light (Sun): WASD (move direction on XZ plane)
//  - Spotlight: IJKL (move position on XZ plane)
//  - ESC: Exit
//
// Build:
//  - Windows (MinGW): g++ lights_sphere.cpp -lopengl32 -lfreeglut -lglu32 -o lights_sphere.exe
//  - Linux:           g++ lights_sphere.cpp -lGL -lGLU -lglut -o lights_sphere
//  - macOS:           g++ lights_sphere.cpp -framework OpenGL -framework GLUT -o lights_sphere

#include <cmath>
#include <algorithm>
#include <cstdlib>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Simple camera orbit
float angleY = 20.0f;  // elevation
float angleX = -30.0f; // azimuth
float dist = 5.0f;

// Light positions for interactive control
float sunDirX = -0.5f, sunDirZ = 0.3f;  // Directional light direction (XZ plane)
float spotPosX = 1.7f, spotPosZ = 1.2f; // Spotlight position (XZ plane)
float spotPosY = 1.8f;                  // Fixed Y position for spotlight

// Sphere material (shiny so you can see specular highlights)
void setupMaterial()
{
    GLfloat mat_ambient[] = {0.15f, 0.15f, 0.15f, 1.0f};
    GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.65f, 1.0f};
    GLfloat mat_specular[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat mat_shininess[] = {64.0f}; // 0..128

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// Lights: GL_LIGHT0 = directional "Sun", GL_LIGHT1 = spotlight
void setupLights()
{
    glEnable(GL_LIGHTING);

    // ---- Directional light ("Sun"): points from direction toward origin.
    // A directional light uses w=0 in its position. The (x,y,z) gives the direction *from*
    // the scene *toward* the light; OpenGL treats it as coming from the opposite direction.
    GLfloat sun_dir_pos[] = {sunDirX, 1.0f, sunDirZ, 0.0f}; // w=0 => directional
    GLfloat sun_ambient[] = {0.02f, 0.02f, 0.02f, 1.0f};
    GLfloat sun_diffuse[] = {0.9f, 0.9f, 0.85f, 1.0f};
    GLfloat sun_specular[] = {0.9f, 0.9f, 0.9f, 1.0f};

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, sun_dir_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_specular);

    // ---- Spotlight: a positional light (w=1) with direction, cutoff, and exponent.
    // Position it off to the side and aim at the origin (the sphere center).
    GLfloat spot_pos[] = {spotPosX, spotPosY, spotPosZ, 1.0f}; // w=1 => point
    GLfloat spot_target[] = {0.0f, 0.0f, 0.0f};                // aim at origin
    GLfloat spot_dir[] = {spot_target[0] - spotPosX,
                          spot_target[1] - spotPosY,
                          spot_target[2] - spotPosZ};
    // Normalize direction
    float len = std::sqrt(spot_dir[0] * spot_dir[0] + spot_dir[1] * spot_dir[1] + spot_dir[2] * spot_dir[2]);
    if (len > 0.0f)
    {
        spot_dir[0] /= len;
        spot_dir[1] /= len;
        spot_dir[2] /= len;
    }

    GLfloat spot_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat spot_diffuse[] = {0.9f, 0.7f, 0.6f, 1.0f};
    GLfloat spot_specular[] = {0.9f, 0.8f, 0.7f, 1.0f};

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 12.5f);   // Tight cone (degrees: 0..90, or 180 for no cone)
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 40.0f); // Focus intensity toward the center (0..128)
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
    glLightfv(GL_LIGHT1, GL_AMBIENT, spot_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spot_specular);
}

void drawAxes(float L = 1.0f)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    // X red
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(L, 0, 0);
    // Y green
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, L, 0);
    // Z blue
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, L);
    glEnd();
    glEnable(GL_LIGHTING);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera
    // Place camera back by 'dist' and orbit using angles.
    float radX = angleX * 3.1415926f / 180.0f;
    float radY = angleY * 3.1415926f / 180.0f;

    float eyeX = dist * std::cos(radY) * std::cos(radX);
    float eyeY = dist * std::sin(radY);
    float eyeZ = dist * std::cos(radY) * std::sin(radX);

    gluLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);

    // Re-issue light positions/directions in world space (good practice each frame)
    setupLights();

    // Optional axes (unlit)
    drawAxes(1.5f);

    // Sphere at origin
    setupMaterial();
    glColor3f(1, 1, 1); // color is modulated with material only if GL_COLOR_MATERIAL is enabled; here it's unused.
    glutSolidSphere(1.0, 60, 40);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (h > 0 ? (double)w / (double)h : 1.0), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int)
{
    const float lightMoveStep = 0.1f;

    switch (key)
    {
    case 27: // ESC
        std::exit(0);

    // Camera controls (original)
    case 'j':
        angleX -= 5.0f;
        break;
    case 'l':
        angleX += 5.0f;
        break;
    case 'i':
        angleY += 5.0f;
        if (angleY > 89)
            angleY = 89;
        break;
    case 'k':
        angleY -= 5.0f;
        if (angleY < -89)
            angleY = -89;
        break;
    case '+':
        dist = std::max(1.5f, dist - 0.2f);
        break;
    case '-':
        dist = std::min(20.0f, dist + 0.2f);
        break;

    // Directional light controls (WASD)
    case 'w':
    case 'W':
        sunDirZ -= lightMoveStep;
        break;
    case 's':
    case 'S':
        sunDirZ += lightMoveStep;
        break;
    case 'a':
    case 'A':
        sunDirX -= lightMoveStep;
        break;
    case 'd':
    case 'D':
        sunDirX += lightMoveStep;
        break;

    // Spotlight position controls (IJKL - reassigned for light control)
    case 'I':
        spotPosZ -= lightMoveStep;
        break;
    case 'K':
        spotPosZ += lightMoveStep;
        break;
    case 'J':
        spotPosX -= lightMoveStep;
        break;
    case 'L':
        spotPosX += lightMoveStep;
        break;
    }
    glutPostRedisplay();
}

void initGL()
{
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // A little global ambient so the unlit sides aren't pitch black
    GLfloat global_ambient[] = {0.08f, 0.08f, 0.08f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // Nice-looking background
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Sphere lit by Directional Sun + Spotlight");

    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}