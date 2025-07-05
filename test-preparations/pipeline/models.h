#pragma once

// 2D shapes
typedef float vec2[2];
void drawCircle(float cx, float cy, float r, int num_segments = 32);
void drawEllipse(float cx, float cy, float rx, float ry, int num_segments = 32);
void drawSquare(float cx, float cy, float size);

// 3D shapes
typedef float vec3[3];
void drawCube(float cx, float cy, float cz, float size);
void drawSphere(float cx, float cy, float cz, float r, int slices = 16, int stacks = 16);
void drawEllipsoid(float cx, float cy, float cz, float rx, float ry, float rz, int slices = 16, int stacks = 16);