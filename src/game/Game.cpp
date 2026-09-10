#include "Game.hpp"

#include <GLFW/glfw3.h>

Game::Game()
    : player_(glm::vec3(0.0f, 0.5f, 0.0f), 5.0f) {
    std::random_device rd;
    rng_.seed(rd());
    
    // Initial spawn
    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}

void Game::update(const Input& input, float deltaTime) {
    if (state_ == GameState::GameOver) {
        if (input.isKeyDown(GLFW_KEY_R)) {
            reset();
        }
        return;
    }

    // 1. update Player
    player_.update(input, deltaTime);

    // 2. update fire cooldown
    fireCooldown_ -= deltaTime;

    // 3. process shooting
    if (input.isKeyDown(GLFW_KEY_SPACE) && fireCooldown_ <= 0.0f) {
        spawnProjectile();
        fireCooldown_ = fireInterval_;
    }

    // 4. update enemy spawn timer
    spawnTimer_ -= deltaTime;

    // 5. spawn enemies if needed
    if (spawnTimer_ <= 0.0f && enemies_.size() < (size_t)maxEnemies_) {
        spawnEnemy();
        spawnTimer_ = spawnInterval_;
    }

    // 6. update Projectiles
    for (auto& proj : projectiles_) {
        if (proj.active()) {
            proj.update(deltaTime);
        }
    }

    // 7. update Enemies & 8. apply Enemy attacks to Player
    for (auto& enemy : enemies_) {
        if (enemy.alive()) {
            bool attacked = enemy.update(player_.position(), deltaTime);
            if (attacked) {
                player_.takeDamage(10); // Fixed 10 damage as suggested
            }
        }
    }

    // 9. resolve projectile/enemy collisions
    resolveProjectileEnemyCollisions();

    // 10. remove inactive/dead objects
    removeInactiveObjects();

    // 11. check Player death & 12. switch to GameOver if needed
    if (!player_.isAlive()) {
        state_ = GameState::GameOver;
    }
}

void Game::reset() {
    player_.reset(glm::vec3(0.0f, 0.5f, 0.0f));
    
    enemies_.clear();
    projectiles_.clear();
    
    score_ = 0;
    spawnTimer_ = spawnInterval_;
    fireCooldown_ = 0.0f;
    state_ = GameState::Running;
    
    for (int i = 0; i < 3; ++i) {
        spawnEnemy();
    }
}

GameState Game::state() const {
    return state_;
}

int Game::score() const {
    return score_;
}

const Player& Game::player() const {
    return player_;
}

const std::vector<Enemy>& Game::enemies() const {
    return enemies_;
}

const std::vector<Projectile>& Game::projectiles() const {
    return projectiles_;
}

void Game::spawnEnemy() {
    std::uniform_int_distribution<int> edgeDist(0, 3);
    std::uniform_real_distribution<float> posDist(-4.2f, 4.2f);

    glm::vec3 spawnPos(0.0f, 0.5f, 0.0f);
    bool validPosition = false;
    int retries = 5;

    while (!validPosition && retries > 0) {
        int edge = edgeDist(rng_);
        float pos = posDist(rng_);
        
        if (edge == 0) { spawnPos.x = -4.2f; spawnPos.z = pos; }       // left
        else if (edge == 1) { spawnPos.x = 4.2f; spawnPos.z = pos; }  // right
        else if (edge == 2) { spawnPos.z = -4.2f; spawnPos.x = pos; } // top
        else { spawnPos.z = 4.2f; spawnPos.x = pos; }                 // bottom
        
        // Spawn safety: require squared distance of ~2.5 from player
        glm::vec3 diff = spawnPos - player_.position();
        float distSq = diff.x * diff.x + diff.z * diff.z;
        if (distSq >= 2.5f) {
            validPosition = true;
        }
        retries--;
    }
    
    if (validPosition) {
        enemies_.emplace_back(spawnPos, 1.5f); // 1.5f speed as suggested
    }
}

void Game::spawnProjectile() {
    glm::vec3 spawnPos = player_.position() + player_.facingDirection() * 0.7f;
    projectiles_.emplace_back(spawnPos, player_.facingDirection(), 10.0f, 2.0f); // 10 speed, 2s lifetime
}

void Game::resolveProjectileEnemyCollisions() {
    float enemyRadius = 0.5f;
    float projRadius = 0.15f;
    float combinedRadius = enemyRadius + projRadius;
    float combinedRadiusSq = combinedRadius * combinedRadius;

    for (auto& proj : projectiles_) {
        if (!proj.active()) continue;

        for (auto& enemy : enemies_) {
            if (!enemy.alive()) continue;

            glm::vec3 diff = proj.position() - enemy.position();
            float distSq = diff.x * diff.x + diff.z * diff.z + diff.y * diff.y;
            
            if (distSq <= combinedRadiusSq) {
                proj.deactivate();
                enemy.kill();
                score_ += 100; // 100 points per enemy
                break; // One projectile can hit one enemy
            }
        }
    }
}

void Game::removeInactiveObjects() {
    std::erase_if(projectiles_, [](const Projectile& p) { return !p.active(); });
    std::erase_if(enemies_, [](const Enemy& e) { return !e.alive(); });
}
