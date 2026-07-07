#include "MapChip.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <map>
#include <Novice.h>

namespace {

	std::map<char, MapChipType> mapChipTypeTable = {
	{'0', MapChipType::kBlank},
	{'B', MapChipType::kBlock},
	{'P', MapChipType::kPlayer},
	{'E', MapChipType::kEnemy},
	};

}

MapChipType MapChip::GetChipType(int x, int y) const {

	if (x < 0 || x >= kMapWidth) {
		return MapChipType::kBlank;
	}

	if (y < 0 || y >= kMapHeight) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[y][x].type;
}

Vector2 MapChip::GetPlayerSpawnPosition() const {

	for (int y = 0; y < kMapHeight; y++) {
		for (int x = 0; x < kMapWidth; x++) {

			if (mapChipData_.data[y][x].type == MapChipType::kPlayer &&
				mapChipData_.data[y][x].subID == 0) {

				return {
	                x * kChipSize + kChipSize * 0.5f,
	                y * kChipSize + kChipSize * 0.5f
				};
			}
		}
	}

	return { 0.0f, 0.0f };
}

Vector2 MapChip::GetEnemySpawnPosition() const {

	for (int y = 0; y < kMapHeight; y++) {
		for (int x = 0; x < kMapWidth; x++) {

			if (mapChipData_.data[y][x].type == MapChipType::kEnemy &&
				mapChipData_.data[y][x].subID == 0) {

				return {
					x * kChipSize + kChipSize * 0.5f,
					y * kChipSize + kChipSize * 0.5f
				};
			}
		}
	}

	return { 0.0f, 0.0f };
}

void MapChip::LoadCSV(const std::string& filePath) {

	mapChipData_.data.clear();
	mapChipData_.data.resize(kMapHeight);

	for (auto& line : mapChipData_.data) {
		line.resize(kMapWidth);
	}

	std::ifstream file(filePath);
	assert(file.is_open());

	std::string line;

	for (int y = 0; y < kMapHeight; y++) {

		std::getline(file, line);
		std::stringstream ss(line);

		for (int x = 0; x < kMapWidth; x++) {

			std::string word;
			std::getline(ss, word, ',');

			if (word.empty()) {
				continue;
			}

			if (mapChipTypeTable.find(word[kChipType]) == mapChipTypeTable.end()) {
				continue;
			}

			mapChipData_.data[y][x].type =
				mapChipTypeTable[word[kChipType]];

			if (word.size() > kChipSubID) {
				mapChipData_.data[y][x].subID =
					static_cast<uint8_t>(word[kChipSubID] - '0');
			}
		}
	}
}

bool MapChip::IsBlock(int x, int y) const {

	if (x < 0 || x >= kMapWidth) {
		return true;
	}

	if (y < 0 || y >= kMapHeight) {
		return true;
	}

	return mapChipData_.data[y][x].type == MapChipType::kBlock;
}

void MapChip::Draw() {

	for (int y = 0; y < mapChipData_.data.size(); y++) {
		for (int x = 0; x < mapChipData_.data[y].size(); x++) {

			if (mapChipData_.data[y][x].type == MapChipType::kBlock) {

				int drawX = x * kChipSize;
				int drawY = y * kChipSize;

				Novice::DrawBox(
					drawX,
					drawY,
					kChipSize,
					kChipSize,
					0.0f,
					0x808080FF,
					kFillModeSolid
				);
			}
		}
	}

	int width = static_cast<int>(mapChipData_.data[0].size()) * kChipSize;
	int height = static_cast<int>(mapChipData_.data.size()) * kChipSize;

	// 縦線
	for (int x = 0; x <= mapChipData_.data[0].size(); x++) {
		int positionX = x * kChipSize;

		Novice::DrawLine(
			positionX, 0,
			positionX, height,
			0x000000FF
		);
	}

	// 横線
	for (int y = 0; y <= mapChipData_.data.size(); y++) {
		int positionY = y * kChipSize;

		Novice::DrawLine(
			0, positionY,
			width, positionY,
			0x000000FF
		);
	}
}