#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// Tree model functions
void drawTree(float x, float y, float z, float scale = 1.0f);
void drawTrunk(float height, float baseRadius, float topRadius);
void drawBranch(float length, float radius, int depth, float angleX, float angleY);
void drawLeaves(float x, float y, float z, float size);

// Enhanced meadow functions
void drawProceduralMeadow(float width, float depth, int grassDensity = 1000);
void drawGrassBlade(float height, float width, float bend, float colorVariation);

// Rock model functions  
void drawRock(float x, float y, float z, float scale = 1.0f);
void drawIrregularRock(float scale, int complexity = 20);

// Lighting and material setup
void setupEnvironmentLighting();
void setTreeMaterial();
void setGrassMaterial(float colorVariation = 0.0f);
void setRockMaterial();

#endif // ENVIRONMENT_H