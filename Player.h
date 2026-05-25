#pragma once
#include "Vector2.h"

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
	Vector2 GetCenter() const;


private:
	Vector2 position_;
	float width_;
	float height_;
	float halfWidth_;
	float halfHeight_;

};

