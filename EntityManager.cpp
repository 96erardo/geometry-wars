#include <iostream>
#include <map>
#include "EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::init () {

}

void EntityManager::update () {
  m_entities.erase(
    std::remove_if(m_entities.begin(), m_entities.end(), [](std::shared_ptr<Entity> e) {
      if (e->isActive() == false) {
        std::cout << e->id() << " destroyed\n";
      }

      return e->isActive() == false;
    }),
    m_entities.end()
  );

  // for (const auto& [tag, entities] : m_entityMap) {
  //  std::remove_if(entities.begin(), entities.end(), [](std::shared_ptr<Entity> e) { return e->isActive() == false; });
  // }

  for (const auto& add : m_toAdd) {
    m_entities.push_back(add);
    m_entityMap[add->tag()].push_back(add);
  }

  m_toAdd.clear();
}

std::shared_ptr<Entity> EntityManager::addEntity (const std::string& tag) {
  std::shared_ptr<Entity> entity = std::make_shared<Entity>(m_lastId++, tag);

  m_toAdd.push_back(entity);

  return entity;
}

std::vector<std::shared_ptr<Entity>> EntityManager::getEntities() {
  return m_entities;
}

// std::vector<Entity> EntityManager::getEntities(std::string& tag) {

// }