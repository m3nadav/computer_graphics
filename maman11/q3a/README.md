# OpenGL Window Position Demo

This program creates an OpenGL window with the following specifications:
- Bottom-right corner at coordinates (230, 210)
- Width: 120 pixels
- Height: 80 pixels

The implementation uses GLUT (OpenGL Utility Toolkit) for cross-platform window management.

## Prerequisites

- OpenGL development libraries
- GLUT library

### Installation

```bash
# macOS
brew install glut
# Note: On macOS, GLUT is included with the OS in the framework form

# Ubuntu/Debian
sudo apt-get install freeglut3-dev

# Windows
# Install freeglut via MSYS2 or MinGW
```

### Platform-Specific Notes

- **macOS**: GLUT headers are located at `<GLUT/glut.h>` (not `<GL/glut.h>`)
- **Linux/Windows**: GLUT headers are located at `<GL/glut.h>`

The code handles these differences with preprocessor directives.

## Building

```bash
make
```

## Running

```bash
./window_position
```

## Code Explanation

The program performs the following steps:
1. Initializes GLUT with `glutInit()`
2. Sets the display mode to single buffer with RGB color model using `glutInitDisplayMode()`
3. Sets the window size to 120x80 pixels with `glutInitWindowSize()`
4. Positions the window so that its bottom-right corner is at (230, 210) using `glutInitWindowPosition()`
5. Creates the window with `glutCreateWindow()`
6. Sets the display callback function with `glutDisplayFunc()`
7. Enters the GLUT main loop with `glutMainLoop()`

The display function simply clears the screen and flushes the buffer. 