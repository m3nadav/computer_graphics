### Code structure

I decided to split the code into multiple modules:

- main.cpp - contains the OpenGL + GLUT initialization, reshape/mouse hooks, and screen settings
- utils.cpp - contains the general shape drawing utilities
- scene.cpp - the actual scene drawing with shapes of specific sizes and per-object-drawing functions

### Compilation

I compiled using the command:

`g++ *.cpp -o main -framework OpenGL -framework GLUT`

Then executed the compiled file using `./main`