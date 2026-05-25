#pragma once
#include <vector>
#include <string>

enum class MapChipType {
	kBlank,   // 空き
	kBlock    // ブロック
};

class MapChip {
public:
	void LoadCSV(const std::string& filePath);
	void Draw();

	bool IsBlock(int x, int y) const;

	int GetChipSize() const { return kChipSize; }

private:
	std::vector<std::vector<MapChipType>> mapData_;

	const int kChipSize = 40;    // マップチップサイズ
};
