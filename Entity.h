#include <string>
#include <memory>
#include "Components.h"

class Entity {
  size_t      m_id     = 0;
  std::string m_tag    = "default";
  bool        m_active = true;
  

  public:
    Entity (size_t id, const std::string& tag);

    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;


    size_t id () const;
    std::string tag () const;
    bool isActive () const;
    void destroy ();
};