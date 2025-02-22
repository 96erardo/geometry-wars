#include<SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <numbers>
#include "Game.h"
#include "utils.h"

Game::Game (const std::string& config) {
  init(config);
}

void Game::init (const std::string& config) {
  std::string type;
  std::ifstream file(config, std::ios::in);

  while (file >> type) {
    if (type == "Window") {
      file >> m_windowConf.W >> m_windowConf.H >> m_windowConf.FL >> m_windowConf.FS;
    
    } else if (type == "Font") {
      file >> m_fontConf.F >> m_fontConf.S >> m_fontConf.R >> m_fontConf.G >> m_fontConf.B;
    
    } else if (type == "Player") {
      file >> m_playerConf.SR >> m_playerConf.CR >> m_playerConf.S >> m_playerConf.FR >> m_playerConf.FG >> m_playerConf.FB >> m_playerConf.OR >> m_playerConf.OG >> m_playerConf.OB >> m_playerConf.OT >> m_playerConf.V;
    
    } else if (type == "Enemy") {
      file >> m_enemyConf.SR >> m_enemyConf.CR >> m_enemyConf.SMIN >> m_enemyConf.SMAX >> m_enemyConf.OR >> m_enemyConf.OG >> m_enemyConf.OB >> m_enemyConf.OT >> m_enemyConf.VMIN >> m_enemyConf.VMAX >> m_enemyConf.L >> m_enemyConf.SP;
    
    } else if (type == "Bullet") {
      file >> m_bulletConf.SR >> m_bulletConf.CR >> m_bulletConf.S >> m_bulletConf.FR >> m_bulletConf.FG >> m_bulletConf.FB >> m_bulletConf.OR >> m_bulletConf.OG >> m_bulletConf.OB >> m_bulletConf.OT >> m_bulletConf.V >> m_bulletConf.L;
    }
  }

  std::time_t seed = time(nullptr);
  std::srand(seed);

  if (!m_font.openFromFile(m_fontConf.F)) {
    std::cout << "Could not load font " << m_fontConf.F << "\n";

    return;
  }
  
  m_window = sf::RenderWindow(
    sf::VideoMode({ m_windowConf.W, m_windowConf.H }), 
    "Geometry Wars",
    sf::Style::Default,
    m_windowConf.FS == 0 ? sf::State::Windowed : sf::State::Fullscreen
  );

  m_window.setFramerateLimit(m_windowConf.FL);

  createScore();
  spawnPlayer();
}

void Game::run () {
  while (m_running) {
    m_entities.update();

    sUserInput();
    sMovement();
    sLifespan();
    sEnemySpawner();
    sScore();
    sCollision();
    sRender();
  }
}

void Game::spawnPlayer () {
  auto entity = m_entities.addEntity("player");

  auto [width, height] = m_window.getSize();

  float x = width / 2;
  float y = height / 2;

  entity->cTransform = std::make_shared<CTransform>(
    Vec2(x, y), 
    Vec2(0, 0),
    0.0
  );
  
  entity->cShape = std::make_shared<CShape>(
    m_playerConf.SR,
    m_playerConf.V,
    sf::Color(m_playerConf.FR, m_playerConf.FG, m_playerConf.FB),
    sf::Color(m_playerConf.OR, m_playerConf.OG, m_playerConf.OB),
    m_playerConf.OT
  );

  entity->cInput = std::make_shared<CInput>();

  entity->cCollision = std::make_shared<CCollision>(m_playerConf.CR);

  m_player = entity;
}

void Game::spawnEnemy () {
  auto entity = m_entities.addEntity("enemy");

  auto [width, height] = m_window.getSize();
  float minX = m_enemyConf.SR;
  float maxX = width - m_enemyConf.SR;
  float minY = m_enemyConf.SR;
  float maxY = height - m_enemyConf.SR;
  float radAngle = (2 * (atan(1) * 4));

  float x = between(maxX, minX);
  float y = between(maxY, minY);

  float speed = between(m_enemyConf.SMAX, m_enemyConf.SMIN);
  float angle = between(radAngle);

  float xSpeed = speed * cos(angle);
  float ySpeed = speed * sin(angle);
  
  int points = between(m_enemyConf.VMAX, m_enemyConf.VMIN);

  int r = between(255);
  int g = between(255);
  int b = between(255);

  entity->cTransform = std::make_shared<CTransform>(
    Vec2(x, y),
    Vec2(xSpeed, ySpeed),
    0.0
  );

  entity->cShape = std::make_shared<CShape>(
    m_enemyConf.SR,
    points,
    sf::Color(r, g, b),
    sf::Color(m_enemyConf.OR, m_enemyConf.OG, m_enemyConf.OB),
    m_enemyConf.OT
  );

  entity->cCollision = std::make_shared<CCollision>(m_enemyConf.CR);

  entity->cScore = std::make_shared<CScore>(
    points * 100
  );
}

void Game::spawnSmallEnemies (const std::shared_ptr<Entity> parent) {
  float x = parent->cTransform->pos.x;
  float y = parent->cTransform->pos.y;
  int points = parent->cShape->circle.getPointCount();
  float angleDistance = (2*(atan(1) * 4)) / points;
  float speed = between(m_enemyConf.SMAX, m_enemyConf.SMIN);

  for (int i = 0; i < points; i++) {
    auto entity = m_entities.addEntity("enemy");
    
    float angle = angleDistance * i;
    float xSpeed = cos(angle) * speed;
    float ySpeed = sin(angle) * speed;

    entity->cTransform = std::make_shared<CTransform>(
      Vec2(x, y),
      Vec2(xSpeed, ySpeed),
      0
    );

    entity->cShape = std::make_shared<CShape>(
      m_enemyConf.SR / 2,
      points,
      parent->cShape->circle.getFillColor(),
      parent->cShape->circle.getOutlineColor(),
      m_enemyConf.OT
    );

    entity->cLifespan = std::make_shared<CLifespan>(m_enemyConf.L);

    entity->cCollision = std::make_shared<CCollision>(
      m_enemyConf.CR / 2
    );

    entity->cScore = std::make_shared<CScore>(
      points * 200
    );
  }
}

void Game::spawnBullet (const std::shared_ptr<Entity> from, const Vec2& point) {
  auto entity = m_entities.addEntity("bullet");

  float co = point.y - from->cTransform->pos.y;
  float ca = point.x - from->cTransform->pos.x;
  float angle = atan2(co, ca);

  float xSpeed = cos(angle) * m_bulletConf.S;
  float ySpeed = sin(angle) * m_bulletConf.S;

  entity->cTransform = std::make_shared<CTransform>(
    Vec2(from->cTransform->pos.x, from->cTransform->pos.y),
    Vec2(xSpeed, ySpeed),
    0
  );

  entity->cShape = std::make_shared<CShape>(
    m_bulletConf.SR,
    m_bulletConf.V,
    sf::Color(m_bulletConf.FR, m_bulletConf.FG, m_bulletConf.FB),
    sf::Color(m_bulletConf.OR, m_bulletConf.OG, m_bulletConf.OB),
    m_bulletConf.OT
  );

  entity->cLifespan = std::make_shared<CLifespan>(
    m_bulletConf.L
  );

  entity->cCollision = std::make_shared<CCollision>(m_bulletConf.CR);
}

void Game::createScore () {
  auto entity = m_entities.addEntity("text");

  entity->cTransform = std::make_shared<CTransform>(
    Vec2(10, 10),
    Vec2(0, 0),
    0
  );

  entity->cText = std::make_shared<CText>(
    m_fontConf.S,
    m_font,
    sf::Color(m_fontConf.R, m_fontConf.G, m_fontConf.B)
  );

  entity->cText->text.setString("SCORE: " + std::to_string(m_score));
}

void Game::sUserInput () {
  while (const std::optional event = m_window.pollEvent()) {

    if (event->is<sf::Event::Closed>()) {
      m_window.close();
      m_running = false;
    
    } else {
      if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scan::W) {
          m_player->cInput->up = true;
        }

        if (keyPressed->scancode == sf::Keyboard::Scan::A) {
          m_player->cInput->left = true;
        }

        if (keyPressed->scancode == sf::Keyboard::Scan::S) {
          m_player->cInput->down = true;
        }

        if (keyPressed->scancode == sf::Keyboard::Scan::D) {
          m_player->cInput->right = true;
        }
    
        if (m_windowConf.FS == 1 && keyPressed->scancode == sf::Keyboard::Scan::Escape) {
          m_running = false;
        }
      
      } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->scancode == sf::Keyboard::Scan::W) {
          m_player->cInput->up = false;
        }

        if (keyReleased->scancode == sf::Keyboard::Scan::A) {
          m_player->cInput->left = false;
        }

        if (keyReleased->scancode == sf::Keyboard::Scan::S) {
          m_player->cInput->down = false;
        }

        if (keyReleased->scancode == sf::Keyboard::Scan::D) {
          m_player->cInput->right = false;
        }
      
      } else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        m_player->cInput->shoot = true;

        spawnBullet(m_player, Vec2(mousePressed->position.x, mousePressed->position.y));
      
      } else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonReleased>()) {
        m_player->cInput->shoot = false;
      }
    }
  }
}

void Game::sMovement () {
  for (auto entity: m_entities.getEntities()) {
    if (entity->cInput) {
      entity->cTransform->velocity.x = 0;
      entity->cTransform->velocity.y = 0;

      if (entity->cInput->up) {        
        entity->cTransform->velocity += Vec2(0, -m_playerConf.S);
      }

      if (entity->cInput->down) {
        entity->cTransform->velocity += Vec2(0, m_playerConf.S);
      }

      if (entity->cInput->left) {
        entity->cTransform->velocity += Vec2(-m_playerConf.S, 0);
      }

      if (entity->cInput->right) {
        entity->cTransform->velocity += Vec2(m_playerConf.S, 0);
      }
    }
  
    if (entity ->cTransform) {
      Vec2 nextPos = entity->cTransform->pos + entity->cTransform->velocity;
      
      auto [width, height] = m_window.getSize();

      if (entity->tag() != "bullet" && entity->tag() != "text") {
        if ((nextPos.x - entity->cShape->circle.getRadius()) <= 0 || 
          (nextPos.x + entity->cShape->circle.getRadius()) >= width
        ) {
          if (entity->cInput) {
            entity->cTransform->velocity.x = 0;
          } else {
            entity->cTransform->velocity.x *= -1;
          }
        }

        if ((nextPos.y - entity->cShape->circle.getRadius()) <= 0 || 
          (nextPos.y + entity->cShape->circle.getRadius()) >= height
        ) {
          if (entity->cInput) {
            entity->cTransform->velocity.y = 0;
          } else {
            entity->cTransform->velocity.y *= -1;
          }
        }
      }

      entity->cTransform->pos += entity->cTransform->velocity;

      if (entity->cShape) {
        entity->cTransform->angle = std::fmod(entity->cTransform->angle + 1.0f, 360);
        
        entity->cShape->circle.setPosition({
          entity->cTransform->pos.x,
          entity->cTransform->pos.y,
        });

        entity->cShape->circle.setRotation(
          sf::degrees(entity->cTransform->angle)
        );
      }

      if (entity->cText) {
        entity->cText->text.setPosition({
          entity->cTransform->pos.x,
          entity->cTransform->pos.y,
        });
      }
    }
  }
}

void Game::sLifespan () {
  for (auto entity: m_entities.getEntities()) {
    if (entity->cLifespan) {
      entity->cLifespan->remaining -= 1;

      if (entity->cLifespan->remaining == 0) {
        entity->destroy();
      }

      if (entity->cShape) {
        sf::Color fill = entity->cShape->circle.getFillColor();
        sf::Color outline = entity->cShape->circle.getOutlineColor();
        int alpha = (entity->cLifespan->remaining * 255) / entity->cLifespan->total;

        entity->cShape->circle.setFillColor(sf::Color(fill.r, fill.g, fill.b, alpha));
        entity->cShape->circle.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, alpha));
      }
    }
  }
}

void Game::sEnemySpawner () {
  m_currentFrame++;

  if (m_currentFrame % m_enemyConf.SP == 0) {
    spawnEnemy();
  }
}

void Game::sScore () {
  for (auto entity: m_entities.getEntities("text")) {
    if (entity->cText) {
      entity->cText->text.setString("SCORE: " + std::to_string(m_score));
    }
  }
}

void Game::sCollision () {
  std::vector<std::shared_ptr<Entity>> collided;
  bool playerHit = false;

  for (auto bullet: m_entities.getEntities("bullet")) {
    for (auto enemy: m_entities.getEntities("enemy")) {
      Vec2 dist = bullet->cTransform->pos - enemy->cTransform->pos;
      
      if (
        ((dist.x * dist.x) + (dist.y * dist.y)) <
        ((bullet->cShape->circle.getRadius() + enemy->cShape->circle.getRadius()) * (bullet->cShape->circle.getRadius() + enemy->cShape->circle.getRadius()))
      ) {

        if (!enemy->cLifespan) {
          spawnSmallEnemies(enemy);
        }

        m_score += enemy->cScore->score;

        bullet->destroy();
        enemy->destroy();
      }
    }
  }

  for (auto enemy: m_entities.getEntities("enemy")) {
    Vec2 dist = m_player->cTransform->pos - enemy->cTransform->pos;
    
    if (
      playerHit == false &&
      enemy->isActive() &&
      ((dist.x * dist.x) + (dist.y * dist.y)) <
      ((m_player->cShape->circle.getRadius() + enemy->cShape->circle.getRadius()) * (m_player->cShape->circle.getRadius() + enemy->cShape->circle.getRadius()))
    ) {
      playerHit = true;
      m_player->destroy();
      enemy->destroy();
    }
  }

  if (playerHit) {
    spawnPlayer();
  }
}

void Game::sRender () {
  m_window.clear(sf::Color::Black);

  for (auto entity: m_entities.getEntities()) {
    if (entity->cShape) {
      m_window.draw(entity->cShape->circle);
    }

    if (entity->cText) {
      m_window.draw(entity->cText->text);
    }
  }

  m_window.display();
}