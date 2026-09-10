#pragma once

#include <vector>
#include <random>

#include "game/Player.hpp"
#include "game/Enemy.hpp"
#include "game/Projectile.hpp"
#include "core/Input.hpp"

enum class GameState {
    Running,
    GameOver
};

class Game {
public:
    Game();

    void update(const Input& input, float deltaTime);

    void reset();

    GameState state() const;

    int score() const;

    const Player& player() const;

    const std::vector<Enemy>& enemies() const;
    const std::vector<Projectile>& projectiles() const;

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
    void resolveProjectileEnemyCollisions();
    void removeInactiveObjects();
};
