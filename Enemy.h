#pragma once
#include "Vector2.h"
#include "MapChip.h"

class Player;

class Enemy
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void UpdateParameter();

	void Vision();

	void DrawVision();

	void SetPlayerPosition(const Vector2& pos) {
		playerPosition_ = pos;
	}

	Vector2 GetPosition() const { return position_; }

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner
	};

	Vector2 CornerPosition(const Vector2& center, Corner corner);

	void SetMapChip(MapChip* map) { mapChip_ = map; }
	bool IsBlocked(const Vector2& start, const Vector2& end);

	enum class Behavior {
		// 行動フラグ
		kPatrol,     // 巡回
	    kDiscovery,  // 発見
	    kChasing,    // 追跡
	    kAlert       // 警戒
	};

	Behavior behavior_ = Behavior::kPatrol;

	void BehaviorPatrolUpdate();
	void BehaviorDiscpveryUpdate();
	void BehaviorChasingUpdate();
	void BehaviorAlertUpdate();

private:

	Vector2 position_;
	Vector2 velocity_;
	Vector2 acceleration_;
	Vector2 angle_;
	float speed_;
	int parameter_;     // パラメーター値
	int maxParameter_;  // 最大パラメーター値
	float width_;
	float height_;
	float halfWidth_;
	float halfHeight_;
	int color_;



	Vector2 playerPosition_; // プレイヤー座標
	float viewDistance_;     // 視野距離
	float viewAngle_;        // 視野角（ラジアン）
	float direction_;        // 向いてる方向（ラジアン）

	MapChip* mapChip_;

};

