#include <Novice.h>
#include "Enemy.h"
#include "Player.h"
#include "MapChip.h"

const char kWindowTitle[] = "LE2B_25_マツモト_ソウシ_TR1";



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	MapChip* mapChip = new MapChip();
	mapChip->LoadCSV("Resources/map.csv");

	Enemy* enemy = new Enemy();
	enemy->Initialize();
	enemy->SetMapChip(mapChip);

	Player* player = new Player();
	player->Initialize();


	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///


		player->Update();

		enemy->SetPlayerPosition(player->GetPosition());

		enemy->Update();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		mapChip->Draw();

		player->Draw();

		enemy->Draw();

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	delete enemy;
	delete player;
	delete mapChip;

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}