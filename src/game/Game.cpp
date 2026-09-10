#include "Game.hpp"

#include <GLFW/glfw3.h>

Game::Game(const RuntimeOptions& options)
    : player_(glm::vec3(0.0f, 0.5f, 0.0f), 5.0f), options_(options) {
    if (options_.stressCount > 0) {
        rng_.seed(options_.seed);
        std::size_t projCount = std::min(options_.stressCount / 5, static_cast<std::size_t>(250));
        std::size_t enemyCount = options_.stressCount - projCount;
        
        float worldSide = std::sqrt(static_cast<float>(options_.stressCount)) * 2.5f;
        std::uniform_real_distribution<float> posDist(-worldSide / 2.0f, worldSide / 2.0f);
        
        for (std::size_t i = 0; i < enemyCount; ++i) {
            enemies_.emplace_back(glm::vec3(posDist(rng_), 0.5f, posDist(rng_)), 1.5f);
        }
        for (std::size_t i = 0; i < projCount; ++i) {
            glm::vec3 dir(1.0f, 0.0f, 0.0f); // Arbitrary direction
            projectiles_.emplace_back(glm::vec3(posDist(rng_), 0.5f, posDist(rng_)), dir, 10.0f, 2.0f);
        }
    } else {
        std::random_device rd;
        rng_.seed(rd());
        for (int i = 0; i < 3; ++i) {
            spawnEnemy();
        }
        spawnTimer_ = spawnInterval_;
    }
}

void Game::update(const Input& input, float deltaTime, Profiler& profiler) {
    VA_PROFILE_SCOPE(profiler, ProfileSection::GameUpdate);

    if (state_ == GameState::GameOver) {
        if (input.isKeyDown(GLFW_KEY_R)) {
            reset();
        }
        return;
    }

    bool isStress = options_.stressCount > 0;

    {
        VA_PROFILE_SCOPE(profiler, ProfileSection::PlayerUpdate);
        player_.update(input, deltaTime);
    }

    if (!isStress) {
        fireCooldown_ -= deltaTime;
        if (input.isKeyDown(GLFW_KEY_SPACE) && fireCooldown_ <= 0.0f) {
            spawnProjectile();
            fireCooldown_ = fireInterval_;
        }

        spawnTimer_ -= deltaTime;
        if (spawnTimer_ <= 0.0f && enemies_.size() < (size_t)maxEnemies_) {
            spawnEnemy();
            spawnTimer_ = spawnInterval_;
        }
    }

    {
        VA_PROFILE_SCOPE(profiler, ProfileSection::ProjectileUpdate);
        for (auto& proj : projectiles_) {
            if (proj.active()) {
                proj.update(deltaTime);
                if (isStress && !proj.active()) {
                    // Respawn projectile in stress mode
                    std::uniform_real_distribution<float> posDist(-std::sqrt(static_cast<float>(options_.stressCount)) * 2.5f / 2.0f, std::sqrt(static_cast<float>(options_.stressCount)) * 2.5f / 2.0f);
                    proj = Projectile(glm::vec3(posDist(rng_), 0.5f, posDist(rng_)), glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, 2.0f);
                }
            }
        }
    }

    {
        VA_PROFILE_SCOPE(profiler, ProfileSection::EnemyAI);
        for (auto& enemy : enemies_) {
            if (enemy.alive()) {
                bool attacked = enemy.update(player_.position(), deltaTime);
                if (attacked && !isStress) {
                    player_.takeDamage(10);
                }
            }
        }
    }

    {
        VA_PROFILE_SCOPE(profiler, ProfileSection::Collision);
        collisionStats_ = GameplayCollisionStats{};
        if (options_.collisionMode == GameplayCollisionMode::SpatialGrid) {
            resolveProjectileEnemyCollisionsSpatialGrid();
        } else {
            resolveProjectileEnemyCollisionsBruteForce();
        }
    }

    if (!isStress) {
        removeInactiveObjects();
        if (!player_.isAlive()) {
            state_ = GameState::GameOver;
        }
    }
}

void Game::reset() {
    player_.reset(glm::vec3(0.0f, 0.5f, 0.0f));

    enemies_.clear();
    projectiles_.clear();

    score_ = 0;
    fireCooldown_ = 0.0f;
    state_ = GameState::Running;

    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }

    // Reset spawn timer so the first additional enemy appears after the interval.
    spawnTimer_ = spawnInterval_;
}

GameState Game::state() const { return state_; }
int Game::score() const { return score_; }
const Player& Game::player() const { return player_; }
const std::vector<Enemy>& Game::enemies() const { return enemies_; }
const std::vector<Projectile>& Game::projectiles() const { return projectiles_; }

void Game::spawnEnemy() {
    std::uniform_int_distribution<int>   edgeDist(0, 3);
    std::uniform_real_distribution<float> posDist(-4.2f, 4.2f);

    glm::vec3 spawnPos(0.0f, 0.5f, 0.0f);
    bool validPosition = false;
    int retries = 5;

    // Minimum spawn distance from the player.
    // The comparison uses SQUARED distance to avoid an unnecessary sqrt().
    constexpr float kMinSpawnDistance    = 2.5f;
    constexpr float kMinSpawnDistanceSq  = kMinSpawnDistance * kMinSpawnDistance;

    while (!validPosition && retries > 0) {
        int   edge = edgeDist(rng_);
        float pos  = posDist(rng_);

        if      (edge == 0) { spawnPos.x = -4.2f; spawnPos.z = pos; }  // left
        else if (edge == 1) { spawnPos.x =  4.2f; spawnPos.z = pos; }  // right
        else if (edge == 2) { spawnPos.z = -4.2f; spawnPos.x = pos; }  // top
        else                { spawnPos.z =  4.2f; spawnPos.x = pos; }  // bottom

        glm::vec3 diff = spawnPos - player_.position();
        float distSq   = diff.x * diff.x + diff.z * diff.z;

        // Correct comparison: distSq is distance^2, so compare against
        // kMinSpawnDistance^2 rather than the raw distance value.
        if (distSq >= kMinSpawnDistanceSq) {
            validPosition = true;
        }
        --retries;
    }

    if (validPosition) {
        enemies_.emplace_back(spawnPos, 1.5f);
    }
}

void Game::spawnProjectile() {
    glm::vec3 spawnPos = player_.position() + player_.facingDirection() * 0.7f;
    projectiles_.emplace_back(spawnPos, player_.facingDirection(), 10.0f, 2.0f);
}

void Game::resolveProjectileEnemyCollisionsBruteForce() {
    bool isStress = options_.stressCount > 0;
    for (auto& proj : projectiles_) {
        if (!proj.active()) continue;
        for (auto& enemy : enemies_) {
            if (!enemy.alive()) continue;
            
            collisionStats_.candidateChecks++;
            if (proj.bounds().intersects(enemy.bounds())) {
                collisionStats_.intersections++;
                if (!isStress) {
                    proj.deactivate();
                    enemy.kill();
                    score_ += 100;
                    break;
                }
            }
        }
    }
}

void Game::resolveProjectileEnemyCollisionsSpatialGrid() {
    bool isStress = options_.stressCount > 0;
    enemyGrid_.clear();
    
    for (std::size_t i = 0; i < enemies_.size(); ++i) {
        if (enemies_[i].alive()) {
            enemyGrid_.insert(i, enemies_[i].bounds());
            collisionStats_.gridCellInsertions++;
        }
    }
    collisionStats_.occupiedCells = enemyGrid_.occupiedCellCount();

    for (auto& proj : projectiles_) {
        if (!proj.active()) continue;

        enemyGrid_.query(proj.bounds(), collisionCandidatesScratch_);

        for (std::size_t enemyIdx : collisionCandidatesScratch_) {
            auto& enemy = enemies_[enemyIdx];
            if (!enemy.alive()) continue;
            
            collisionStats_.candidateChecks++;
            if (proj.bounds().intersects(enemy.bounds())) {
                collisionStats_.intersections++;
                if (!isStress) {
                    proj.deactivate();
                    enemy.kill();
                    score_ += 100;
                    break;
                }
            }
        }
    }
}

void Game::removeInactiveObjects() {
    std::erase_if(projectiles_, [](const Projectile& p) { return !p.active(); });
    std::erase_if(enemies_,     [](const Enemy& e)      { return !e.alive();  });
}
