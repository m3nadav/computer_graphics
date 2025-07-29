#ifndef TAIL_H
#define TAIL_H

// Function to calculate tangent vector at the end of the tail curve
float calculateTailTangent(float controlPoints[5][3], float tangent[3]);

// Function to calculate rotation from tangent vector to align hair tuft
void calculateHairTuftRotation(float tangent[3], float &angle, float &axisX, float &axisY, float &axisZ);

// Function to draw the animated tail
void drawTail();

#endif // TAIL_H