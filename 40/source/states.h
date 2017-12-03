// states.h

enum GameState
{
  GAME_STATE_LOADING,
  GAME_STATE_START,
  GAME_STATE_PLAY,
  GAME_STATE_END,
};

static const char* game_state_string(GameState state)
{
  switch(state)
  {
    case GAME_STATE_LOADING: return "game_state_loading"; break;
    case GAME_STATE_START: return "game_state_start"; break;
    case GAME_STATE_PLAY: return "game_state_play"; break;
    case GAME_STATE_END: return "game_state_end"; break;
  }
  return "GAME_STATE_UNKNOWN";
}
