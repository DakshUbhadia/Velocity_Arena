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


---

## AABB Representation

An Axis-Aligned Bounding Box (AABB) is defined by two corner points:

```
min = center - halfExtents
max = center + halfExtents
```

For a unit cube centered at (cx, cy, cz) with half-extents (0.5, 0.5, 0.5):
```
min = (cx - 0.5, cy - 0.5, cz - 0.5)
max = (cx + 0.5, cy + 0.5, cz + 0.5)
```

The box sides are always parallel to the world axes (X, Y, Z), which is what
"axis-aligned" means. This constraint makes intersection testing extremely cheap.

### Why Half Extents?

Half extents (radius) are preferred over full width/height/depth because
the conversion to min/max is a single add/subtract:
```
min = center - halfExtents
max = center + halfExtents
```

---

## Interval Overlap

An AABB on the X axis is an interval `[min.x, max.x]`.

Two 1D intervals `[a_min, a_max]` and `[b_min, b_max]` overlap if and only if:
```
a_max >= b_min  AND  a_min <= b_max
```

Equivalently, they are SEPARATED (do not overlap) if:
```
a_max < b_min  OR  a_min > b_max
```

This gives us an early-out implementation:
```cpp
if (max_.x < other.min_.x || min_.x > other.max_.x) return false;  // X gap
if (max_.y < other.min_.y || min_.y > other.max_.y) return false;  // Y gap
if (max_.z < other.min_.z || min_.z > other.max_.z) return false;  // Z gap
return true;
```

---

## Why All Three Axes Must Overlap

Two 3D boxes collide if and only if their projections overlap on ALL three axes
simultaneously. This follows from the Separating Axis Theorem (SAT):

> If there exists any axis along which the projections of two convex shapes
> do NOT overlap, the shapes are separated.

For AABBs, the candidate separating axes are just the three world axes (X, Y, Z)
because the boxes are axis-aligned. We check all three. If any axis shows a gap,
we immediately return "no collision."

---

## AABB Complexity

A single AABB intersection test requires at most 6 comparisons (2 per axis).

**A single AABB-vs-AABB test is O(1).**

However, testing all N*(N-1)/2 unique pairs in a brute-force search is **O(N^2)**.
Do not confuse the cost of one test with the cost of the loop that calls it.

---

## Naive All-Pairs Pair Count

For N objects, the number of unique UNORDERED pairs (i, j) where i != j is:

```
N * (N - 1) / 2
```

Derivation:
- There are N choices for i and (N-1) choices for j (excluding i itself).
- N * (N-1) counts ORDERED pairs (A,B) and (B,A) separately.
- Divide by 2 to count each pair once: N * (N-1) / 2.

Verification:
```
N = 100:   100 * 99 / 2  =  4,950
N = 500:   500 * 499 / 2 = 124,750
N = 1000: 1000 * 999 / 2 = 499,500
N = 2500: 2500 * 2499 / 2 = 3,123,750
N = 5000: 5000 * 4999 / 2 = 12,497,500
```


# Spatial Grid Mathematics

## World to Cell Mapping

    cellX =
        floor(worldX / cellSize)

    cellZ =
        floor(worldZ / cellSize)

Applying floor is required to handle correct offset for negative world boundaries. A pure cast truncates to zero (e.g. (int)-0.5 = 0), missing appropriate partition bounding and resulting in grid inversion gaps.

## AABB Cell Range

    minCell =
        floor(AABB.min / cellSize)

    maxCell =
        floor(AABB.max / cellSize)

## Candidate Reduction

Spatial partitioning relies on the physical impossibility of objects located at distinct independent sectors of space to overlap. Rather than checking an object across the full volume against all other distant actors, it guarantees intersection validity simply against the narrow proximate pool enclosed inside their common sector(s).

## Pair Count Baseline

Retained format:

    N(N-1)/2

## Candidate Reduction Formula

    reduction =
        1 - (optimizedCandidates / baselineCandidates)
