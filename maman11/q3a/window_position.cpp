#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void display() {
    glClear(GL_COLOR_BUFFER_BIT); // מנקה את המסך
    glFlush(); // מרענן את התצוגה
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    // קביעת גודל החלון
    glutInitWindowSize(120, 80);
    
    // קביעת מיקום ראשית הצירים בהנתן הפינה הימנית התחתונה של החלון
    glutInitWindowPosition(230 - 120, 210 - 80);
    
    // יצירת החלון
    glutCreateWindow("OpenGL Window");
    
    // הגדרת פונקציית הציור
    glutDisplayFunc(display);
    
    glutMainLoop();
    return 0;
} 