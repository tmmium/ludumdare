// entity.cc

enum EntityType
{
  ENTITY_SPAWN,
  ENTITY_FINISH,
  ENTITY_PICKUP,
  ENTITY_SPIKE,
  ENTITY_INVALID,
  ENTITY_TYPE_COUNT,
};

struct Entity
{
  EntityType type;
  bool active;
  v3 position;
  float timer;
  float y_bob;
  bool triggered;
};

void reset(Entity* entity)
{
  entity->active=true;
  if (entity->type==ENTITY_PICKUP)
    entity->timer=entity->position.x;
  else
    entity->timer=0.0f;
  entity->y_bob=0.0f;
  entity->triggered=false;
}

void init(Entity* entity,EntityType type,const v3 position)
{
  entity->type=type;
  entity->position=position;
  reset(entity);
}

v3 entity_position(const Entity* entity)
{
  v3 res=entity->position;
  res.y+=entity->y_bob;
  return res;
}

float entity_rotation(const Entity* entity)
{
  return entity->timer;
}
