#ifndef LEVELB_H
#define LEVELB_H

#include "Scene.h"

constexpr int LEVELB_WIDTH  = 14,
              LEVELB_HEIGHT = 8;

class LevelB : public Scene {
private:
    unsigned int mLevelData1[LEVELB_WIDTH * LEVELB_HEIGHT] = {
        11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
        11,  0,  0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0, 11,
        11,  0,  0,  0,  0,  0,  0, 0, 11, 0,  0,  0,  0, 11,
        11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 11,
        11,  0,  0,  0,  0,  0,  0,  0,  0,  0, 12, 12, 12, 11,
        11, 12, 12, 12,  0,  0,  0, 12, 12, 12, 12,  0 , 0, 11,
        11, 12,  0, 12,  0,  0,  0, 12, 0,  12,  12,  0,  12,11,
        11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 11
    };

    unsigned int mLevelData2[LEVELB_WIDTH * LEVELB_HEIGHT] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1,
        1, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1
    };
    std::map<Direction, std::vector<int>> mouseAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3 }},
      {LEFT,  {  8,  9, 10, 11 }},
      {UP,    { 4, 5, 6, 7 }},
      {RIGHT, { 12, 13, 14, 15 }},
   };
   std::map<Direction, std::vector<int>> ghostAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3 }},
      {LEFT,  {  4,  5, 6, 7 }},
      {UP,    { 12, 13, 14, 15 }},
      {RIGHT, { 8, 9, 10, 11 }},
   };
    Entity *key = nullptr;
    Entity *door = nullptr;
    Entity *real_enemy1 = nullptr;
    Entity *real_enemy2 = nullptr;
    Entity *real_enemy3 = nullptr;
    Entity *real_enemy4 = nullptr;
    Entity *real_enemy5 = nullptr;
    Entity *dead_enemy1 = nullptr;
    Entity *dead_enemy2 = nullptr;
    Entity *dead_enemy3 = nullptr;
    Entity *dead_enemy4 = nullptr;
    Entity *dead_enemy5 = nullptr;
    gworld prev_state = REAL;
    Map *mRealMap = nullptr;
    Map *mGhostMap = nullptr;
    Texture2D mBgReal;
    Texture2D mBgGhost;
    Texture2D mCurrentBg;
    std::vector<Entity*> mBullets;
    int num_fired = 0;
    float mFireCooldown = 0.25f;
    float mFireTimer = 0.0f;
    int counter_variable = 0;
    int mPrevLives = 0;
public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    LevelB();
    LevelB(Vector2 origin, const char *bgHexCode);
    ~LevelB();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void switch_worlds();
    void fire_bullet();
    void update_bullets(float deltaTime);
    void render_bullets();
    void switch_enemy_set();
    void render_enemies();
    void initialise_enemies();
    void update_enemies(float deltaTime);
    int get_num_alive();
    void particle_system(float x_pos, float y_pos);
};

#endif