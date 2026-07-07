#include "Enemy.h"
#include <Novice.h>
#include <imgui.h>
#include <cmath>
#include <algorithm>
#include "MathUtility.h"
#include <queue>
#include <map>
#include <set>
#include <random>
#include <ctime>

const float kPlayerWidth = 20.0f;
const float kPlayerHeight = 20.0f;

void Enemy::Initialize(){
	position_ = { 0.0f, 0.0f };
	velocity_ = { 0.0f, 0.0f };
	acceleration_ = { 0.0f, 0.0f };
	angle_ = { 0.0f, 0.0f };
	speed_ = 0.0f;
	parameter_ = 0.0f;
	maxParameter_ = 100.0f	;
	width_ = 20.0f;
	height_ = 20.0f;
	halfWidth_ = width_ / 2.0f;
	halfHeight_ = height_ / 2.0f;

	viewDistance_ = 250.0f;
	viewAngle_ = 3.14f / 8.0f; 
	moveDirection_ = MoveDirection::Up;

	behavior_ = Behavior::kPatrol;
	nextBehavior_ = behavior_;

	lostTimer_ = 0.0f;

	currentAngle_ = -3.14159265f / 2.0f;
	targetAngle_ = currentAngle_;
	rotateSpeed_ = 0.2f;

	discoverSpeed_ = 0.0f;
	playerDistance_ = 5000.0f;

	path_.clear();
	pathIndex_ = 0;
	hasPath_ = false;

	mapChip_ = nullptr;

	color_ = 0x00FF00FF;

	pTextureHandle_ = Novice::LoadTexture("./NoviceResources/white1x1.png");
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

	isInView_ = false;

	Vector2 enemyCenter = {
		position_.x + halfWidth_,
		position_.y + halfHeight_
	};

	Vector2 playerCenter = {
		playerPosition_.x + kPlayerWidth / 2,
		playerPosition_.y + kPlayerHeight / 2
	};

	Vector2 toPlayer;
	toPlayer.x = playerCenter.x - enemyCenter.x;
	toPlayer.y = playerCenter.y - enemyCenter.y;

	// プレイヤーとの距離
	float distance = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

	playerDistance_ = distance;

	// 視覚内か？
	if (distance <= viewDistance_) {

		// 単位ベクトル化
		toPlayer.x /= distance;
		toPlayer.y /= distance;

		float direction = GetDirection();

		Vector2 forward = {
			cosf(direction),
			sinf(direction)
		};

		// 角度の判定
		float dot = forward.x * toPlayer.x + forward.y * toPlayer.y;
		float cosFov = cosf(viewAngle_);

		if (dot > cosFov) {

			// 遮蔽物の判定
			if (!IsBlocked(enemyCenter, playerCenter)) {
				isInView_ = true;
			}
		}

		// 視界内なら最後に見た位置を更新
		if (isInView_) {
			lastSeenPlayerPos_ = playerCenter;
			hasLastSeen_ = true;
		} 
	}

	// 見えていなければ遠距離扱い
	if (!isInView_) {
		playerDistance_ = 5000.0f;
	}

	if (playerDistance_ <= viewDistance_ * 0.2f) {
		discoverSpeed_ = 6.0f;
	} else if (playerDistance_ <= viewDistance_ * 0.4f) {
		discoverSpeed_ = 4.0f;
	} else if (playerDistance_ <= viewDistance_ * 0.7f) {
		discoverSpeed_ = 2.0f;
	} else {
		discoverSpeed_ = 1.0f;
	}

}

float Enemy::GetDirection() const {
	return currentAngle_;
}

bool Enemy::UpdateTurn() {

	if (!isTurning_) {
		return false;
	}

	// 回転時間中
	if (turnTimer_ > 0) {

		float t = 1.0f - (float)turnTimer_ / (float)kTurnTime;

		currentAngle_ = startAngle_ + (targetAngle_ - startAngle_) * t;

		turnTimer_--;
		return true;
	}

	// 回転終了
	currentAngle_ = targetAngle_;

	moveDirection_ = nextDirection_;
	isTurning_ = false;

	SetTargetPosition();

	return false;
}

void Enemy::SetTargetPosition() {
	int chipSize = mapChip_->GetChipSize();

	int mapX = (int)((position_.x + halfWidth_) / chipSize);
	int mapY = (int)((position_.y + halfHeight_) / chipSize);

	switch (moveDirection_) {

	case MoveDirection::Up:
		targetPosition_ = {
			mapX * chipSize + chipSize / 2.0f - halfWidth_,
			(mapY - 1) * chipSize + chipSize / 2.0f - halfHeight_
		};
		break;

	case MoveDirection::Right:
		targetPosition_ = {
			(mapX + 1) * chipSize + chipSize / 2.0f - halfWidth_,
			mapY * chipSize + chipSize / 2.0f - halfHeight_
		};
		break;

	case MoveDirection::Down:
		targetPosition_ = {
			mapX * chipSize + chipSize / 2.0f - halfWidth_,
			(mapY + 1) * chipSize + chipSize / 2.0f - halfHeight_
		};
		break;

	case MoveDirection::Left:
		targetPosition_ = {
			(mapX - 1) * chipSize + chipSize / 2.0f - halfWidth_,
			mapY * chipSize + chipSize / 2.0f - halfHeight_
		};
		break;
	}

	hasTarget_ = true;
}

void Enemy::BuildPath(Vector2 start, Vector2 goal) {
	struct Node {
		int x, y;
	};

	int chip = mapChip_->GetChipSize();

	int startX = (int)((start.x + halfWidth_) / chip);
	int startY = (int)((start.y + halfHeight_) / chip);

	int goalX = (int)((goal.x + halfWidth_) / chip);
	int goalY = (int)((goal.y + halfHeight_) / chip);

	std::queue<Node> q;
	std::map<std::pair<int, int>, std::pair<int, int>> parent;
	std::set<std::pair<int, int>> visited;

	q.push({ startX, startY });
	visited.insert({ startX, startY });

	int dirs[4][2] = {
		{1,0},{-1,0},{0,1},{0,-1}
	};

	bool found = false;

	while (!q.empty()) {
		Node cur = q.front();
		q.pop();

		if (cur.x == goalX && cur.y == goalY) {
			found = true;
			break;
		}

		for (auto& d : dirs) {
			int nx = cur.x + d[0];
			int ny = cur.y + d[1];

			if (mapChip_->IsBlock(nx, ny)) continue;
			if (visited.count({ nx, ny })) continue;

			visited.insert({ nx, ny });
			parent[{nx, ny}] = { cur.x, cur.y };
			q.push({ nx, ny });
		}
	}

	if (!found) return;

	std::vector<Vector2> rev;
	std::pair<int, int> cur = { goalX, goalY };

	while (cur.first != startX || cur.second != startY) {
		rev.push_back({
			cur.first * chip + chip / 2.0f - halfWidth_,
			cur.second * chip + chip / 2.0f - halfHeight_
			});

		cur = parent[cur];
	}

	std::reverse(rev.begin(), rev.end());

	path_ = rev;
	pathIndex_ = 0;
	hasPath_ = true;
}

void Enemy::MovePatrol(float speed) {
	Vector2 diff = {
		targetPosition_.x - position_.x,
		targetPosition_.y - position_.y
	};

	float length = sqrtf(diff.x * diff.x + diff.y * diff.y);

	if (length == 0.0f) {
		hasTarget_ = false;
		return;
	}

	// 正規化
	diff.x /= length;
	diff.y /= length;

	float move = min(speed, length);

	Vector2 nextPos = {
		position_.x + diff.x * move,
		position_.y + diff.y * move
	};

	if (IsBlocked(position_, nextPos)) {
		hasTarget_ = false; // ここで止める
		return;
	}

	position_ = nextPos;

	// 到達
	if (move == length) {
		hasTarget_ = false;
	}
}

void Enemy::MoveAlert(float speed) {

	if (!hasPath_) {
		return;
	}

	if (pathIndex_ >= path_.size()) {
		hasPath_ = false;
		return;
	}

	Vector2 target = path_[pathIndex_];

	Vector2 diff = {
		target.x - position_.x,
		target.y - position_.y
	};

	float len = sqrtf(diff.x * diff.x + diff.y * diff.y);

	if (len < 2.0f) {
		pathIndex_++;
		return;
	}

	// 目標角度
	float targetAngle = atan2f(diff.y, diff.x);

	float delta = targetAngle - currentAngle_;

	while (delta > 3.14159265f) {
		delta -= 2.0f * 3.14159265f;
	}

	while (delta < -3.14159265f) {
		delta += 2.0f * 3.14159265f;
	}

	// 向きを補間
	currentAngle_ += delta * rotateSpeed_;

	position_.x += cosf(currentAngle_) * speed;
	position_.y += sinf(currentAngle_) * speed;
}

void Enemy::MoveChasing(float speed) {

	if (!hasPath_) {
		return;
	}

	if (pathIndex_ >= path_.size()) {
		hasPath_ = false;
		return;
	}

	Vector2 target = path_[pathIndex_];

	Vector2 diff = {
		target.x - position_.x,
		target.y - position_.y
	};

	float len = sqrtf(diff.x * diff.x + diff.y * diff.y);

	if (len < 2.0f) {
		pathIndex_++;
		return;
	}

	// 目標角度
	float targetAngle = atan2f(diff.y, diff.x);

	float delta = targetAngle - currentAngle_;

	while (delta > 3.14159265f) {
		delta -= 2.0f * 3.14159265f;
	}

	while (delta < -3.14159265f) {
		delta += 2.0f * 3.14159265f;
	}

	// 向きを補間
	currentAngle_ += delta * rotateSpeed_;

	position_.x += cosf(currentAngle_) * speed;
	position_.y += sinf(currentAngle_) * speed;
}

void Enemy::BehaviorPatrolUpdate() {

	// 回転中なら回転処理だけ
	if (UpdateTurn()) {

		if (isInView_) {
			parameter_ += discoverSpeed_;
		} else {
			parameter_ -= 0.5f;
		}

		parameter_ = std::clamp(parameter_, 0.0f, maxParameter_);

		if (parameter_ >= 30.0f) {
			nextBehavior_ = Behavior::kAlert;
		}

		return;
	}

	// 次の目的地を決める
	if (!hasTarget_) {

		int chipSize = mapChip_->GetChipSize();

		int mapX = (int)((position_.x + halfWidth_) / chipSize);
		int mapY = (int)((position_.y + halfHeight_) / chipSize);

		std::vector<MoveDirection> candidates;

		switch (moveDirection_) {

		case MoveDirection::Up:
			if (!mapChip_->IsBlock(mapX, mapY - 1)) {
				candidates.push_back(MoveDirection::Up);
			}

			if (!mapChip_->IsBlock(mapX - 1, mapY)) {
				candidates.push_back(MoveDirection::Left);
			}

			if (!mapChip_->IsBlock(mapX + 1, mapY)) {
				candidates.push_back(MoveDirection::Right);
			}

			break;

		case MoveDirection::Right:
			if (!mapChip_->IsBlock(mapX + 1, mapY)) {
				candidates.push_back(MoveDirection::Right);
			}

			if (!mapChip_->IsBlock(mapX, mapY - 1)) {
				candidates.push_back(MoveDirection::Up);
			}

			if (!mapChip_->IsBlock(mapX, mapY + 1)) {
				candidates.push_back(MoveDirection::Down);
			}

			break;

		case MoveDirection::Down:
			if (!mapChip_->IsBlock(mapX, mapY + 1)) {
				candidates.push_back(MoveDirection::Down);
			}

			if (!mapChip_->IsBlock(mapX + 1, mapY)) {
				candidates.push_back(MoveDirection::Right);
			}

			if (!mapChip_->IsBlock(mapX - 1, mapY)) {
				candidates.push_back(MoveDirection::Left);
			}

			break;

		case MoveDirection::Left:
			if (!mapChip_->IsBlock(mapX - 1, mapY)) {
				candidates.push_back(MoveDirection::Left);
			}

			if (!mapChip_->IsBlock(mapX, mapY + 1)) {
				candidates.push_back(MoveDirection::Down);
			}

			if (!mapChip_->IsBlock(mapX, mapY - 1)) {
				candidates.push_back(MoveDirection::Up);
			}

			break;
		}

		// ランダム選択
		if (!candidates.empty()) {

			static std::mt19937 rng((unsigned)time(nullptr));
			std::uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);

			nextDirection_ = candidates[dist(rng)];

		} else {
			nextDirection_ = moveDirection_;
		}

		// 向きが変わるなら回転開始
		if (nextDirection_ != moveDirection_) {

			startAngle_ = currentAngle_;

			switch (nextDirection_) {

			case MoveDirection::Up:
				targetAngle_ = -3.14159265f / 2.0f;
				break;

			case MoveDirection::Right:
				targetAngle_ = 0.0f;
				break;

			case MoveDirection::Down:
				targetAngle_ = 3.14159265f / 2.0f;
				break;

			case MoveDirection::Left:
				targetAngle_ = 3.14159265f;
				break;
			}

			isTurning_ = true;
			turnTimer_ = kTurnTime;
			return;
		}

		// 同じ向きならそのまま移動
		moveDirection_ = nextDirection_;
		SetTargetPosition();
	}

	// ターゲットへ移動
	MovePatrol(patrolSpeed_);

	if (viewDistance_ >=  250.0f) {
		viewDistance_ -= 0.5f;
	}

	if (viewAngle_ >= 0.3927f) {  // 22.5度
		viewAngle_ -= 0.01f;
	}

	// パラメータ処理
	if (isInView_) {
		parameter_ += discoverSpeed_;
	} else {
		parameter_ -= 0.5f;
	}

	parameter_ = std::clamp(parameter_, 0.0f, maxParameter_);

	if (parameter_ >= 30.0f) {
		nextBehavior_ = Behavior::kAlert;
		return;
	}

	color_ = 0x00FF00FF;
}

void Enemy::BehaviorAlertUpdate() {

	if (viewDistance_ < 300.0f) {
		viewDistance_ += 0.5f;
	}

	if (viewAngle_ < 0.7854f) {  // 45度
		viewAngle_ += 0.01f;
	}

	if (hasLastSeen_) {

		if (!hasPath_) {
			BuildPath(position_, lastSeenPlayerPos_);
		}

		MoveAlert(alertSpeed_);
	}

	if (isInView_) {
		parameter_ += discoverSpeed_ * 1.5f;

	} else {
		parameter_ -= 0.5f;

	}

	parameter_ = std::clamp(parameter_, 0.0f, maxParameter_);

	// 完全発見
	if (parameter_ >= 100.0f) {
		hasPath_ = false;
		nextBehavior_ = Behavior::kChasing;
		return;
	}

	// 見失って目的地にも到着
	if (!hasPath_ && !isInView_) {

		nextBehavior_ = Behavior::kPatrol;
		return;
	}

	color_ = 0xFFFF00FF;
}

void Enemy::BehaviorChasingUpdate() {

	if (isInView_) {
		lostTimer_ = 0;

		// 常に現在位置を記録
		lastSeenPlayerPos_ = {
			playerPosition_.x + kPlayerWidth / 2,
			playerPosition_.y + kPlayerHeight / 2
		};

		// 毎フレーム最新ルートを作る
		BuildPath(position_, lastSeenPlayerPos_);

	} else {
		lostTimer_++;

		// 見失っても最後に見た場所までは向かう
		if (lostTimer_ >= 120) {

			parameter_ = 30.0f;
			nextBehavior_ = Behavior::kAlert;
			return;
		}
	}

	MoveChasing(chasingSpeed_);

	color_ = 0xFF0000FF;
}

void Enemy::UpdateBehavior() {
	// 行動の更新
	switch (behavior_) {
	case Behavior::kPatrol: {
		BehaviorPatrolUpdate();
		break;
	}

	case Behavior::kAlert: {
		BehaviorAlertUpdate();
		break;
	}

	case Behavior::kChasing: {
		BehaviorChasingUpdate();
		break;
	}
	}

	// 状態が変わるならここで一括処理
	if (behavior_ != nextBehavior_) {

		behavior_ = nextBehavior_;

		hasPath_ = false;
		path_.clear();
		pathIndex_ = 0;
		hasTarget_ = false;
		hasLastSeen_ = false;
	}
}

void Enemy::Update() {

	Vision();
	UpdateBehavior();

	ImGui::SliderFloat2("Position", &position_.x, 0.0f, 1280.0f);
	ImGui::SliderFloat("Parameter", &parameter_, 0.0f, maxParameter_);

}

void Enemy::DrawVision() {

	const int kDivision = 40;

	float direction = GetDirection();

	float startAngle = direction - viewAngle_;
	float endAngle = direction + viewAngle_;

	Vector2 center = {
		position_.x + halfWidth_,
		position_.y + halfHeight_
	};

	// 分割して描画
	for (int i = 0; i < kDivision; i++) {

		// 開始角度と終了角度
		float angle1 = startAngle + (endAngle - startAngle) * i / kDivision;
		float angle2 = startAngle + (endAngle - startAngle) * (i + 1) / kDivision;

		// 3頂点座標
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

	Vector2 center = {
		position_.x + halfWidth_,
		position_.y + halfHeight_
	};

	// 四隅（中心基準）
	Vector2 lt = { -halfWidth_, -halfHeight_ };
	Vector2 rt = { halfWidth_, -halfHeight_ };
	Vector2 lb = { -halfWidth_,  halfHeight_ };
	Vector2 rb = { halfWidth_,  halfHeight_ };

	auto Rotate = [&](Vector2 p)
		{
			Vector2 result;

			result.x = p.x * cosf(currentAngle_) - p.y * sinf(currentAngle_);
			result.y = p.x * sinf(currentAngle_) + p.y * cosf(currentAngle_);

			result.x += center.x;
			result.y += center.y;

			return result;
		};

	lt = Rotate(lt);
	rt = Rotate(rt);
	lb = Rotate(lb);
	rb = Rotate(rb);

	Novice::DrawQuad(
		(int)lt.x, (int)lt.y,
		(int)rt.x, (int)rt.y,
		(int)lb.x, (int)lb.y,
		(int)rb.x, (int)rb.y,
		0, 0,
		1, 1,
		pTextureHandle_,
		color_
	);

	DrawVision();
}