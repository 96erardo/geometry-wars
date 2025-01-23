#include<SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "EntityManager.h"

struct WindowConfig { unsigned int W, H; int FL, FS; };
struct FontConfig { std::string F; float S; int R, G, B; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SP; float SMIN; float SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game {
  sf::RenderWindow m_window;
  EntityManager    m_entities;
  WindowConfig     m_windowConf {};
  FontConfig       m_fontConf {};
  PlayerConfig     m_playerConf {};
  EnemyConfig      m_enemyConf {};
  BulletConfig     m_bulletConf {};
  int              m_score = 0;
  size_t           m_currentFrame = 0;
  int              m_lastEnemySpawnTime = 0;
  bool             m_paused = false;
  bool             m_running = true;
  std::time_t      m_seed = time(nullptr);

  std::shared_ptr<Entity> m_player;

  void init (const std::string& config);
  void setPaused (bool paused);

  void sMovement();
  void sUserInput();
  void sLifespan();
  void sRender();
  void sEnemySpawner();
  void sCollision();

  void spawnPlayer();
  void spawnEnemy();
  void spawnSmallEnemies(const std::shared_ptr<Entity> entity);
  void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
  void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

  public:
    Game (const std::string& config);

    void run ();
};