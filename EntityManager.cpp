#include <iostream>
#include <map>
#include "EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::init () {

}

void EntityManager::update () {
  for (auto add : m_toAdd) {
    m_entities.push_back(add);
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