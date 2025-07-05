# 3D Viewing Pipeline Demo: Train Animation

This project demonstrates the 3D viewing pipeline using a train animation in both 2D and 3D, with clear separation of each pipeline stage. The code is modular, with each file representing a specific stage or utility in the pipeline.

## Pipeline Stages & Code Structure

```
pipeline/
  models.h, models.cpp        # Basic shape drawing (2D/3D primitives)
  modeling_2d.cpp             # 2D train model (Modeling Coordinates)
  modeling_3d.cpp             # 3D train model (Modeling Coordinates)
  world_2d.cpp                # 2D world transform, animation, wheel rotation (World Coordinates)
  world_3d.cpp                # 3D world transform, animation, wheel rotation (World Coordinates)
  viewing_2d.cpp              # 2D view transform (Viewing Coordinates)
  viewing_3d.cpp              # 3D camera/view transform (Viewing Coordinates)
  projection_2d.cpp           # 2D orthographic projection (Projection Coordinates)
  projection_3d.cpp           # 3D perspective projection (Projection Coordinates)
  main.cpp                    # Main loop, event handling, pipeline orchestration
```

---

## File-by-File Breakdown

### `models.h` / `models.cpp`  
**Purpose:** Define and implement basic 2D/3D shape drawing functions (circle, ellipse, square, cube, sphere, ellipsoid, etc.).
- **Mathematics:** Geometry of primitives, parametric equations for circles/ellipses, vertex lists for cubes, etc.
- **Pipeline Stage:** Utility, used in Modeling Coordinates.

### `modeling_2d.cpp` / `modeling_3d.cpp`  
**Purpose:** Compose the train from basic shapes in local/model coordinates.
- **Mathematics:** All parts are defined relative to the train's origin (e.g., body at (0,0), wheels at offsets, chimney at front, etc.).
- **Pipeline Stage:** **Modeling Coordinates (MC)**
- **Action:** No world, view, or projection transforms applied yet. Just the local geometry of the train.

### `world_2d.cpp` / `world_3d.cpp`  
**Purpose:** Place the train in the world, animate its movement, and rotate the wheels.
- **Mathematics:**
  - **Translation:** Moves the train from left to right: `glTranslatef(x, y, z)`
  - **Wheel Rotation:** Angle = distance traveled / wheel radius (arc length formula)
  - **Animation:** Position and wheel angle are functions of time or frame
- **Pipeline Stage:** **World Coordinates (WC)**
- **Action:** Apply world transforms (translation, rotation for wheels), animate over time, or step through frames if paused.

### `viewing_2d.cpp` / `viewing_3d.cpp`  
**Purpose:** Set up the camera/view transformation.
- **Mathematics:**
  - **2D:** Identity (no camera movement)
  - **3D:** `gluLookAt(eye, center, up)` sets the camera position and orientation
- **Pipeline Stage:** **Viewing Coordinates (VC)**
- **Action:** Transform world coordinates to camera/view coordinates.

### `projection_2d.cpp` / `projection_3d.cpp`  
**Purpose:** Set up the projection transformation.
- **Mathematics:**
  - **2D:** `gluOrtho2D(left, right, bottom, top)` for orthographic projection
  - **3D:** `gluPerspective(fovy, aspect, near, far)` for perspective projection
- **Pipeline Stage:** **Projection Coordinates (PC)**
- **Action:** Project view coordinates to normalized device coordinates.

### `main.cpp`  
**Purpose:** Orchestrate the pipeline, handle windowing, input, and rendering loop.
- **Actions:**
  - Calls each pipeline stage in order: Projection → Viewing → World → Modeling
  - Handles user input for toggling 2D/3D, pausing, stepping, etc.
  - Draws on-screen status overlays
- **Pipeline Stages:** All (coordinates are transformed through the full pipeline here)

---

## Pipeline Mathematics & Flow

1. **Modeling Coordinates (MC):**
   - Each train part is defined in its own local space.
   - Example: A wheel at (0.35, -0.22) relative to the train's origin.
2. **World Coordinates (WC):**
   - The whole train is translated to its world position and animated.
   - Wheels are rotated according to distance traveled.
3. **Viewing Coordinates (VC):**
   - The scene is transformed by the camera/view matrix (identity in 2D, `gluLookAt` in 3D).
4. **Projection Coordinates (PC):**
   - The scene is projected to 2D screen space (orthographic for 2D, perspective for 3D).
5. **Normalized/Device Coordinates (NC/DC):**
   - Handled by OpenGL: coordinates are mapped to the viewport and pixels.

---

## Controls
- `SPACE`: Toggle between 2D and 3D scenes
- `p`: Pause/play animation
- Left/Right arrows: Step frame-by-frame when paused
- `ESC`: Quit

---

## Summary
- Each file represents a clear stage in the 3D viewing pipeline.
- The code is modular and easy to follow for educational or demonstration purposes.
- The mathematics and transformations for each stage are commented and separated for clarity. 