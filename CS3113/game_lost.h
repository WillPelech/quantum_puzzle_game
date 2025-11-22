#include "Scene.h"

class game_lost:public Scene
{
    public:
        game_lost();
        game_lost(Vector2 origin, const char* bgHexCode,const char* start_text);
        ~game_lost();
        void initialise() override;
        void update(float deltaTime) override;
        void render() override;
        void shutdown();
    private:
        std::string start_text;
};