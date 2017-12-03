// audio.cc

enum SoundID
{
  SOUND_STEP,
  SOUND_SPAWN,
  SOUND_FINISH,
  SOUND_PICKUP,
  SOUND_SPIKE,
  SOUND_DEAD,
  SOUND_BONUS,
  SOUND_COUNT,
};

static const char* sound_id_string(SoundID id)
{
  static const char* names[]={"step","spawn","finish","pickup","spike","dead","bonus"};
  return names[id];
}

struct Audio
{
  int sounds[SOUND_COUNT];
};

bool init(Audio* audio)
{
  char filename[256];
  for (int i=0;i<SOUND_COUNT;i++)
  {
    sprintf_s<256>(filename,"assets/%s.ogg",sound_id_string((SoundID)i));
    audio->sounds[i]=bq_load_sound(filename);
    if (audio->sounds[i]==0) {return false;}
  }
  return true;
}

void play(const Audio* audio,const SoundID id,const float volume)
{
  bq_play_sound(audio->sounds[id],volume);
}
