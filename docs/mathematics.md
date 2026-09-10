# Coordinate Spaces

The 3D rendering pipeline transforms object vertices through several coordinate spaces before they appear on the screen:

```text
Local / Model Space
        ↓
      Model
        ↓
World Space
        ↓
       View
        ↓
Camera / View Space
        ↓
    Projection
        ↓
Clip Space
        ↓
Perspective Division
        ↓
Normalized Device Coordinates
        ↓
Viewport
        ↓
Screen
```

## Model Matrix

The model matrix is used to transform a vertex from its **local object coordinates** to **world coordinates**. It defines where the object is located in the global 3D world, as well as its orientation and size. In this implementation, we apply `translation`, `rotation`, and `scale` transformations using `glm::translate`, `glm::rotate`, and `glm::scale` on independent model matrices for the floor and each cube.

## View Matrix

The view matrix represents the transformation of the world relative to the camera. Instead of moving the camera, OpenGL conceptually moves the entire world in the opposite direction. 
This project constructs the view matrix using:
```cpp
glm::lookAt(cameraPosition, targetPosition, upVector)
```
- **eye**: Where the camera is located in world space.
- **center/target**: What the camera is looking at.
- **up**: The vector pointing upwards in the world, defining the camera's tilt.

## Projection Matrix

The projection matrix maps the 3D coordinates to 2D screen coordinates while simulating perspective (objects further away appear smaller).
This project uses:
```cpp
glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane)
```
- **field of view**: The vertical viewing angle.
- **aspect ratio**: The ratio of the viewport's width to its height.
- **near clipping plane**: The closest distance at which objects are rendered.
- **far clipping plane**: The furthest distance at which objects are rendered.
- **perspective**: Applies the perspective divide (w-component) to give depth perception.

## Matrix Multiplication Order

In our vertex shader (`basic.vert`), we calculate the vertex position as:
```text
Projection × View × Model × Position
```
Because vector-matrix multiplication is associative and performed right-to-left in GLSL, the `Model` transformation applies to the `Position` first, putting it into world space. Next, the `View` matrix puts it into camera space, and finally, the `Projection` matrix moves it into clip space.

## Homogeneous Coordinate

A 3D position is represented as a `vec4` with `w = 1.0`:
```glsl
vec4(aPosition, 1.0)
```
Using `w = 1` allows the vertex to undergo translation when multiplied by a 4x4 matrix. If `w = 0`, the position acts as a direction vector and ignores translation. Furthermore, the `w` component stores the depth value after the perspective projection for perspective division.

## Depth Buffer

- **What it is**: The depth buffer (or Z-buffer) stores the depth value of every pixel on the screen. 
- **Why GL_DEPTH_TEST is enabled**: When `GL_DEPTH_TEST` is enabled, OpenGL compares the depth of a new pixel being drawn against the existing depth in the buffer. If the new pixel is behind the existing one, it is discarded, preventing further objects from rendering on top of closer ones.
- **Why GL_DEPTH_BUFFER_BIT must be cleared**: We clear it every frame using `glClear` to reset the depth values to the maximum depth, allowing the new frame to properly perform depth testing from a clean slate.
