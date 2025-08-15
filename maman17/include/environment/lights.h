#ifndef LIGHTS_H
#define LIGHTS_H

// Lighting setup
void setupEnvironmentLighting();
void setupSunLighting();

// Lamp post lighting system
void setupLampLighting();

// Light control functions
void setLightIntensity(float intensity);
void setLightPosition(float x, float y, float z);
void setAmbientLevel(float level);
float getLightIntensity();
float getLightPositionX();
float getLightPositionY();
float getLightPositionZ();
float getAmbientLevel();
void enableCustomLightPosition(bool enable);

// Lamp light control functions
void setLampIntensity(float intensity);
void setLampDirection(float angleX, float angleZ);
void getLampDirection(float *angleX, float *angleZ);
float getLampIntensity();
void enableLampLight(bool enable);

// Reset functions
void resetLightingToDefaults();


// Accessor functions for variables needed by moved functions
float *getSunPositions();
bool getUseCustomPosition();
float getLampDirectionX();
float getLampDirectionZ();
float getLampHeight();
float getBackrestOffset();
void updateCurrentBenchParameters(float x, float y, float z, float rotation);

// Material functions
void setMaterialFromColor(float r, float g, float b, float shininess = 20.0f, float specularLevel = 0.2f);

// Environment-specific materials
void setTreeMaterial();
void setGrassMaterial(float colorVariation = 0.0f);
void setRockMaterial(float colorVariation = 0.0f);
void setMetalMaterial();

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

#endif // LIGHTS_H