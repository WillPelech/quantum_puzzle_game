#include "Scene.h"

class start_menu : public Scene {
public:
    start_menu();
    start_menu(Vector2 origin, const char* bgHexCode);
    ~start_menu();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown();
};
