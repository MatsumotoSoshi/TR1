#include "Player.h"
#include <Novice.h>

void Player::Initialize() {
	position_ = { 0.0f, 0.0f };
	width_ = 20.0f;
	height_ = 20.0f;
	halfWidth_ = width_ / 2.0f;
	halfHeight_ = height_ / 2.0f;
}

Vector2 Player::CornerPosition(const Vector2& center, Corner corner) {

	Vector2 offsetTable[kNumCorner] = {
		{ +halfWidth_ - 1.0f, +halfHeight_ - 1.0f }, // 右下
		{ -halfWidth_,        +halfHeight_ - 1.0f }, // 左下
		{ +halfWidth_ - 1.0f, -halfHeight_ },        // 右上
		{ -halfWidth_,        -halfHeight_ }         // 左上
	};

	Vector2 result;
	result.x = center.x + offsetTable[(int)corner].x;
	result.y = center.y + offsetTable[(int)corner].y;

	return result;
}


void Player::CollisionMapTop(CollisionInfo& info) {

	if (info.move.y >= 0.0f) {
		return;
	}

	Vector2 nextCenter = {
		position_.x + info.move.x,
		position_.y + info.move.y
	};

	Vector2 leftTop = CornerPosition(nextCenter, kLeftTop);
	Vector2 rightTop = CornerPosition(nextCenter, kRightTop);

	int chip = mapChip_->GetChipSize();

	if (mapChip_->IsBlock(
		(int)leftTop.x / chip,
		(int)leftTop.y / chip) ||

		mapChip_->IsBlock(
			(int)rightTop.x / chip,
			(int)rightTop.y / chip)) {

		info.move.y = 0.0f;
		info.hitTop = true;
	}
}

void Player::CollisionMapBottom(CollisionInfo& info) {

	if (info.move.y <= 0.0f) {
		return;
	}

	Vector2 nextCenter = {
		position_.x + info.move.x,
		position_.y + info.move.y
	};

	Vector2 leftBottom = CornerPosition(nextCenter, kLeftBottom);
	Vector2 rightBottom = CornerPosition(nextCenter, kRightBottom);

	int chip = mapChip_->GetChipSize();

	if (mapChip_->IsBlock(
		(int)leftBottom.x / chip,
		(int)(leftBottom.y) / chip) ||

		mapChip_->IsBlock(
			(int)rightBottom.x / chip,
			(int)(rightBottom.y) / chip)) {

		info.move.y = 0.0f;
		info.hitBottom = true;
	}
}

void Player::CollisionMapLeft(CollisionInfo& info) {

	if (info.move.x >= 0.0f) {
		return;
	}

	Vector2 nextCenter = {
		position_.x + info.move.x,
		position_.y + info.move.y
	};

	Vector2 leftTop = CornerPosition(nextCenter, kLeftTop);
	Vector2 leftBottom = CornerPosition(nextCenter, kLeftBottom);

	int chip = mapChip_->GetChipSize();

	if (mapChip_->IsBlock(
		(int)leftTop.x / chip,
		(int)leftTop.y / chip) ||

		mapChip_->IsBlock(
			(int)leftBottom.x / chip,
			(int)leftBottom.y / chip)) {

		info.move.x = 0.0f;
		info.hitLeft = true;
	}
}

void Player::CollisionMapRight(CollisionInfo& info) {

	if (info.move.x <= 0.0f) {
		return;
	}

	Vector2 nextCenter = {
		position_.x + info.move.x,
		position_.y + info.move.y
	};

	Vector2 rightTop = CornerPosition(nextCenter, kRightTop);
	Vector2 rightBottom = CornerPosition(nextCenter, kRightBottom);

	int chip = mapChip_->GetChipSize();

	if (mapChip_->IsBlock(
		(int)(rightTop.x) / chip,
		(int)rightTop.y / chip) ||

		mapChip_->IsBlock(
			(int)(rightBottom.x) / chip,
			(int)rightBottom.y / chip)) {

		info.move.x = 0.0f;
		info.hitRight = true;
	}
}

void Player::MoveResult(const CollisionInfo& info) {
	position_.x += info.move.x;
	position_.y += info.move.y;
}

void Player::Update() {
	CollisionInfo info{};

	if (Novice::CheckHitKey(DIK_W)) {
		info.move.y = -5.0f;
	}

	if (Novice::CheckHitKey(DIK_S)) {
		info.move.y = 5.0f;
	}

	if (Novice::CheckHitKey(DIK_A)) {
		info.move.x = -5.0f;
	}

	if (Novice::CheckHitKey(DIK_D)) {
		info.move.x = 5.0f;
	}

	CollisionMapTop(info);
	CollisionMapBottom(info);
	CollisionMapLeft(info);
	CollisionMapRight(info);

	MoveResult(info);
}

void Player::Draw() {
	Novice::DrawBox(
		(int)(position_.x - halfWidth_),
		(int)(position_.y - halfHeight_),
		(int)width_,
		(int)height_,
		0.0f,
		0xFFFFFFFF,
		kFillModeSolid
	);
}