#pragma once

#include "dxutils.h"

class AssetLoader {
public:
	AssetLoader(std::string _levelpath) {
		levelPath = _levelpath;
		ParseModelBin();
		ParseTextureBin();
	}
	~AssetLoader() = default;



private:
	void ParseModelBin() {
		
	}
	void ParseTextureBin() {

	}

	std::string levelPath;
	std::vector<BinModel> availableModels = std::vector<BinModel>();
};