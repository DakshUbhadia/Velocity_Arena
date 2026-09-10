# Mathematics in Velocity Arena

## Coordinate System
- Right-handed coordinate system.
- X is Right, Y is Up, -Z is Forward.

## Matrix Transformations
- `modelMatrix`: Translates, rotates, and scales objects from local space to world space.
- `viewMatrix`: Moves the world relative to the camera. Computed via `glm::lookAt(position, target, up)`.
- `projectionMatrix`: Applies perspective foreshortening.

## Vector Math and Movement

### Movement Equation
To make movement frame-rate independent, the position updates follow the fundamental equation:
`distance = speed × time`
`position_ += direction × movementSpeed_ × deltaTime;`

### Vector Normalization
When moving diagonally (e.g., W and D simultaneously), the resulting raw movement vector is `(1, 0, 1)`. 
Its magnitude is calculated using the Pythagorean theorem: `sqrt(1^2 + 1^2) = sqrt(2) ≈ 1.414`.
This means diagonal movement is 41% faster than axis-aligned movement.

To fix this, we normalize the vector if its length is greater than 0:
```cpp
if (glm::length(movement) > 0.0f) {
    movement = glm::normalize(movement);
}
```
Normalization scales the vector so its length is exactly `1.0`, keeping movement speed uniform in all directions.
