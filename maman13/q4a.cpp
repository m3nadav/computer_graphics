#include <GLUT/glut.h> // Apple ships GLUT in a different path
#include <cmath>
#include <cstdlib>
#include <cstdio>

// ---------- window dimensions ------------
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

// ---------- clipping rectangle ------------
const float CLIP_XMIN = 200.f;
const float CLIP_XMAX = 600.f;
const float CLIP_YMIN = 150.f;
const float CLIP_YMAX = 450.f;

// ---------- animation pace ------------
const int STEP_DELAY_MS = 1000;

// ---------- Cohen–Sutherland codes ------------
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

struct Point
{
    float x, y;
};

// ---------- state variables ------------
Point p0, p1;             // original segment endpoints
Point c0, c1;             // current animated segment endpoints
int code0 = 0, code1 = 0; // current outcodes
bool setFirst = false;    // did user set first endpoint?
bool running = false;     // algorithm is in progress
bool accepted = false;    // result flags
bool rejected = false;

// ---------- helpers ------------
int computeOutCode(Point p)
{
    int code = INSIDE;
    if (p.x < CLIP_XMIN)
        code |= LEFT;
    else if (p.x > CLIP_XMAX)
        code |= RIGHT;
    if (p.y < CLIP_YMIN)
        code |= BOTTOM;
    else if (p.y > CLIP_YMAX)
        code |= TOP;
    return code;
}

// Run a single iteration of the algorithm; scheduled via glutTimerFunc
void clipStep(int /*value*/)
{
    if (!running)
        return;

    // choose an endpoint outside
    int outCodeOut = code0 ? code0 : code1;
    Point newPt = {0.f, 0.f};

    // Determine from which direction the point deviates from the square
    if (outCodeOut & TOP)
    { // set y = ymax
        newPt.x = c0.x + (c1.x - c0.x) * (CLIP_YMAX - c0.y) / (c1.y - c0.y);
        newPt.y = CLIP_YMAX;
    }
    else if (outCodeOut & BOTTOM)
    { // set y = ymin
        newPt.x = c0.x + (c1.x - c0.x) * (CLIP_YMIN - c0.y) / (c1.y - c0.y);
        newPt.y = CLIP_YMIN;
    }
    else if (outCodeOut & RIGHT)
    { // set x = xmax
        newPt.y = c0.y + (c1.y - c0.y) * (CLIP_XMAX - c0.x) / (c1.x - c0.x);
        newPt.x = CLIP_XMAX;
    }
    else if (outCodeOut & LEFT)
    { // set x = xmin
        newPt.y = c0.y + (c1.y - c0.y) * (CLIP_XMIN - c0.x) / (c1.x - c0.x);
        newPt.x = CLIP_XMIN;
    }

    // replace the deviating point with the updated point
    if (outCodeOut == code0)
    {
        c0 = newPt;
        code0 = computeOutCode(c0);
    }
    else
    {
        c1 = newPt;
        code1 = computeOutCode(c1);
    }

    // check if we're done
    if ((code0 | code1) == 0)
    { // both inside
        accepted = true;
        running = false;
    }
    else if (code0 & code1)
    { // trivial reject
        rejected = true;
        running = false;
    }
    else
    {
        // schedule next step
        glutTimerFunc(STEP_DELAY_MS, clipStep, 0);
    }
    glutPostRedisplay();
}

// ---------- drawing ------------
void drawRectangle()
{
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(CLIP_XMIN, CLIP_YMIN);
    glVertex2f(CLIP_XMAX, CLIP_YMIN);
    glVertex2f(CLIP_XMAX, CLIP_YMAX);
    glVertex2f(CLIP_XMIN, CLIP_YMAX);
    glEnd();
}

void drawSegment(Point a, Point b, float r, float g, float bcol, float width = 2.f, bool dashed = false)
{
    glLineWidth(width);
    glColor3f(r, g, bcol);
    if (dashed)
    {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
    }
    glBegin(GL_LINES);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glEnd();
    if (dashed)
        glDisable(GL_LINE_STIPPLE);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawRectangle();

    if (setFirst)
    {
        if (!running && !accepted && !rejected)
        {
            // show the first point when we have it but haven't started
            glPointSize(5.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POINTS);
            glVertex2f(p0.x, p0.y);
            glEnd();
        }
        else
        {
            // Show the original segment in gray dashed
            drawSegment(p0, p1, 0.6f, 0.6f, 0.6f, 1.5f, true);
        }
    }

    if (running)
    {
        // Show the current state of the clipping
        drawSegment(c0, c1, 1.f, 1.f, 0.f, 3.f); // yellow
    }

    if (accepted)
    {
        // Show the final clipped segment
        drawSegment(c0, c1, 0.f, 1.f, 0.f, 3.f); // green
    }
    if (rejected)
    {
        // Show the rejected segment
        drawSegment(p0, p1, 1.f, 0.f, 0.f, 3.f); // red
    }

    glutSwapBuffers();
}

// ---------- event handlers ------------
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h); // origin bottom‑left
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    // handle left mouse button down events only
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;

    // convert to window‑space coords (y inverted)
    Point click = {(float)x, (float)(WIN_HEIGHT - y)};

    if (!setFirst)
    {
        printf("Setting first point\n"); // Debug print
        p0 = click;
        setFirst = true;
        running = false;
        accepted = false;
        rejected = false;
        glutPostRedisplay();
    }
    else if (!running && !accepted && !rejected)
    {
        printf("Setting second point and starting animation\n"); // Debug print
        p1 = click;
        // init algorithm vars
        c0 = p0;
        c1 = p1;
        code0 = computeOutCode(c0);
        code1 = computeOutCode(c1);
        printf("Outcodes: code0=%d, code1=%d\n", code0, code1); // Debug print

        // Check for trivial cases first
        if ((code0 | code1) == 0)
        { // accept when both points are inside the square
            accepted = true;
            running = false;
        }
        else if (code0 & code1)
        { // reject when both points are outside the square on the same side
            rejected = true;
            running = false;
        }
        else
        {
            running = true;
            // kick off first step immediately
            glutTimerFunc(STEP_DELAY_MS, clipStep, 0);
        }
        glutPostRedisplay();
    }
    else
    {
        printf("Resetting state\n"); // Debug print
        // any click resets
        setFirst = false;
        running = false;
        accepted = false;
        rejected = false;
        glutPostRedisplay();
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("Cohen‑Sutherland Line Clipping");
    glClearColor(0.f, 0.f, 0.f, 1.f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}