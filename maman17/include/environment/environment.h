#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// Tree model functions
void drawTree(float x, float y, float z, float scale = 1.0f);
void drawTrunk(float height, float baseRadius, float topRadius);
void drawBranch(float length, float radius, int depth, float angleX, float angleY);
void drawLeaves(float x, float y, float z, float size);

// Meadow functions
void drawProceduralMeadow(float width, float depth, int grassDensity = 1000);
void drawGrassBlade(float height, float width, float bend, float colorVariation);

// Rock model functions
void drawRock(float x, float y, float z, float scale = 1.0f, float colorVariation = 0.0f);
void drawIrregularRock(float scale, int complexity = 20, float colorVariation = 0.0f);
void drawScatteredRocks(float x, float y, float z, int numRocks);

// Metal bench and lamp post functions
void drawMetalBench(float x, float y, float z, float scale = 1.0f, float rotateY = 0.0f);
void drawMetalBenchAndLamp(float x, float y, float z, float scale = 1.0f, float rotateY = 0.0f);

// Lighting visual elements
void drawSun();
void drawLampPost(float benchX, float benchY, float benchZ, float scale = 1.0f, float benchRotation = 0.0f);

// Sun position management functions
void setSunPosition(int positionIndex);
int getCurrentSunPosition();

// Random seed management for consistent generation
void setSeedForObject(float x, float y, float z, int objectType);

// Simple collision detection utilities to avoid drawing rocks too close to the cow
bool isPositionTooCloseToPoint(float x, float z, float targetX, float targetZ, float minDistance);
bool isPositionSafeForRock(float rockX, float rockZ);

// Cow dimension calculations for collision detection
float getCowCollisionRadius();
float getCowSpawnRadius();
float getCowClearanceRadius();

// Ground texture management
unsigned int loadGroundTexture(const char *filename);
void drawTexturedGroundPatch(float centerX, float centerZ, float size, unsigned int textureID);
void drawWorldGround(float worldSize);

#endif