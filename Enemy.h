#pragma once
#include "Vector.h"
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

	void UpdateBehavior();

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
		kAlert,      // 警戒
	    kDiscovery,  // 発見
	    kChasing,    // 追跡
		kSerch,      // 探索
	}; 

	Behavior behavior_ = Behavior::kPatrol;
	Behavior nextBehavior_;

	void BehaviorPatrolInitialize();  // 巡回行動の
	void BehaviorAlertInitialize();  // 警戒行動の初期化
	void BehaviorChasingInitialize();  // 追跡行動の初期化
	void BehaviorPatrolUpdate();  // 巡回行動の更新
	void BehaviorAlertUpdate();  // 警戒行動の更新
	void BehaviorChasingUpdate();  // 追跡行動の更新

	void SetPosition(const Vector2& position) {
		position_ = position;
	}

	enum class MoveDirection {
		Up,
		Right,
		Down,
		Left
	};

	MoveDirection moveDirection_ = MoveDirection::Up;

	float GetDirection() const;

	bool UpdateTurn();
	void SetTargetPosition();
	void MovePatrol(float speed);
	void MoveAlert(float speed);
	void MoveChasing(float speed);

	void BuildPath(Vector2 start, Vector2 goal);

private:

	Vector2 position_;
	Vector2 velocity_;
	Vector2 acceleration_;
	Vector2 angle_;
	float speed_;
	float parameter_;     // パラメーター値
	float maxParameter_;  // 最大パラメーター値
	float width_;
	float height_;
	float halfWidth_;
	float halfHeight_;
	int color_;


	// 視界内かのフラグ
	bool isInView_ = false;

	Vector2 playerPosition_; // プレイヤー座標
	float viewDistance_;     // 視野距離
	float viewAngle_;        // 視野角（ラジアン）

	float lostTimer_ = 0.0f; // 見失ってからの経過時間

	MapChip* mapChip_;

	Vector2 targetPosition_;  // 目的地座標
	bool hasTarget_ = false;  // 目的地があるかどうか
	int turnTimer_ = 0;  // 回転タイマー
	const int kTurnTime = 60;  // 回転にかかる時間
	MoveDirection nextDirection_;  // 次の向き
	bool isTurning_;  // 回転中かどうか

	bool wasInView_ = false;

	float patrolSpeed_ = 1.0f;  // 巡回時の移動速度
	float alertSpeed_ = 2.5f;   // 警戒時の移動速度
	float chasingSpeed_ = 2.0f; // 追跡時の移動速度

	Vector2 lastSeenPlayerPos_;
	bool hasLastSeen_ = false;

	Vector2 moveStart_;
	Vector2 moveEnd_;

	std::vector<Vector2> path_;
	int pathIndex_ = 0;
	bool hasPath_ = false;

	float currentAngle_; // 現在角度
	float targetAngle_; // 目標角度
	float startAngle_; // 開始角度
	float rotateSpeed_;

	float discoverSpeed_;
	float playerDistance_;

	int pTextureHandle_;
};

