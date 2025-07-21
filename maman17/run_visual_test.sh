#!/bin/bash
echo "Compiling and running visual test..."
g++ -std=c++11 -I./include -framework OpenGL -framework GLUT src/visual_test.cpp src/cow/spots.cpp src/cow/body.cpp src/cow/cow.cpp src/shapes/shapes.cpp -o visual_test_working
./visual_test_working 