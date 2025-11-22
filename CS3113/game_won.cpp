#include "game_won.h"

game_won::game_won() :Scene{ {0.0f},nullptr}{}
game_won::game_won(Vector2 origin, const char *bgHexCode, const char* start_text)
    : Scene { origin, bgHexCode }, 
    start_text(start_text)
{
}

game_won::~game_won() { shutdown(); }

void game_won::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/music/TheEntertainer.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   float sizeRatio  = 48.0f / 64.0f;
}

void game_won::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
    
   if (IsKeyPressed(KEY_ENTER))
   {
      mGameState.nextSceneID = 5;
   }
}

void game_won::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   int fontSize = 40;
   int x = 50;
   int y = 300;
   DrawText(start_text.c_str(), x, y, fontSize,GREEN);
}

void game_won::shutdown()
{
   StopMusicStream(mGameState.bgm);
   UnloadMusicStream(mGameState.bgm);
}