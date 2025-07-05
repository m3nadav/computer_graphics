#include <GLUT/glut.h>
#include <algorithm>

struct Point
{
    float x, y;
};

struct Color
{
    float r, g, b;
};

void drawLine(Point p1, Point p2, Color color)
{
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();
}

int orientation(Point p, Point q, Point r)
{
    // 0 -> collinear, 1 -> clockwise, -1 -> counterclockwise
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0)
        return 0;
    return (val > 0) ? 1 : -1;
}

bool onSegment(Point p, Point r, Point q)
{
    // check if r lies on segment pq
    return (r.x <= std::max(p.x, q.x) && r.x >= std::min(p.x, q.x) && r.y <= std::max(p.y, q.y) && r.y >= std::min(p.y, q.y));
}

Point findIntersection(Point p, Point q, Point r, Point s)
{
    // Line p-q represented as A1*x + B1*y = C1
    float A1 = q.y - p.y;
    float B1 = p.x - q.x;
    float C1 = A1 * p.x + B1 * p.y;

    // Line r-s represented as A2*x + B2*y = C2
    float A2 = s.y - r.y;
    float B2 = r.x - s.x;
    float C2 = A2 * r.x + B2 * r.y;

    float det = A1 * B2 - A2 * B1;
    if (det == 0)
        return {0, 0};

    float x = (C1 * B2 - C2 * B1) / det;
    float y = (A1 * C2 - A2 * C1) / det;

    return {x, y};
}

bool checkIntersecting(Point p, Point q, Point r, Point s)
{
    // check if pq and rs intersect
    int o1 = orientation(p, q, r);
    int o2 = orientation(p, q, s);
    int o3 = orientation(r, s, p);
    int o4 = orientation(r, s, q);

    // general case
    if (o1 != o2 && o3 != o4)
        return true;

    // collinearity check
    if (o1 == 0 && onSegment(p, r, q))
        return true;
    if (o2 == 0 && onSegment(p, s, q))
        return true;
    if (o3 == 0 && onSegment(r, p, s))
        return true;
    if (o4 == 0 && onSegment(r, q, s))
        return true;

    return false;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Point p = {100, 300};
    Point q = {300, 100};
    Point r = {50, 50};
    Point s = {250, 370};
    drawLine(p, q, {1.0, 0.0, 0.0});
    drawLine(r, s, {0.0, 1.0, 0.0});
    if (checkIntersecting(p, q, r, s))
    {
        Point intersection = findIntersection(p, q, r, s);
        glColor3f(0.0, 0.0, 1.0);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glVertex2f(intersection.x, intersection.y);
        glEnd();
    }

    glutSwapBuffers();
}

void init()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 400.0, 0.0, 400.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // double buffering for smooth swapping
    glutInitWindowSize(400, 400);
    glutCreateWindow("Intersection of two lines");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}