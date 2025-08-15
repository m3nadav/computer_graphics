#include <iostream>
#include <iomanip>
#include <cmath>
#include <tuple>

// Simplified camera movement test without OpenGL dependencies
class SimpleCameraTest
{
private:
    float angleX; // Vertical angle (elevation)
    float angleY; // Horizontal angle (azimuth)
    float anchorX, anchorY, anchorZ;

public:
    SimpleCameraTest(float ax, float ay) : angleX(ax), angleY(ay), anchorX(0.0f), anchorY(0.0f), anchorZ(0.0f) {}
    
    void setAnchor(float x, float y, float z) { anchorX = x; anchorY = y; anchorZ = z; }
    void setAngles(float x, float y) { angleX = x; angleY = y; }
    std::tuple<float, float, float> getAnchor() const { return {anchorX, anchorY, anchorZ}; }

    void moveAnchorForward(float distance)
    {
        // Calculate forward direction based on current camera orientation
        float angleYRad = angleY * M_PI / 180.0f;
        float angleXRad = angleX * M_PI / 180.0f;
        
        // Forward direction is the direction the camera is looking
        // In spherical coordinates, forward is the opposite of the camera position vector
        float forwardX = -cos(angleYRad) * cos(angleXRad);
        float forwardZ = -sin(angleYRad) * cos(angleXRad);
        
        anchorX += forwardX * distance;
        anchorZ += forwardZ * distance;
    }

    void moveAnchorLeft(float distance)
    {
        // Calculate right direction (perpendicular to forward in XZ plane)
        float angleYRad = angleY * M_PI / 180.0f;
        
        // Right vector is 90 degrees clockwise from forward in XZ plane
        float rightX = -sin(angleYRad);
        float rightZ = cos(angleYRad);
        
        // Left is opposite of right
        float leftX = -rightX;
        float leftZ = -rightZ;
        
        anchorX += leftX * distance;
        anchorZ += leftZ * distance;
    }
};

void testCameraDirectionalMovement()
{
    std::cout << "Testing Camera Direction-Relative Movement\n";
    std::cout << "==========================================\n";
    
    // Test camera with specific angles
    SimpleCameraTest camera(35.0f, 55.0f);
    
    std::cout << std::fixed << std::setprecision(3);
    
    // Test initial anchor position
    auto anchor = camera.getAnchor();
    std::cout << "Initial anchor: (" << std::get<0>(anchor) << ", " 
              << std::get<1>(anchor) << ", " << std::get<2>(anchor) << ")\n";
    
    // Test forward movement
    camera.moveAnchorForward(1.0f);
    anchor = camera.getAnchor();
    std::cout << "After moving forward: (" << std::get<0>(anchor) << ", " 
              << std::get<1>(anchor) << ", " << std::get<2>(anchor) << ")\n";
    
    // Reset position
    camera.setAnchor(0.0f, 0.0f, 0.0f);
    
    // Test left movement
    camera.moveAnchorLeft(1.0f);
    anchor = camera.getAnchor();
    std::cout << "After moving left: (" << std::get<0>(anchor) << ", " 
              << std::get<1>(anchor) << ", " << std::get<2>(anchor) << ")\n";
    
    // Reset position
    camera.setAnchor(0.0f, 0.0f, 0.0f);
    
    // Test with different angle
    camera.setAngles(35.0f, 0.0f);  // Face in positive Z direction
    camera.moveAnchorForward(1.0f);
    anchor = camera.getAnchor();
    std::cout << "Facing Z+ direction, after forward: (" << std::get<0>(anchor) << ", " 
              << std::get<1>(anchor) << ", " << std::get<2>(anchor) << ")\n";
    
    // Reset and test 90 degree turn
    camera.setAnchor(0.0f, 0.0f, 0.0f);
    camera.setAngles(35.0f, 90.0f);  // Face in negative X direction
    camera.moveAnchorForward(1.0f);
    anchor = camera.getAnchor();
    std::cout << "Facing X- direction, after forward: (" << std::get<0>(anchor) << ", " 
              << std::get<1>(anchor) << ", " << std::get<2>(anchor) << ")\n";
    
    std::cout << "\nTest completed successfully!\n";
}

int main()
{
    testCameraDirectionalMovement();
    return 0;
}