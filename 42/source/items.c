// items.c

#define kBackpackItemCapacity          64
static const int kItemListHeight       = 7;

typedef enum
{
   ITEM_CATEGORY_WEAPON,
   ITEM_CATEGORY_ARMOR,
   ITEM_CATEGORY_HEALTH,
} item_category_t;

static const char *kItemCategoryNameTable[] =
{
   "Weapon",
   "Armor",
   "Health",
};

typedef enum 
{
   ITEM_LEVEL_NONE,
   ITEM_LEVEL_OLD,
   ITEM_LEVEL_COMMON,
   ITEM_LEVEL_UNCOMMON,
   ITEM_LEVEL_RARE,
   ITEM_LEVEL_EPIC,
   ITEM_LEVEL_LEGENDARY,
   ITEM_LEVEL_COUNT,
} item_level_t;

static const int kItemLevelProbabilityTable[] =
{
   0,    // ITEM_LEVEL_NONE,
   100,  // ITEM_LEVEL_OLD,
   80,   // ITEM_LEVEL_COMMON,
   50,   // ITEM_LEVEL_UNCOMMON,
   15,   // ITEM_LEVEL_RARE,
   7,    // ITEM_LEVEL_EPIC,
   1,    // ITEM_LEVEL_LEGENDARY,
};

static const int kItemLevelColorIndexTable[] =
{
   COLOR_BLACK,
   COLOR_LIGHT_GRAY,
   COLOR_WHITE,
   COLOR_LIGHT_GREEN,
   COLOR_BLUE,
   COLOR_PURPLE,
   COLOR_ORANGE,
};

typedef struct 
{
   const char *name;
   int level;
   int category;
} item_desc_t;

static item_desc_t kItemDesc[] = 
{
   // weapons
   { "Stick"            , ITEM_LEVEL_OLD         , ITEM_CATEGORY_WEAPON },
   { "Fork"             , ITEM_LEVEL_OLD         , ITEM_CATEGORY_WEAPON },
   { "Table Spoon"      , ITEM_LEVEL_OLD         , ITEM_CATEGORY_WEAPON },
   { "Knife"            , ITEM_LEVEL_COMMON      , ITEM_CATEGORY_WEAPON },
   { "Trent Razor"      , ITEM_LEVEL_COMMON      , ITEM_CATEGORY_WEAPON },
   { "Dillinger"        , ITEM_LEVEL_COMMON      , ITEM_CATEGORY_WEAPON },
   { "Machette"         , ITEM_LEVEL_UNCOMMON    , ITEM_CATEGORY_WEAPON },
   { ".38 Special"      , ITEM_LEVEL_UNCOMMON    , ITEM_CATEGORY_WEAPON },
   { "Luger"            , ITEM_LEVEL_UNCOMMON    , ITEM_CATEGORY_WEAPON },
   { "Flitlock Musket"  , ITEM_LEVEL_UNCOMMON    , ITEM_CATEGORY_WEAPON },
   { "Shotgun"          , ITEM_LEVEL_RARE        , ITEM_CATEGORY_WEAPON },
   { "Loochester"       , ITEM_LEVEL_RARE        , ITEM_CATEGORY_WEAPON },
   { "Crossbow"         , ITEM_LEVEL_RARE        , ITEM_CATEGORY_WEAPON },
   { "Carl Gustav m/45" , ITEM_LEVEL_RARE        , ITEM_CATEGORY_WEAPON },
   { "AK-47"            , ITEM_LEVEL_EPIC        , ITEM_CATEGORY_WEAPON },
   { "M4 SpecOpz"       , ITEM_LEVEL_EPIC        , ITEM_CATEGORY_WEAPON },
   { "Mini-gun"         , ITEM_LEVEL_LEGENDARY   , ITEM_CATEGORY_WEAPON },
   { "Sniper-Fifle"     , ITEM_LEVEL_LEGENDARY   , ITEM_CATEGORY_WEAPON },

   // armor
   { "T-Shirt"          , ITEM_LEVEL_OLD         , ITEM_CATEGORY_ARMOR  },
   { "Hoodie"           , ITEM_LEVEL_COMMON      , ITEM_CATEGORY_ARMOR  },
   { "Leather Jacket"   , ITEM_LEVEL_UNCOMMON    , ITEM_CATEGORY_ARMOR  },
   { "Hockey Helmet"    , ITEM_LEVEL_RARE        , ITEM_CATEGORY_ARMOR  },
   { "WWI Helmet"       , ITEM_LEVEL_EPIC        , ITEM_CATEGORY_ARMOR  },
   { "Bulletproof West" , ITEM_LEVEL_EPIC        , ITEM_CATEGORY_ARMOR  },
   { "SpecOps Helmet"   , ITEM_LEVEL_LEGENDARY   , ITEM_CATEGORY_ARMOR  },
   
   // health
   { "Old Used Plaster" , ITEM_LEVEL_OLD         , ITEM_CATEGORY_HEALTH },
   { "Asprin"           , ITEM_LEVEL_OLD         , ITEM_CATEGORY_HEALTH },
   { "Bottle of Water"  , ITEM_LEVEL_OLD         , ITEM_CATEGORY_HEALTH },
   { "Bandage"          , ITEM_LEVEL_COMMON      , ITEM_CATEGORY_HEALTH },
   { "Tournique"        , ITEM_LEVEL_UNCOMMON    , ITEM_CATEGORY_HEALTH },
   { "Painkillers"      , ITEM_LEVEL_RARE        , ITEM_CATEGORY_HEALTH },
   { "Adrenaline"       , ITEM_LEVEL_EPIC        , ITEM_CATEGORY_HEALTH },
   { "Adrenaline"       , ITEM_LEVEL_LEGENDARY   , ITEM_CATEGORY_HEALTH },
};

typedef struct 
{
   int item_start_index;
   int item_count;
   const item_desc_t *items[kBackpackItemCapacity];
} backpack_t;

static int
backpack_stat_summary(backpack_t *backpack, int category)
{
   const int item_count = backpack->item_count;

   int result = 0;
   for (int item_index = 0;
        item_index < item_count;
        item_index++)
   {
      const item_desc_t *desc = backpack->items[item_index];
      if (desc->category == category)
      {
         result += desc->level;
      }
   }

   return result;
}

static void 
backpack_add_item(backpack_t *backpack, const item_desc_t *desc)
{
   if (backpack->item_count < kBackpackItemCapacity)
   {
      backpack->items[backpack->item_count++] = desc;
   }
}

static void 
backpack_add_random_item(backpack_t *backpack)
{
   const int probability_count = ARRAYCOUNT(kItemLevelProbabilityTable);

   int weight_sum = 0;
   for (int i = 1; i < probability_count; i++)
   {
      weight_sum += kItemLevelProbabilityTable[i];
   }

   int random_value = random() % weight_sum;
   int level_index = -1;
   for (int i = 1; i < probability_count; i++)
   {
      if (random_value < kItemLevelProbabilityTable[i])
      {
         level_index = i;
         break;
      }

      random_value -= kItemLevelProbabilityTable[i];
   }
   ASSERT(level_index != -1);

   if (level_index > 2)
   {
      int asd = 0;
   }

   const int item_count = ARRAYCOUNT(kItemDesc);
   uint32_t item_of_level_count = 0;
   for (int i = 0; i < item_count; i++)
   {
      if (kItemDesc[i].level == level_index)
      {
         item_of_level_count++;
      }
   }
   ASSERT(item_of_level_count != 0);

   const item_desc_t *desc = NULL;
   uint32_t number = random();
   int index = (number & 0xffffff) % item_of_level_count;
   for (int i = 0; i < item_count; i++)
   {
      if (kItemDesc[i].level != level_index)
         continue;

      if (index == 0)
      {
         desc = kItemDesc + i; 
         break;
      }
      index--;
   }
   ASSERT(desc);

   backpack_add_item(backpack, desc);
}

static void 
backpack_scroll_list(backpack_t *backpack, int dir)
{
   int item_start_index = backpack->item_start_index;
   int last_item_index = backpack->item_count;
   int item_index_distance = backpack->item_count - backpack->item_start_index;

   if (last_item_index < kItemListHeight)
      last_item_index = 0;

   if (last_item_index > 0 && (item_index_distance - dir) >= kItemListHeight)
      item_start_index += dir;
   if (item_start_index < 0)
      item_start_index = 0;

   backpack->item_start_index = item_start_index;
}

static void
backpack_init(backpack_t *backpack, allocator_t *allocator)
{
   const item_desc_t **items = backpack->items;
   for (int item_index = 0;
        item_index < kBackpackItemCapacity;
        item_index++)
   {
      items[item_index] = NULL;
   }

   backpack->item_start_index = 0;
   backpack->item_count = 0;
}

static void 
backpack_update(backpack_t *backpack, const f32 dt)
{
}

static void 
backpack_draw(backpack_t *backpack, render_commands_t *render_commands)
{
   int item_name_column_x = 10;
   int item_stat_column_x = 160;
   int item_type_column_x = 180;
   int item_row_y = 50;

   int item_count = backpack->item_count;
   int item_limit_count = min(backpack->item_count, kItemListHeight);
   int item_start_index = backpack->item_start_index;
   int item_end_index = item_start_index + item_limit_count;

   draw_text_shadowed(render_commands, 
                      COLOR_AQUA, 
                      COLOR_BLACK,
                      item_name_column_x,
                      item_row_y,
                      1,
                      "INVENTORY (%d/%d)",
                      item_count,
                      kBackpackItemCapacity);
#if 0
   int attack_sum = backpack_stat_summary(backpack, ITEM_CATEGORY_WEAPON); 
   int defense_sum = backpack_stat_summary(backpack, ITEM_CATEGORY_ARMOR);
   int health_sum = backpack_stat_summary(backpack, ITEM_CATEGORY_HEALTH);
   
   draw_text_shadowed(render_commands, 
                      COLOR_AQUA, 
                      COLOR_BLACK,
                      item_name_column_x + 50,
                      item_row_y,
                      1,
                      "Atk: %d",
                      attack_sum);

   draw_text_shadowed(render_commands, 
                      COLOR_AQUA, 
                      COLOR_BLACK,
                      item_name_column_x + 90,
                      item_row_y,
                      1,
                      "Def: %d",
                      defense_sum, health_sum);

   draw_text_shadowed(render_commands, 
                      COLOR_AQUA, 
                      COLOR_BLACK,
                      item_name_column_x + 130,
                      item_row_y,
                      1,
                      "Hlth: %d",
                      health_sum);
#endif

   item_row_y += kNewLineAdvanceY + 2;

   for (int item_index = item_start_index; 
        item_index < item_end_index; 
        item_index++)
   {
      const item_desc_t *item = backpack->items[item_index];
      const int color_index = kItemLevelColorIndexTable[item->level]; 

      draw_text_shadowed(render_commands, 
                         color_index, 
                         COLOR_BLACK,
                         item_name_column_x,
                         item_row_y,
                         1,
                         item->name);
      draw_text_shadowed(render_commands, 
                         color_index, 
                         COLOR_BLACK,
                         item_stat_column_x,
                         item_row_y,
                         1,
                         "+%d",
                         item->level);

      draw_text_shadowed(render_commands, 
                         color_index, 
                         COLOR_BLACK,
                         item_type_column_x,
                         item_row_y,
                         1,
                         kItemCategoryNameTable[item->category]);

      item_row_y += kNewLineAdvanceY + 1;
   }

   // scrollbar ?
}
