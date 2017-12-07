// entity.cc

// todo: union for entity types
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
  v3 position;
  bool active;
  // union 
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

const unsigned ENTITYCOLOR[ENTITY_TYPE_COUNT]=
{
  0xff0000ff, // spawn
  0xff00ff00, // finish
  0xff00ffff, // pickup
  0xffff00ff, // spike
  0xff000000, // invalid
};

static unsigned entity_color(EntityType type)
{
  return ENTITYCOLOR[type];
}

static bool is_entity_type(unsigned color,EntityType type)
{
  return ENTITYCOLOR[type]==color;
}
