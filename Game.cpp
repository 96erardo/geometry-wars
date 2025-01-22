#include<SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>
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
  
  m_window = sf::RenderWindow(
    sf::VideoMode({ m_windowConf.W, m_windowConf.H }), 
    "Geometry Wars",
    sf::Style::Default,
    m_windowConf.FS == 0 ? sf::State::Windowed : sf::State::Fullscreen
  );

  m_window.setFramerateLimit(m_windowConf.FL);

  spawnPlayer();
  spawnEnemy();
}

void Game::run () {
  while (m_running) {
    m_entities.update();

    sUserInput();
    sMovement();
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
      entity->cTransform->pos += entity->cTransform->velocity;

      if (!entity->cInput) {
        auto [width, height] = m_window.getSize();

        if ((entity->cTransform->pos.x - entity->cShape->circle.getRadius()) <= 0 || 
          (entity->cTransform->pos.x + entity->cShape->circle.getRadius()) >= width
        ) {
          entity->cTransform->velocity.x *= -1;
        }

        if ((entity->cTransform->pos.y - entity->cShape->circle.getRadius()) <= 0 || 
          (entity->cTransform->pos.y + entity->cShape->circle.getRadius()) >= height
        ) {
          entity->cTransform->velocity.y *= -1;
        }
      }

      entity->cTransform->angle = std::fmod(entity->cTransform->angle + 1.0f, 360);
    }
  }
}

void Game::sRender () {
  m_window.clear(sf::Color::Black);

  for (auto entity: m_entities.getEntities()) {
    if (entity->cTransform) {
      entity->cShape->circle.setPosition({
        entity->cTransform->pos.x,
        entity->cTransform->pos.y,
      });

      entity->cShape->circle.setRotation(
        sf::degrees(entity->cTransform->angle)
      );
    }

    m_window.draw(entity->cShape->circle);
  }

  m_window.display();
}