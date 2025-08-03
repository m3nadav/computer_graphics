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

// Metal bench functions
void drawMetalBench(float x, float y, float z, float scale = 1.0f);
void setMetalMaterial();

// Lighting and material setup
void setupEnvironmentLighting();
void setTreeMaterial();
void setGrassMaterial(float colorVariation = 0.0f);
void setRockMaterial();
void setMaterialFromColor(float r, float g, float b, float shininess = 20.0f, float specularLevel = 0.2f);

// Cow-specific materials
void setCowWhiteMaterial();
void setCowLightGrayMaterial();
void setCowHoofMaterial();
void setCowHoofDetailMaterial();
void setCowPinkMaterial();
void setCowBlackMaterial();
void setCowBeigeMaterial();
void setCowDarkBrownMaterial();
void setCowHornMaterial();

// Drawing utility materials
void setGridMaterial();
void setRedAxisMaterial();
void setGreenAxisMaterial();
void setBlueAxisMaterial();
void setBlackTextMaterial();

// Random seed management for consistent generation
void setSeedForObject(float x, float y, float z, int objectType);

// Ground texture management
unsigned int loadGroundTexture(const char *filename);
void drawTexturedGroundPatch(float centerX, float centerZ, float size, unsigned int textureID);
void drawWorldGround(float worldSize);

#endif // ENVIRONMENT_H