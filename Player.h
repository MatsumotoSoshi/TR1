#pragma once
#include "Vector.h"
#include "MapChip.h"

class Player
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


	Vector2 GetPosition() const { return position_; }

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner
	};

	Vector2 CornerPosition(const Vector2& center, Corner corner);

	struct CollisionInfo {
		Vector2 move;
		bool hitTop = false;
		bool hitBottom = false;
		bool hitLeft = false;
		bool hitRight = false;
	};

	void CollisionMapTop(CollisionInfo& info);
	void CollisionMapBottom(CollisionInfo& info);
	void CollisionMapLeft(CollisionInfo& info);
	void CollisionMapRight(CollisionInfo& info);

	void MoveResult(const CollisionInfo& info);

	void SetMapChip(MapChip* map) { mapChip_ = map; }

	void SetPosition(const Vector2& position) {
		position_ = position;
	}

private:
	Vector2 position_;
	float width_;
	float height_;
	float halfWidth_;
	float halfHeight_;

	MapChip* mapChip_ = nullptr;

};

