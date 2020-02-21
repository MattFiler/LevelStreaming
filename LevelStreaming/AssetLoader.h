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
		std::ifstream fin(levelPath + "LEVEL_MODELS.BIN", std::ios::in | std::ios::binary);
		int file_version;
		fin.read((char*)&file_version, 4);
		//if (file_version !=)
		int entry_count;
		fin.read((char*)&entry_count, 4);
		for (int i = 0; i < entry_count; i++) {
			INT16 lod;
			fin.read((char*)&lod, 2);
			BinModel thisModelData = BinModel();
			thisModelData.modelLOD = (LevelOfDetail)lod;
			INT8 string_len;
			fin.read((char*)&string_len, 1);
			thisModelData.modelName = "";
			for (int x = 0; x < string_len; x++) {
				char this_char;
				fin.read((char*)&this_char, 1);
				thisModelData.modelName += this_char;
			}
			fin.read((char*)&thisModelData.pakOffset, 4);
			fin.read((char*)&thisModelData.pakLength, 4);
			fin.read((char*)&thisModelData.modelPartCount, 4);
			for (int x = 0; x < thisModelData.modelPartCount; x++) {
				int this_vert_count;
				fin.read((char*)&this_vert_count, 4);
				thisModelData.vertCount.push_back(this_vert_count);
			}
		}
		fin.close();
	}
	void ParseTextureBin() {

	}

	std::string levelPath;
	std::vector<BinModel> availableModels = std::vector<BinModel>();
};