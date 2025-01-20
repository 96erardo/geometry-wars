#include <iostream>
#include <map>
#include "Entity.h"

class EntityManager {
  std::vector<Entity> m_entities;
  std::map<std::string, std::vector<Entity>> m_entityMap;
  std::vector<Entity> m_toAdd;

  public:
    EntityManager();

    void init ();

    void update ();

    std::shared_ptr<Entity> addEntity ();

    std::vector<Entity> getEntities();

    std::vector<Entity> getEntities(std::string& tag);
};