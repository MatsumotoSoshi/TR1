#include "Enemy.h"
#include <Novice.h>
#include <imgui.h>
#include <cmath>
#include <algorithm>

const float kPlayerWidth = 20.0f;
const float kPlayerHeight = 20.0f;

void Enemy::Initialize(){
	position_ = { 640.0f, 320.0f };
	velocity_ = { 0.0f, 0.0f };
	acceleration_ = { 0.0f, 0.0f };
	angle_ = { 0.0f, 0.0f };
	speed_ = 0.0f;
	parameter_ = 0;
	maxParameter_ = 100;
	width_ = 20.0f;
	height_ = 20.0f;
	halfWidth_ = width_ / 2.0f;
	halfHeight_ = height_ / 2.0f;

	viewDistance_ = 250.0f;
	viewAngle_ = 3.14f / 8.0f; 
	direction_ = -3.14f / 2.0f;

	behavior_ = Behavior::kPatrol;

	mapChip_ = nullptr;
}

Vector2 Enemy::CornerPosition(const Vector2& center, Corner corner) {

	Vector2 offsetTable[kNumCorner] = {
		{ +halfWidth_, +halfHeight_ }, // 右下
		{ -halfWidth_, +halfHeight_ }, // 左下
		{ +halfWidth_, -halfHeight_ }, // 右上
		{ -halfWidth_, -halfHeight_ }  // 左上
	};

	Vector2 result;
	result.x = center.x + offsetTable[corner].x;
	result.y = center.y + offsetTable[corner].y;

	return result;
}

bool Enemy::IsBlocked(const Vector2& start, const Vector2& end) {

	Vector2 direction;
	direction.x = end.x - start.x;
	direction.y = end.y - start.y;

	float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

	if (length == 0.0f) {
		return false;
	}

	direction.x /= length;
	direction.y /= length;

	int kChipSize = mapChip_->GetChipSize();

	float step = kChipSize / 4.0f;

	for (float t = 0; t < length; t += step) {

		float x = start.x + direction.x * t;
		float y = start.y + direction.y * t;

		int gridX = (int)(x / kChipSize);
		int gridY = (int)(y / kChipSize);

		if (mapChip_->IsBlock(gridX, gridY)) {
			return true;
		}
	}

	return false;
}

void Enemy::Vision() {

	Vector2 enemyCenter = {
		position_.x + halfWidth_,
		position_.y + halfHeight_
	};

	Vector2 playerCenter = {
		playerPosition_.x + kPlayerWidth / 2,
		playerPosition_.y + kPlayerHeight / 2
	};

	// 視界内かのフラグ
	bool isInView = false;

	Vector2 toPlayer;
	toPlayer.x = playerCenter.x - enemyCenter.x;
	toPlayer.y = playerCenter.y - enemyCenter.y;

	// プレイヤーとの距離
	float distance = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

	if (distance <= viewDistance_ && distance > 0.0f) {

		toPlayer.x /= distance;
		toPlayer.y /= distance;

		Vector2 forward = { cosf(direction_), sinf(direction_) };

		float dot = forward.x * toPlayer.x + forward.y * toPlayer.y;
		float cosFov = cosf(viewAngle_);

		if (dot > cosFov) {

			// 壁判定
			if (!IsBlocked(enemyCenter, playerCenter)) {
				isInView = true;
			}
		}
	}

	// パラメータ処理
	if (isInView) {
		parameter_ += 2;
	} else {
		parameter_ -= 1;
	}

	parameter_ = std::clamp(parameter_, 0, maxParameter_);
}

void Enemy::UpdateParameter() {
	if (parameter_ < 30.0f) {
		behavior_ = Behavior::kPatrol;
	} else if (parameter_ < 80.0f) {
			behavior_ = Behavior::kAlert;
	} else {
			behavior_ = Behavior::kDiscovery;
	}

	switch (behavior_) {
	case Behavior::kPatrol: {
		color_ = 0x00FF00FF;
		break;
	}

	case Behavior::kAlert: {
		color_ = 0xFFFF00FF;
		break;
	}

	case Behavior::kDiscovery: {
		color_ = 0xFF0000FF;
		break;
	}
	}

}

void Enemy::Update() {
	Vision();
	UpdateParameter();

	ImGui::SliderFloat2("Position", &position_.x, 0.0f, 1280.0f);
	ImGui::SliderInt("Parameter", &parameter_, 0, maxParameter_);

}

void Enemy::DrawVision() {

	const int kDivision = 40;

	float startAngle = direction_ - viewAngle_;
	float endAngle = direction_ + viewAngle_;

	Vector2 center = {
		position_.x + halfWidth_,
		position_.y + halfHeight_
	};

	for (int i = 0; i < kDivision; i++) {

		float angle1 = startAngle + (endAngle - startAngle) * i / kDivision;
		float angle2 = startAngle + (endAngle - startAngle) * (i + 1) / kDivision;

		Vector2 position[3];
		position[0] = center;

		position[1] = {
			center.x + cosf(angle1) * viewDistance_,
			center.y + sinf(angle1) * viewDistance_
		};

		position[2] = {
			center.x + cosf(angle2) * viewDistance_,
			center.y + sinf(angle2) * viewDistance_
		};

		// 扇描画
		Novice::DrawTriangle(
			(int)position[0].x, (int)position[0].y,
			(int)position[1].x, (int)position[1].y,
			(int)position[2].x, (int)position[2].y,
			0xFFFFFFAA,
			kFillModeSolid
		);
	}
}

void Enemy::Draw() {
	Novice::DrawBox(
		(int)position_.x, (int)position_.y,
		(int)width_, (int)height_,
		0.0f,
		color_,
		kFillModeSolid
	);
	DrawVision();

}