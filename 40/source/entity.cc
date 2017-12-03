// entity.cc

enum EntityType
{
  ENTITY_INVALID,
  ENTITY_SPAWN,
  ENTITY_FINISH,
  ENTITY_TYPE_COUNT,
};

struct Entity
{
  EntityType type;
  v3 position;
  v3 animation_offset;
  float timer;
  Mesh mesh;
};

void init(Entity* entity,EntityType type,const v3 position)
{
  entity->type=type;
  entity->position=position;
  entity->animation_offset={0.0f,0.0f,0.0f};
  entity->timer=0.0f;
  init(&entity->mesh,6*6);
}
