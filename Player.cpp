#include "Player.h"
#include <Novice.h>

void Player::Initialize() {
	position_ = { 300.0f, 300.0f };
	width_ = 20.0f;
	height_ = 20.0f;
	halfWidth_ = width_ / 2.0f;
	halfHeight_ = height_ / 2.0f;
}

Vector2 Player::CornerPosition(const Vector2& center, Corner corner) {

	Vector2 offsetTable[kNumCorner] = {
		{ +halfWidth_, +halfHeight_ }, // 右下
		{ -halfWidth_, +halfHeight_ }, // 左下
		{ +halfWidth_, -halfHeight_ }, // 右上
		{ -halfWidth_, -halfHeight_ }  // 左上
	};

	Vector2 result;
	result.x = center.x + offsetTable[(int)corner].x;
	result.y = center.y + offsetTable[(int)corner].y;

	return result;
}

Vector2 Player::GetCenter() const {
	return {
		position_.x + halfWidth_,
		position_.y + halfHeight_
	};
}

void Player::Update() {
	if (Novice::CheckHitKey(DIK_W)) {
		position_.y -= 5.0f;
	}

	if (Novice::CheckHitKey(DIK_S)) {
		position_.y += 5.0f;
	}

	if (Novice::CheckHitKey(DIK_A)) {
		position_.x -= 5.0f;
	}

	if (Novice::CheckHitKey(DIK_D)) {
		position_.x += 5.0f;
	}
}

void Player::Draw() {
	Novice::DrawBox(
		(int)position_.x,
		(int)position_.y,
		(int)width_,
		(int)height_,
		0.0f,
		0xFFFFFFFF,
		kFillModeSolid
	);
}