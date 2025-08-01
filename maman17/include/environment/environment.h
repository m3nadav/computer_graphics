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

// Random seed management for consistent generation
void setSeedForObject(float x, float y, float z, int objectType);

// Ground texture management
unsigned int loadGroundTexture(const char* filename);
void drawTexturedGroundPatch(float centerX, float centerZ, float size, unsigned int textureID);
void drawWorldGround(float worldSize);

#endif // ENVIRONMENT_H