#include "common/animation.h"

// Global animation counter
static int animationFrame = 0;

// Returns the current global animation frame counter for time-based animations.
int getAnimationFrame()
{
    return animationFrame;
}

// Advances the global animation frame counter to drive time-based scene animations.
void incrementAnimationFrame()
{
    animationFrame++;
}
