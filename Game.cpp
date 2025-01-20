#include<SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "Game.h"

Game::Game (const std::string& config) {
  init(config);
}

void Game::init (const std::string& config) {
  std::string type;
  std::ifstream file(config, std::ios::in);


  while (file >> type) {
    std::cout << type << "\n";

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

    m_window = sf::RenderWindow(
      sf::VideoMode({ m_windowConf.W, m_windowConf.H }), 
      "Geometry Wars",
      sf::Style::Default,
      m_windowConf.FS == 0 ? sf::State::Windowed : sf::State::Fullscreen
    );

    m_window.setFramerateLimit(m_windowConf.FL);
  }
}

void Game::run () {
  while (m_window.isOpen()) {
    while (const std::optional event = m_window.pollEvent()) {

      if (event->is<sf::Event::Closed>()) {
        m_window.close();
      }
    }

    m_window.clear(sf::Color::Black);

    m_window.display();
  }
}