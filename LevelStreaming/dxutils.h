#pragma once

#include <windows.h>
#include "dxshared.h"
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "dxerr.h"
#include "DDSTextureLoader.h"
#include "InputHandler.h"

#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>

#include <time.h>
#include <random>

using namespace DirectX;

/* Handle DX HRESULT errors nicely in debug */
#ifdef _DEBUG
	#ifndef HR
		#define HR(x) { HRESULT hr = x; if (FAILED(hr)) { DXTraceW(__FILEW__, __LINE__, hr, L#x, TRUE); } }
	#endif
#else
	#ifndef HR
		#define HR(x) x;
	#endif
#endif

enum LevelType {
	FE_LEVEL, // Frontend Level
	STD_LEVEL // Standard Level
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};

struct SimpleVertexAlt
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 colourTint;
	XMFLOAT4 ambientLight;
	XMFLOAT4 pointlightPosition; //W is used for intensity
	XMFLOAT4 pointlightColour;
};

struct ConstantBufferAlt
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct VertexGroup
{
	int v; //Vertex
	int c; //Tex coord
	int n; //Normal
	bool set = false;
};

enum VertReaderType  
{
	VERTEX,
	COORDINATE,
	NORMAL,
};

enum LevelOfDetail {
	HIGH,
	LOW,
	UNLOADED
};

struct Material
{
	std::string materialName = "";

	float r, g, b, a = 1.0f;
	std::string texturePath = "DATA/ENV/GLOBAL/plastic_base.dds"; //placeholder blank texture
	int textureID = -1; //Used by the new BIN/PAK loading
};

struct LoadedMaterial
{
	std::string materialName = "";
	ID3D11ShaderResourceView* materialTexture = nullptr;
	XMFLOAT4 materialColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct LoadedModelPart
{
	std::vector<SimpleVertex> compVertices = std::vector<SimpleVertex>();
	std::vector<WORD> compIndices = std::vector<WORD>();
	Material thisMaterial = Material();
};

struct LoadedModel 
{
	std::vector<LoadedModelPart> modelParts = std::vector<LoadedModelPart>();
};

struct BinModel {
	LevelOfDetail modelLOD;
	std::string modelName;
	int pakOffset;
	int pakLength;
	int modelPartCount;
	std::vector<int> vertCount = std::vector<int>();
};

struct BinModelPair {
	std::string name = "";
	BinModel LOD0 = BinModel();
	BinModel LOD1 = BinModel();
};

struct Face
{
	std::vector<VertexGroup> verts = std::vector<VertexGroup>(); //Vertices (SHOULD BE 3 - TRIANGULATED)
	std::string materialName = ""; //Material name to link with MTL
};

struct ZoneLoadQueue {
	ZoneLoadQueue(std::thread* _t, int _i) {
		threadOperation = _t;
		zoneID = _i;
	}
	bool executing = true;
	bool completed = false;
	std::thread* threadOperation;
	int zoneID;
};

struct ModelDef {
	std::string modelName = "";
	std::string modelPath_LOD1 = "";
	std::string modelPath_LOD2 = "";
};

struct ModelPlacement {
	std::string modelName = "";
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(0, 0, 0);
};

struct BoundsPair {
	DirectX::XMFLOAT3 localBottomLeft;
	DirectX::XMFLOAT3 localTopRight;
};

/* Debug logger */
class Debug
{
public:
	static void Log(std::string msg) {
		OutputDebugString(msg.c_str());
		OutputDebugString("\n");
	}
	static void Log(int msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(float msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(double msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(unsigned long msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
	static void Log(size_t msg) {
		OutputDebugString(std::to_string(msg).c_str());
		OutputDebugString("\n");
	}
};

class Utilities
{
public:
	/* Try and compile a shader from file (function created by Microsoft originally) */
	HRESULT CompileShaderFromFile(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
		//Request d3d debugging if in debug
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		dwShaderFlags |= D3DCOMPILE_DEBUG;
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		//Try and compile shader, handle errors
		ID3DBlob* pErrorBlob = nullptr;
		HRESULT hr = S_OK;
		hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
				pErrorBlob->Release();
			}
			return hr;
		}
		if (pErrorBlob) pErrorBlob->Release();

		return S_OK;
	}

	/* Convert a DirectX Matrix to Float4X4 */
	XMFLOAT4X4 MatrixToFloat4x4(XMMATRIX mat)
	{
		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, mat);
		return temp;
	}

	/* Load a texture from a PAK archive, given an ID */
	ID3D11ShaderResourceView* LoadTextureFromPAK(int textureID)
	{
		//Find our texture in the PAK and return it
		ID3D11ShaderResourceView* materialTexture = nullptr;
		for (int i = 0; i < dxshared::currentTexturePool.size(); i++) {
			if (dxshared::currentTexturePool[i].textureID == textureID) {
				std::ifstream fin(dxshared::currentLevelPath + "LEVEL_TEXTURES.PAK", std::ios::in | std::ios::binary);
				fin.seekg(dxshared::currentTexturePool[i].pakOffset);
				uint8_t* thing;
				thing = new uint8_t[dxshared::currentTexturePool[i].pakLength];
				fin.read((char*)&thing[0], dxshared::currentTexturePool[i].pakLength);
				CreateDDSTextureFromMemory(dxshared::m_pDevice, (const uint8_t *)thing, dxshared::currentTexturePool[i].pakLength, nullptr, &materialTexture);
				return materialTexture;
			}
		}
		//Couldn't find one - fall back to our default
		std::string normstr = "DATA/ENV/GLOBAL/plastic_base.dds";
		std::wstring widestr = std::wstring(normstr.begin(), normstr.end());
		HR(CreateDDSTextureFromFile(dxshared::m_pDevice, widestr.c_str(), nullptr, &materialTexture));
		return materialTexture;
	}

	/* Load a model from a PAK archive, given BIN header info */
	LoadedModel LoadModelFromPAK(BinModel model_data) 
	{
		//Open PAK and skip to the bit we're interested in
		std::ifstream fin(dxshared::currentLevelPath + "LEVEL_MODELS.PAK", std::ios::in | std::ios::binary);
		fin.seekg(model_data.pakOffset);

		//Load data for each model part
		LoadedModel thisModel = LoadedModel();
		LoadedModelPart modelPart = LoadedModelPart();
		for (int i = 0; i < model_data.modelPartCount; i++) {
			fin.read((char*)&modelPart.thisMaterial.r, 4);
			fin.read((char*)&modelPart.thisMaterial.g, 4);
			fin.read((char*)&modelPart.thisMaterial.b, 4);
			fin.read((char*)&modelPart.thisMaterial.a, 4);
			fin.read((char*)&modelPart.thisMaterial.textureID, 4);

			for (int x = 0; x < model_data.vertCount[i]; x++) {
				int index_number;
				fin.read((char*)&index_number, 4);
				modelPart.compIndices.push_back((WORD)index_number);

				SimpleVertex thisVert = SimpleVertex();
				fin.read((char*)&thisVert.Pos.x, 4);
				fin.read((char*)&thisVert.Pos.y, 4);
				fin.read((char*)&thisVert.Pos.z, 4);
				fin.read((char*)&thisVert.Tex.x, 4);
				fin.read((char*)&thisVert.Tex.y, 4);
				fin.read((char*)&thisVert.Normal.x, 4);
				fin.read((char*)&thisVert.Normal.y, 4);
				fin.read((char*)&thisVert.Normal.z, 4);
				modelPart.compVertices.push_back(thisVert);
			}

			thisModel.modelParts.push_back(modelPart);
			modelPart = LoadedModelPart();
		}
		fin.close();
		return thisModel;
	}

	/* Load a model from an OBJ file and return its indices and vertexes (todo: make it condense the vertex array) */
	LoadedModel LoadModelFromOBJ(std::string path)
	{
		//Open OBJ
		std::ifstream in(path.c_str());
		if (!in) Debug::Log("Failed to open model file!");

		//Parse the OBJ to vertices/texcoords/normals
		std::vector<XMFLOAT3> verts;
		std::vector<XMFLOAT2> coords;
		std::vector<XMFLOAT3> normals;
		std::vector<Face> faces;
		std::string str;
		std::string materialLibrary = "";
		std::string currentMaterial = "";
		while (std::getline(in, str))
		{
			if (str.size() > 0)
			{
				if (str.length() > 7 && str.substr(0, 7) == "mtllib ")
				{
					materialLibrary = str.substr(7);
				}
				else if (str.length() > 2 && str.substr(0, 2) == "v ")
				{
					str = str.substr(2);
					std::string thisPos = "";
					XMFLOAT3 thisVertPos;
					int vertPosIndex = 0;
					for (int i = 0; i < str.length() + 1; i++)
					{
						if (str[i] == ' ' || i == str.length())
						{
							if (thisPos == "") continue;
							if (vertPosIndex == 0) thisVertPos.x = std::stof(thisPos);
							if (vertPosIndex == 1) thisVertPos.y = std::stof(thisPos);
							if (vertPosIndex == 2) thisVertPos.z = std::stof(thisPos);
							vertPosIndex++;
							thisPos = "";
							continue;
						}
						thisPos += str[i];
					}
					verts.push_back(thisVertPos);
				}
				else if (str.length() > 3 && str.substr(0, 3) == "vt ")
				{
					str = str.substr(3);
					std::string thisPos = "";
					XMFLOAT2 thisTexCoord;
					int texCoordIndex = 0;
					for (int i = 0; i < str.length() + 1; i++)
					{
						if (str[i] == ' ' || i == str.length())
						{
							if (thisPos == "") continue;
							if (texCoordIndex == 0) thisTexCoord.x = std::stof(thisPos);
							if (texCoordIndex == 1) thisTexCoord.y = std::stof(thisPos) * -1;
							texCoordIndex++;
							thisPos = "";
							continue;
						}
						thisPos += str[i];
					}
					coords.push_back(thisTexCoord);
				}
				else if (str.length() > 3 && str.substr(0, 3) == "vn ")
				{
					str = str.substr(3);
					std::string thisPos = "";
					XMFLOAT3 thisNormal;
					int normalIndex = 0;
					for (int i = 0; i < str.length() + 1; i++)
					{
						if (str[i] == ' ' || i == str.length())
						{
							if (thisPos == "") continue;
							if (normalIndex == 0) thisNormal.x = std::stof(thisPos);
							if (normalIndex == 1) thisNormal.y = std::stof(thisPos);
							if (normalIndex == 2) thisNormal.z = std::stof(thisPos);
							normalIndex++;
							thisPos = "";
							continue;
						}
						thisPos += str[i];
					}
					normals.push_back(thisNormal);
				}
				else if (str.length() > 7 && str.substr(0, 7) == "usemtl ")
				{
					currentMaterial = str.substr(7);
				}
				else if (str.length() > 2 && str.substr(0, 2) == "f ")
				{
					str = str.substr(2);
					Face thisFace = Face();
					VertexGroup thisVert = VertexGroup();
					VertReaderType next = VertReaderType::VERTEX;
					std::string currentNumber = "";
					for (int i = 0; i < str.length() + 1; i++) {
						char thisChar = str[i];
						if (thisChar == '/' || thisChar == ' ' || i == str.length())
						{
							if (currentNumber == "") {
								if (verts.size() == 0) Debug::Log("This model has no vertices!");
								if (coords.size() == 0) Debug::Log("This model has no UVs!");
								if (normals.size() == 0) Debug::Log("This model has no normals!");
								continue;
							}
							switch (next) {
							case VertReaderType::VERTEX:
								thisVert.v = std::stoi(currentNumber);
								next = VertReaderType::COORDINATE;
								break;
							case VertReaderType::COORDINATE:
								thisVert.c = std::stoi(currentNumber);
								next = VertReaderType::NORMAL;
								break;
							case VertReaderType::NORMAL:
								thisVert.n = std::stoi(currentNumber);
								next = VertReaderType::VERTEX;
								break;
							}
							thisVert.set = true;
							currentNumber = "";

							if (thisChar == '/') continue;
							if (!thisVert.set) continue;
							thisFace.verts.push_back(thisVert);
							thisVert = VertexGroup();
							continue;
						}
						currentNumber += thisChar;
					}
					if (thisFace.verts.size() != 3) Debug::Log("This model is not triangulated!");
					thisFace.materialName = currentMaterial;
					faces.push_back(thisFace);
				}
			}
		}
		in.close();

		//Open and parse MTL if it exists
		std::vector<Material> materials = std::vector<Material>();
		if (materialLibrary != "")
		{
			//Get model path parts
			std::vector<std::string> modelPath = std::vector<std::string>();
			std::string currentPath = "";
			for (int i = 0; i < path.length(); i++) 
			{
				if (path[i] == '/' || path[i] == '\\')
				{
					modelPath.push_back(currentPath);
					currentPath = "";
					continue;
				}
				currentPath += path[i];
			}
			modelPath.push_back(currentPath);

			//Get material path parts
			std::vector<std::string> mtlPath = std::vector<std::string>();
			currentPath = "";
			for (int i = 0; i < materialLibrary.length(); i++)
			{
				if (materialLibrary[i] == '/' || materialLibrary[i] == '\\')
				{
					mtlPath.push_back(currentPath);
					currentPath = "";
					continue;
				}
				currentPath += materialLibrary[i];
			}
			mtlPath.push_back(currentPath);

			//Reconstruct a good path to MTL using model path
			std::string pathToMtl = "";
			if (mtlPath.size() == 1) 
			{
				if (modelPath.size() == 1) 
				{
					pathToMtl = mtlPath[0];
				}
				else
				{
					for (int i = 0; i < modelPath.size() - 1; i++) 
					{
						pathToMtl += modelPath[i] + "/";
					}
					pathToMtl += mtlPath[0];
				}
			}
			else
			{
				pathToMtl = materialLibrary;
			}

			//Open MTL
			std::ifstream in2(pathToMtl.c_str());
			if (!in2) Debug::Log("Failed to open material file!"); 
			
			//Parse MTL into materials
			std::string str;
			Material currentMaterial = Material();
			while (std::getline(in2, str))
			{
				if (str.size() > 0)
				{
					if (str.length() > 7 && str.substr(0, 7) == "newmtl ")
					{
						if (currentMaterial.materialName != "")
						{
							materials.push_back(currentMaterial);
							currentMaterial = Material();
						}
						currentMaterial.materialName = str.substr(7);
					}
					else if (str.length() > 3 && str.substr(0, 3) == "Kd ")
					{
						str = str.substr(3);
						std::string thisColour = "";
						int thisColourIndex = 0;
						for (int i = 0; i < str.length() + 1; i++)
						{
							if (str[i] == ' ' || i == str.length())
							{
								if (thisColour == "") continue;
								if (thisColourIndex == 0) currentMaterial.r = std::stof(thisColour);
								if (thisColourIndex == 1) currentMaterial.g = std::stof(thisColour);
								if (thisColourIndex == 2) currentMaterial.b = std::stof(thisColour);
								thisColourIndex++;
								thisColour = "";
								continue;
							}
							thisColour += str[i];
						}
					}
					else if (str.length() > 2 && str.substr(0, 2) == "d ")
					{
						currentMaterial.a = std::stof(str.substr(2));
					}
					else if (str.length() > 7 && str.substr(0, 7) == "map_Kd ")
					{
						currentMaterial.texturePath = str.substr(7);
						if (currentMaterial.texturePath[1] == ':') {
							Debug::Log("Texture uses system path! " + currentMaterial.texturePath);
						}
						else
						{
							std::string texPrepend = "";
							for (int i = pathToMtl.length(); i >= 0; i--)
							{
								if (pathToMtl[i] == '/' || pathToMtl[i] == '\\')
								{
									texPrepend = pathToMtl.substr(0, i);
									break;
								}
							}
							currentMaterial.texturePath = texPrepend + "/" + currentMaterial.texturePath;
						}
					}
				}
			}
			if (currentMaterial.materialName != "")
			{
				materials.push_back(currentMaterial);
			}
		}

		//Create vertex and index arrays from the data
		LoadedModel thisModel = LoadedModel();
		LoadedModelPart modelPart = LoadedModelPart();
		int totalIndex = 0;
		for (int i = 0; i < faces.size(); i++)
		{
			for (int x = 0; x < faces[i].verts.size(); x++)
			{
				SimpleVertex thisVertInfo = SimpleVertex();
				thisVertInfo.Pos = verts[faces[i].verts[x].v - 1];
				thisVertInfo.Tex = coords[faces[i].verts[x].c - 1];
				thisVertInfo.Normal = normals[faces[i].verts[x].n - 1];

				if (modelPart.thisMaterial.materialName != faces[i].materialName) 
				{
					if (totalIndex != 0)
					{
						thisModel.modelParts.push_back(modelPart);
						modelPart = LoadedModelPart();
					}
					for (int y = 0; y < materials.size(); y++)
					{
						if (materials[y].materialName == faces[i].materialName)
						{
							modelPart.thisMaterial = materials[y];
							break;
						}
					}
				}

				modelPart.compVertices.push_back(thisVertInfo);
				modelPart.compIndices.push_back((WORD)totalIndex);

				totalIndex++;
			}
		}
		thisModel.modelParts.push_back(modelPart);
		return thisModel;
	}

	/* Save a LoadedModel out to an OBJ and MTL (todo: make it condense the vertex array) */
	bool SaveModel(LoadedModel model_data, std::string path) 
	{
		std::ofstream objFile;
		objFile.open(path + ".obj");
		std::ofstream mtlFile;
		mtlFile.open(path + ".mtl");
		if (!objFile.is_open()) return false;
		if (!mtlFile.is_open()) return false;
		objFile << "#### AUTOGENERATED BY PLANTGENERATOR ####\n";
		objFile << "mtllib " << path << ".mtl\n";
		mtlFile << "#### AUTOGENERATED BY PLANTGENERATOR ####\n";
		std::vector<std::string> parsedMats = std::vector<std::string>();
		int offset = 0;
		for (int i = 0; i < model_data.modelParts.size(); i++) {
			//OBJ
			objFile << "o part_" << std::to_string(i) << "\n";
			for (int x = 0; x < model_data.modelParts[i].compVertices.size(); x++) {
				XMFLOAT3 thisVert = model_data.modelParts[i].compVertices[x].Pos;
				objFile << "v " << std::to_string(thisVert.x) << " " << std::to_string(thisVert.y) << " " << std::to_string(thisVert.z) << "\n";
			}
			for (int x = 0; x < model_data.modelParts[i].compVertices.size(); x++) {
				XMFLOAT2 thisCoord = model_data.modelParts[i].compVertices[x].Tex;
				objFile << "vt " << std::to_string(thisCoord.x) << " " << std::to_string(thisCoord.y * -1) << "\n";
			}
			for (int x = 0; x < model_data.modelParts[i].compVertices.size(); x++) {
				XMFLOAT3 thisNorm = model_data.modelParts[i].compVertices[x].Normal;
				objFile << "vn " << std::to_string(thisNorm.x) << " " << std::to_string(thisNorm.y) << " " << std::to_string(thisNorm.z) << "\n";
			}
			objFile << "usemtl " << model_data.modelParts[i].thisMaterial.materialName << "\n";
			for (int x = 0; x < model_data.modelParts[i].compIndices.size(); x+=3) {
				objFile << "f " << model_data.modelParts[i].compIndices[x] + 1 + offset << "/" << model_data.modelParts[i].compIndices[x] + 1 + offset << "/" << model_data.modelParts[i].compIndices[x] + 1 + offset << " ";
				objFile << model_data.modelParts[i].compIndices[x+1] + 1 + offset << "/" << model_data.modelParts[i].compIndices[x+1] + 1 + offset << "/" << model_data.modelParts[i].compIndices[x+1] + 1 + offset << " ";
				objFile << model_data.modelParts[i].compIndices[x+2] + 1 + offset << "/" << model_data.modelParts[i].compIndices[x+2] + 1 + offset << "/" << model_data.modelParts[i].compIndices[x+2] + 1 + offset << "\n";
			}
			offset += model_data.modelParts[i].compIndices[model_data.modelParts[i].compIndices.size() - 1] + 1;

			//MTL
			bool alreadyExported = false;
			for (int x = 0; x < parsedMats.size(); x++) {
				if (parsedMats[x] == model_data.modelParts[i].thisMaterial.materialName) {
					alreadyExported = true;
					break;
				}
			}
			if (alreadyExported) continue;
			parsedMats.push_back(model_data.modelParts[i].thisMaterial.materialName);
			mtlFile << "\nnewmtl " << model_data.modelParts[i].thisMaterial.materialName << "\n";
			mtlFile << "Kd " << model_data.modelParts[i].thisMaterial.r << " " << model_data.modelParts[i].thisMaterial.g << " " << model_data.modelParts[i].thisMaterial.b << "\n";
			mtlFile << "d " << model_data.modelParts[i].thisMaterial.a << "\n";
			mtlFile << "map_Kd " << model_data.modelParts[i].thisMaterial.materialName << ".dds\n";
			mtlFile << "illum 2\n";

			//Copy texture file
			CopyFromTo(model_data.modelParts[i].thisMaterial.texturePath, model_data.modelParts[i].thisMaterial.materialName + ".dds");
		}
		objFile.close();
		mtlFile.close();
		Debug::Log("Saved OBJ");
		return true;
	}

	/* Copy a file from one location to another */
	void CopyFromTo(std::string from, std::string to) {
		std::ifstream  src(from, std::ios::binary);
		std::ofstream  dst(to, std::ios::binary);
		dst << src.rdbuf();
		src.close();
		dst.close();
	}

	/* Transform a loadedmodel by a world matrix, and push it to another loadedmodel */
	void TransformAndPush(XMMATRIX world, LoadedModel& model, LoadedModel& push_to) {
		XMFLOAT3 transformedVert = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (int x = 0; x < model.modelParts.size(); x++) {
			LoadedModelPart thisPart = LoadedModelPart();
			thisPart.compIndices = model.modelParts[x].compIndices;
			thisPart.thisMaterial = model.modelParts[x].thisMaterial;
			for (int y = 0; y < model.modelParts[x].compVertices.size(); y++) {
				SimpleVertex thisVertInfo = model.modelParts[x].compVertices[y];
				XMStoreFloat3(&transformedVert, XMVector3Transform(XMLoadFloat3(&model.modelParts[x].compVertices[y].Pos), world));
				thisVertInfo.Pos = transformedVert;
				thisPart.compVertices.push_back(thisVertInfo);
			}
			push_to.modelParts.push_back(thisPart);
		}
	}
};

namespace Memory 
{
	/* Safely delete a pointer */
	template <class T> void SafeDelete(T& t)
	{
		if (t)
		{
			delete t;
			t = nullptr;
		}
	}

	/* Safely delete a pointer array */
	template <class T> void SafeDeleteArray(T& t)
	{
		if (t)
		{
			delete[] t;
			t = nullptr;
		}
	}

	/* Safely release a d3d resource */
	template <class T> void SafeRelease(T& t)
	{
		if (t)
		{
			t->Release();
			t = nullptr;
		}
	}
};