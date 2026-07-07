#pragma once
#include "Player.h"
#include "Enemy.h"
#include "MapChip.h"

class GameScene {
public:
    void Initialize();
    void Update();
    void Draw();
    ~GameScene();

private:
    Player* player_ = nullptr;
    Enemy* enemy_ = nullptr;
    MapChip* mapChip_ = nullptr;
};