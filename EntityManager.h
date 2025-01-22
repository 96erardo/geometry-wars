#include <iostream>
#include <memory>
#include <map>
#include "Entity.h"

class EntityManager {
  size_t m_lastId = 0;
  std::vector<std::shared_ptr<Entity>> m_entities;
  std::map<std::string, std::vector<std::shared_ptr<Entity>>> m_entityMap;
  std::vector<std::shared_ptr<Entity>> m_toAdd;

  public:
    EntityManager();

    void init ();

    void update ();

    std::shared_ptr<Entity> addEntity (const std::string& tag);

    std::vector<std::shared_ptr<Entity>> getEntities();

    std::vector<std::shared_ptr<Entity>> getEntities(std::string& tag);
};