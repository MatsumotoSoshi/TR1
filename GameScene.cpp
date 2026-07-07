#include "GameScene.h"


void GameScene::Initialize() {
	mapChip_ = new MapChip();
	mapChip_->LoadCSV("Resources/map.csv");

	player_ = new Player();
	player_->Initialize();
	player_->SetMapChip(mapChip_);

	enemy_ = new Enemy();
	enemy_->Initialize();
	enemy_->SetMapChip(mapChip_);


	// CSVからスポーン位置取得
	player_->SetPosition(mapChip_->GetPlayerSpawnPosition());
	enemy_->SetPosition(mapChip_->GetEnemySpawnPosition());
}

void GameScene::Update() {
	player_->Update();
	enemy_->SetPlayerPosition(player_->GetPosition());
	enemy_->Update();
}

void GameScene::Draw() {
	mapChip_->Draw();
	player_->Draw();
	enemy_->Draw();
}

GameScene::~GameScene() {
	delete player_;
	delete enemy_;
	delete mapChip_;
}