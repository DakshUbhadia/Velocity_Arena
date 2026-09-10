#pragma once

#include <vector>
#include <random>

#include "game/Player.hpp"
#include "game/Enemy.hpp"
#include "game/Projectile.hpp"
#include "core/Input.hpp"
#include "core/Profiler.hpp"
#include "core/RuntimeOptions.hpp"
#include "physics/SpatialGrid.hpp"

struct GameplayCollisionStats {
    std::uint64_t candidateChecks{0};
    std::uint64_t intersections{0};
    std::uint64_t gridCellInsertions{0};
    std::size_t occupiedCells{0};
};

enum class GameState {
    Running,
    GameOver
};

class Game {
public:
    Game(const RuntimeOptions& options = RuntimeOptions{});

    void update(const Input& input, float deltaTime, Profiler& profiler);

    void reset();

    GameState state() const;

    int score() const;

    const Player& player() const;

    const std::vector<Enemy>& enemies() const;
    const std::vector<Projectile>& projectiles() const;

    const GameplayCollisionStats& collisionStats() const { return collisionStats_; }

private:
    Player player_;

    std::vector<Enemy> enemies_;
    std::vector<Projectile> projectiles_;

    GameState state_{GameState::Running};

    int score_{0};

    float spawnTimer_{0.0f};
    float spawnInterval_{2.0f};
    int maxEnemies_{12};
    
    float fireCooldown_{0.0f};
    float fireInterval_{0.20f};

    std::mt19937 rng_;

    void spawnEnemy();
    void spawnProjectile();
    void resolveProjectileEnemyCollisionsBruteForce();
    void resolveProjectileEnemyCollisionsSpatialGrid();
    void removeInactiveObjects();

    RuntimeOptions options_;
    GameplayCollisionStats collisionStats_;
    SpatialGrid enemyGrid_{2.0f};
    std::vector<std::size_t> collisionCandidatesScratch_;
};
