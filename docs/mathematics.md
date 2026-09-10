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

### Facing Vector
The player remembers its normalized movement direction to have a stable firing direction even when stationary.
`facingDirection = movement;`

### Chase Direction
Enemies pursue the player using:
`enemyDirection = playerPosition - enemyPosition;`

### Chase Speed Normalization
The chase speed must not depend on the distance to the player. Normalizing the `enemyDirection` ensures the enemy moves at a constant speed toward the player.

### Projectile Equation
A projectile follows a simple linear motion equation:
`position = position + direction * speed * deltaTime;`

### Squared Distance and Collision Detection
For distance checking, the naive distance formula uses `sqrt()`:
`distance = sqrt(dx^2 + dz^2)`

Because `sqrt()` is computationally expensive, we instead compare the **squared distance** against the **squared threshold**:
`distanceSquared = dx*dx + dz*dz`

If we want to check `distance <= radius`, we can mathematically check `distanceSquared <= radius * radius`. This avoids computing the square root entirely.

### Spawn Position Offset
To avoid the projectile spawning inside the player, we offset the position by adding a portion of the facing direction:
`projectilePosition = playerPosition + facingDirection * offset;`
