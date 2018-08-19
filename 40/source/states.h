// states.h

enum GameState
{
  GAME_STATE_LOADING,
  GAME_STATE_MENU,
  GAME_STATE_OPTIONS,
  GAME_STATE_PLAY,
  GAME_STATE_END,
  GAME_STATE_EDITOR,
};

static const char* game_state_string(GameState state)
{
  switch(state)
  {
    case GAME_STATE_LOADING: return "loading"; break;
    case GAME_STATE_MENU: return "menu"; break;
    case GAME_STATE_OPTIONS: return "options"; break;
    case GAME_STATE_PLAY: return "play"; break;
    case GAME_STATE_END: return "end"; break;
    case GAME_STATE_EDITOR: return "editor"; break;
  }
  return "unknown";
}
