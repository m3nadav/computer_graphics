// axes_vector_rotation.cpp
// Compile with: g++ axes_vector_rotation.cpp -o axes_vector_rotation -lGL -lGLU -lglut
#include <GLUT/glut.h>
#include <cmath>
#include <cstdio>

// Vector definition (not aligned with any axis)
float vec[3] = {1.0f, 2.0f, 3.0f};

// Normalize a vector
void normalize(float v[3])
{
    float len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len > 0.0f)
    {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}

// Cross product
void cross(const float a[3], const float b[3], float out[3])
{
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

// Dot product
float dot(const float a[3], const float b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// Compute rotation matrix to rotate 'from' to 'to'
void computeRotationMatrix(const float from[3], const float to[3], float matrix[16])
{
    float v[3];
    cross(from, to, v);
    float c = dot(from, to);
    float s = std::sqrt((1 + c) * 2);
    float invs = 1.0f / s;

    // Quaternion to matrix
    float q[4] = {s * 0.5f, v[0] * invs, v[1] * invs, v[2] * invs};
    float qw = q[0], qx = q[1], qy = q[2], qz = q[3];
    matrix[0] = 1 - 2 * qy * qy - 2 * qz * qz;
    matrix[1] = 2 * qx * qy - 2 * qz * qw;
    matrix[2] = 2 * qx * qz + 2 * qy * qw;
    matrix[3] = 0;
    matrix[4] = 2 * qx * qy + 2 * qz * qw;
    matrix[5] = 1 - 2 * qx * qx - 2 * qz * qz;
    matrix[6] = 2 * qy * qz - 2 * qx * qw;
    matrix[7] = 0;
    matrix[8] = 2 * qx * qz - 2 * qy * qw;
    matrix[9] = 2 * qy * qz + 2 * qx * qw;
    matrix[10] = 1 - 2 * qx * qx - 2 * qy * qy;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}

void drawAxes()
{
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // X axis (red)
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);
    // Y axis (green)
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);
    // Z axis (blue)
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();
}

void drawVector(const float v[3], float r, float g, float b)
{
    glColor3f(r, g, b);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(v[0], v[1], v[2]);
    glEnd();
}

void drawText(float x, float y, const char *str)
{
    glRasterPos2f(x, y);
    for (const char *c = str; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(4, 4, 4, 0, 0, 0, 0, 1, 0);

    drawAxes();

    // Draw original vector (magenta)
    drawVector(vec, 1, 0, 1);

    // Compute rotation matrix to align vec to z axis
    float from[3] = {vec[0], vec[1], vec[2]};
    normalize(from);
    float to[3] = {0, 0, 1};
    float rotMat[16];
    computeRotationMatrix(from, to, rotMat);

    // Draw rotated vector (yellow)
    glPushMatrix();
    glMultMatrixf(rotMat);
    drawVector(from, 1, 1, 0);
    glPopMatrix();

    // Show calculation as text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    char buf[256];
    snprintf(buf, sizeof(buf), "Original vector: [%.2f, %.2f, %.2f]", vec[0], vec[1], vec[2]);
    glColor3f(1, 1, 1);
    drawText(10, 570, buf);
    drawText(10, 540, "Rotating magenta vector to align with Z axis (yellow)");
    drawText(10, 510, "Rotation matrix:");
    for (int i = 0; i < 4; ++i)
    {
        snprintf(buf, sizeof(buf), "[%.2f %.2f %.2f %.2f]", rotMat[i], rotMat[i + 4], rotMat[i + 8], rotMat[i + 12]);
        drawText(10, 490 - 20 * i, buf);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 1, 20);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Axes, Vector, and Rotation Matrix");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}