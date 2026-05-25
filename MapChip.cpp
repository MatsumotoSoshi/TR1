#include "MapChip.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <Novice.h>


void MapChip::LoadCSV(const std::string& filePath) {

	mapData_.clear();

	std::ifstream file(filePath);
	assert(file.is_open());

	std::string line;

	while (std::getline(file, line)) {

		std::vector<MapChipType> row;
		std::istringstream lineStream(line);
		std::string cell;

		while (std::getline(lineStream, cell, ',')) {

			if (cell == "1") {
				row.push_back(MapChipType::kBlock);
			} else {
				row.push_back(MapChipType::kBlank);
			}
		}

		mapData_.push_back(row);
	}
}

bool MapChip::IsBlock(int x, int y) const {

	if (y < 0 || y >= mapData_.size()) return true;
	if (x < 0 || x >= mapData_[0].size()) return true;

	return mapData_[y][x] == MapChipType::kBlock;
}

void MapChip::Draw() {

	for (int y = 0; y < mapData_.size(); y++) {
		for (int x = 0; x < mapData_[y].size(); x++) {

			if (mapData_[y][x] == MapChipType::kBlock) {

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

	int width = static_cast<int>(mapData_[0].size()) * kChipSize;
	int height = static_cast<int>(mapData_.size()) * kChipSize;

	// 縦線
	for (int x = 0; x <= mapData_[0].size(); x++) {
		int positionX = x * kChipSize;

		Novice::DrawLine(
			positionX, 0,
			positionX, height,
			0x000000FF
		);
	}

	// 横線
	for (int y = 0; y <= mapData_.size(); y++) {
		int positionY = y * kChipSize;

		Novice::DrawLine(
			0, positionY,
			width, positionY,
			0x000000FF
		);
	}
}