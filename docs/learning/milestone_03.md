# Milestone 3 — Core Gameplay Loop

## What We Built
We turned the movement prototype into an actual playable game loop. We added a player with health, projectile shooting, enemy spawning with a finite-state machine (Idle, Chase, Attack), distance-based collision, score tracking, and game over/restart logic.

## Files Added
- `src/game/Enemy.hpp` & `src/game/Enemy.cpp`
- `src/game/Projectile.hpp` & `src/game/Projectile.cpp`
- `src/game/Game.hpp` & `src/game/Game.cpp`
- `docs/learning/milestone_03.md`

## Files Modified
- `src/game/Player.hpp` & `src/game/Player.cpp`
- `src/main.cpp`
- `CMakeLists.txt`
- `docs/architecture.md` & `docs/mathematics.md`
- `PROGRESS.md` & `README.md`

## Complete Gameplay Update Flow
1. Process Input
2. Update Game (Player, Fire Cooldown, Spawn Timer, Projectiles, Enemies, Collisions, Inactive Removal, Death Check)
3. Update Camera
4. Render Frame
5. Swap Buffers

## Game Ownership Model
`Game` owns the `Player`, a vector of `Enemy`, and a vector of `Projectile`. It handles the game state orchestration. `main.cpp` owns the window, graphics resources, and inputs.

## Player Facing Direction
We added a `facingDirection` vector to the `Player` that updates whenever the player moves. It does NOT reset to zero when the player stops.

## Projectile Motion
Linear motion update: `position += direction * speed * deltaTime`.

## Projectile Lifetime
Projectiles have a set lifetime (2.0s) and deactivate when it reaches zero, preventing infinite active projectiles.

## Enemy Finite-State Machine
Enemies exist in one of three states based on distance to the player.

## Idle State
Occurs when the player is out of detection radius (e.g. > 6.0). Enemy does nothing.

## Chase State
Occurs when player is within detection radius but out of attack range. Enemy moves towards the player on the XZ plane.

## Attack State
Occurs when the player is within attack range (e.g. <= 0.8). Enemy damages the player based on a cooldown timer.

## Enemy Attack Cooldown
We use a timer (`attackCooldown_`) to ensure attacks happen at intervals (e.g. 0.75s) rather than every single frame.

## Enemy Spawning
Enemies are spawned randomly on the perimeter of the 10x10 floor using standard `<random>` generators.

## Random Number Generation
`std::mt19937` is seeded ONCE in the `Game` constructor using `std::random_device`, providing random sequences for enemy spawning.

## Temporary Distance-Based Collision
We check collisions using the squared Euclidean distance between two entities vs the sum of their radii squared.

## Squared-Distance Optimization
Avoids `sqrt()` computation by comparing `distance^2 <= radius^2`.

## Safe Vector Removal
We don't remove elements directly in a `for` loop, as it invalidates iterators. Instead, we mark entities as dead/inactive.

## std::erase_if
A C++20 standard algorithm used at the end of the frame to remove all inactive projectiles and dead enemies safely in one pass.

## Game State
Tracked using a simple `enum class GameState { Running, GameOver }`.

## Game Over and Restart
When player health drops to 0, state shifts to `GameOver`. Pressing `R` calls `reset()`, clearing vectors and restoring player.

## Resource and Object Lifetimes
OpenGL resources are safely constructed within a try/catch block and inner scope so they die before `glfwTerminate()`.

## Important C++ Concepts
- `std::erase_if`
- Object ownership & `std::vector`
- Exception safety
- Random Number Generators

## Important Mathematics
- Vector Normalization
- Pythagorean distance (squared)
- Coordinate offset for spawning

## Design Decisions
- `Game` class does not know about OpenGL colors.
- Reuse `cubeMesh` for Player, Enemy, and Projectile.

## Alternatives We Did Not Use
- Object Pooling (kept it simple with vectors and `erase_if` for now).
- AABB Collision (using simpler squared-distance for MS3).

## Common Bugs
- Normalizing a zero vector.
- Vector iteration invalidation on erase.
- Damage applied per frame instead of on cooldown.
- Game continuing updates during GameOver.

## Interview Questions
1. **Why introduce a Game class?** To prevent `main.cpp` from becoming a monolithic God class owning all gameplay state.
2. **What objects does Game own?** `Player`, `std::vector<Enemy>`, `std::vector<Projectile>`.
3. **Why doesn't Game own Renderer?** Rendering logic requires graphics context and should be separate from pure game simulation state.
4. **Why doesn't Player render itself?** Keep game logic separate from graphics API.
5. **How does the player remember its facing direction?** It stores the last non-zero normalized movement vector.
6. **Why keep facing direction when movement becomes zero?** So the player can still shoot projectiles in the direction they were last looking while stationary.
7. **How is projectile movement calculated?** `pos += direction * speed * dt`.
8. **Why does projectile movement use delta time?** To ensure the speed is consistent regardless of framerate.
9. **Why do projectiles have finite lifetime?** To prevent them from traveling infinitely and wasting memory/CPU processing.
10. **Why haven't you implemented object pooling yet?** A simple baseline is needed first; optimizations like pooling are for later milestones.
11. **What is a finite-state machine?** A mathematical model of computation where a system can be in exactly one of a finite number of states.
12. **What states does Enemy have?** Idle, Chase, Attack.
13. **What causes Idle -> Chase?** Player entering detection radius.
14. **What causes Chase -> Attack?** Player entering attack range.
15. **What causes Attack -> Chase?** Player leaving attack range.
16. **Why normalize the chase direction?** So the enemy moves at a constant `movementSpeed` instead of faster when further away.
17. **Why is an attack cooldown needed?** To avoid applying damage every single frame (e.g. 60+ times per second).
18. **How are enemies spawned?** Randomly along the 4 edges of the arena boundary.
19. **Why is the random generator seeded once?** Seeding multiple times (e.g. per frame) can produce identical numbers if done too quickly or reset the distribution sequence.
20. **Why avoid spawning directly on the player?** It would instantly damage the player unfairly.
21. **How does projectile/enemy collision currently work?** Point-distance based threshold (spherical/circular collision).
22. **Why compare squared distances?** To avoid the expensive `sqrt()` operation.
23. **Why aren't we using AABB yet?** To keep this milestone simple and establish a baseline before the Milestone 4 collision optimization.
24. **What is the complexity of projectile-vs-enemy collision?** O(P * E), where P is projectiles and E is enemies.
25. **Why don't you erase vector elements inside the nested loop?** It invalidates iterators and shifts elements, causing crashes or skipped checks.
26. **What does std::erase_if do?** Removes all elements in a container that satisfy a given predicate safely.
27. **How is score prevented from incrementing twice?** The projectile is deactivated and enemy is killed immediately upon the first hit detection, and the inner loop breaks.
28. **What happens when the player reaches zero health?** `GameState` becomes `GameOver`, gameplay updates stop.
29. **What exactly does Game::reset() reset?** Player position/health/direction, clears entities, resets score/timers, and restores state to `Running`.
30. **What would need to change to support thousands of entities?** Spatial partitioning (Grid/Quadtree), Object Pooling, potentially Data-Oriented Design (ECS).
31. **What systems will be optimized in later milestones?** Collision detection (AABB + Grid) and Object Lifetimes (Pooling).
32. **Why is this current implementation useful as a baseline?** It gives us an understandable, working game loop to benchmark against and prove that optimizations actually help.

## Things I Should Be Able To Explain Without Looking At Code
- Exception safety of OpenGL initialization
- Vector normalization & length
- Simple FSM state transitions
- Game loop timing variables
- Clean ownership hierarchies
