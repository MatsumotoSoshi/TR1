#pragma once
#include <vector>
#include <string>
#include "Vector.h"

enum class MapChipType {
	kBlank,
	kBlock,
	kPlayer,
	kEnemy,
};

struct MapChipDataUnit {
	MapChipType type;
	uint8_t subID;
};

struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data;
};

enum MapChipCharIndex {
	kChipType = 0,
	kChipSubID = 1,
};

class MapChip {
public:
	void LoadCSV(const std::string& filePath);
	void Draw();

	bool IsBlock(int x, int y) const;

	Vector2 GetPlayerSpawnPosition() const;
	Vector2 GetEnemySpawnPosition() const;

	int GetChipSize() const { return kChipSize; }

	MapChipType GetChipType(int x, int y) const;

private:
	MapChipData mapChipData_;

	static const int kChipSize = 40;
	static const int kMapHeight = 36;
	static const int kMapWidth = 64;
};