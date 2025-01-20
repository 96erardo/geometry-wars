#include<SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"

struct WindowConfig { unsigned int W; unsigned int H; int FL; int FS; };
struct FontConfig { std::string F; float S; int R; int G; int B; };
struct PlayerConfig { int SR; int CR; float S; int FR; int FG; int FB; int OR; int OG; int OB; int OT; int V; };
struct EnemyConfig { int SR; int CR; float SMIN; float SMAX; int OR; int OG; int OB; int OT; int VMIN; int VMAX; int L; int SP; };
struct BulletConfig { int SR; int CR; float S; int FR; int FG; int FB; int OR; int OG; int OB; int OT; int V; int L; };

class Game {
  sf::RenderWindow m_window;
  EntityManager m_entities;
  bool m_paused;
  bool m_running;

  WindowConfig m_windowConf {};
  FontConfig m_fontConf {};
  PlayerConfig m_playerConf {};
  EnemyConfig m_enemyConf {};
  BulletConfig m_bulletConf {};

  void init (const std::string& config);

  public:
    Game (const std::string& config);

    void run ();
};