class Entity {
  size_t      m_id;
  std::string m_tag;
  bool        m_active = true;

  public:
    Entity (size_t id, std::string& tag);

    size_t id () const;
    std::string tag () const;
    bool isActive () const;
    void destroy ();
};