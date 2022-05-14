#include "stdafx.h"
#include "BlockJsonParser.h"
#include "rapidjson/document.h"
#include <xlocbuf>

const std::map< Faces, std::vector<XMFLOAT2>>* BlockJsonParser::GetUVOfType(uint8_t id) const
{	
	if (m_BlockMap.count(id) > 0) {
		return &m_BlockMap.at(id).uvCoords;
	}
	return nullptr;
}

bool BlockJsonParser::IsSolid(uint8_t id) const
{


	if (m_BlockMap.count(id) > 0) {
		return m_BlockMap.find(id)->second.IsSolid;
	}
	return false;
}

void BlockJsonParser::ParseFile(std::wstring path)
{	
	using namespace rapidjson;
	std::ifstream file{ path };
	if (!file.is_open())
		throw ParserException("Unable to open file");

	// read the file contents
	std::stringstream contents;
	contents << file.rdbuf();

	Document document;
	document.Parse(contents.str().c_str());
	GenericObject fullobj  = document.GetObj();

	
	std::wstring wideTexurePath;
	std::string texturePath = fullobj.FindMember("TexturePath")->value.GetString();
	wideTexurePath = to_wstring(texturePath);
	m_pTextureAtlas = ContentManager::Load<TextureData>(wideTexurePath);
	int textureAtlasTotalCol = fullobj.FindMember("TotalCol")->value.GetInt();
	int textureAtlasTotalRow = fullobj.FindMember("TotalRow")->value.GetInt();
	//Get BlockInformation
	GenericArray blockArr = fullobj.FindMember("BlockInfo")->value.GetArray();

	for (int i = 0; i < static_cast<int>(blockArr.Size()); i++)
	{
		BlockInformation info;
		GenericObject blockObj = blockArr[i].GetObj();
		uint8_t blockID = static_cast<uint8_t>(blockObj.FindMember("BlockID")->value.GetUint());
		info.name = blockObj.FindMember("name")->value.GetString();
		info.IsSolid = blockObj.FindMember("IsSolid")->value.GetBool();

		GenericArray UVarray = blockObj.FindMember("UV")->value.GetArray();
		GenericObject arrayOBj = UVarray.begin()->GetObj();
		
		int idx = 0;
		for (auto uvIt = arrayOBj.begin(); uvIt < arrayOBj.end(); uvIt++)
		{
			
			GenericObject obj = uvIt->value.GetObj();
			std::string name = uvIt->name.GetString();

			int currCol = obj.FindMember("AtlasCol")->value.GetInt();
			int currRow = obj.FindMember("AtlasRow")->value.GetInt();

			float texWidth = (m_pTextureAtlas->GetDimension().x / textureAtlasTotalCol) / m_pTextureAtlas->GetDimension().x;
			float texHeight = (m_pTextureAtlas->GetDimension().y / textureAtlasTotalRow) / m_pTextureAtlas->GetDimension().y;


			XMFLOAT2 uvCoordsTopRight{ (texWidth * currCol),
				texHeight * currRow };

			XMFLOAT2 uvCoordsTopLeft{ uvCoordsTopRight.x - texWidth, uvCoordsTopRight.y };

			XMFLOAT2 uvCoordsBotLeft{ uvCoordsTopRight.x, uvCoordsTopRight.y - texHeight };

			XMFLOAT2 uvCoordsBotRight{ uvCoordsTopRight.x - texWidth, uvCoordsTopRight.y - texHeight };

			switch (idx)
			{
			case 0:
				info.uvCoords[Faces::TOP].reserve(4);
				info.uvCoords[Faces::TOP].push_back(uvCoordsTopLeft);
				info.uvCoords[Faces::TOP].push_back(uvCoordsTopRight);
				info.uvCoords[Faces::TOP].push_back(uvCoordsBotLeft);
				info.uvCoords[Faces::TOP].push_back(uvCoordsBotRight);
				break;
			case 1:
				info.uvCoords[Faces::BOT].reserve(4);
				info.uvCoords[Faces::BOT].push_back(uvCoordsTopLeft);
				info.uvCoords[Faces::BOT].push_back(uvCoordsTopRight);
				info.uvCoords[Faces::BOT].push_back(uvCoordsBotLeft);
				info.uvCoords[Faces::BOT].push_back(uvCoordsBotRight);
				break;
			case 2:
				info.uvCoords[Faces::LEFT].reserve(4);
				info.uvCoords[Faces::LEFT].push_back(uvCoordsTopLeft);
				info.uvCoords[Faces::LEFT].push_back(uvCoordsTopRight);
				info.uvCoords[Faces::LEFT].push_back(uvCoordsBotLeft);
				info.uvCoords[Faces::LEFT].push_back(uvCoordsBotRight);
				break;
			case 3:
				info.uvCoords[Faces::RIGHT].reserve(4);
				info.uvCoords[Faces::RIGHT].push_back(uvCoordsTopLeft);
				info.uvCoords[Faces::RIGHT].push_back(uvCoordsTopRight);
				info.uvCoords[Faces::RIGHT].push_back(uvCoordsBotLeft);
				info.uvCoords[Faces::RIGHT].push_back(uvCoordsBotRight);
				break;
			case 4:
				info.uvCoords[Faces::FRONT].reserve(4);
				info.uvCoords[Faces::FRONT].push_back(uvCoordsTopLeft);
				info.uvCoords[Faces::FRONT].push_back(uvCoordsTopRight);
				info.uvCoords[Faces::FRONT].push_back(uvCoordsBotLeft);
				info.uvCoords[Faces::FRONT].push_back(uvCoordsBotRight);
				break;
			case 5:
				info.uvCoords[Faces::BACK].reserve(4);
				info.uvCoords[Faces::BACK].push_back(uvCoordsTopLeft);
				info.uvCoords[Faces::BACK].push_back(uvCoordsTopRight);
				info.uvCoords[Faces::BACK].push_back(uvCoordsBotLeft);
				info.uvCoords[Faces::BACK].push_back(uvCoordsBotRight);
				break;
			default:
				break;
			}
			idx++;
		}

		//int currCol = blockObj.FindMember("AtlasCol")->value.GetInt();
		//int currRow = blockObj.FindMember("AtlasRow")->value.GetInt();

		//float texWidth = (m_pTextureAtlas->GetDimension().x / textureAtlasTotalCol) / m_pTextureAtlas->GetDimension().x;
		//float texHeight = (m_pTextureAtlas->GetDimension().y / textureAtlasTotalRow) / m_pTextureAtlas->GetDimension().y;

		//info.uvCoords.reserve(4);

		//XMFLOAT2 uvCoordsTopLeft{ (texWidth * currCol ),
		//	texHeight * currRow};
		//info.uvCoords.push_back(uvCoordsTopLeft);

		//XMFLOAT2 uvCoordsTopRight{ uvCoordsTopLeft.x + texWidth, uvCoordsTopLeft.y};
		//info.uvCoords.push_back(uvCoordsTopRight);

		//XMFLOAT2 uvCoordsBotLeft{ uvCoordsTopLeft.x, uvCoordsTopLeft.y + texHeight };
		//info.uvCoords.push_back(uvCoordsBotLeft);

		//XMFLOAT2 uvCoordsBotRight{ uvCoordsTopLeft.x + texWidth, uvCoordsTopLeft.y + texHeight };
		//info.uvCoords.push_back(uvCoordsBotRight);

		m_BlockMap.insert(std::make_pair(blockID, info));
	}

	file.close();
}

std::wstring BlockJsonParser::to_wstring(std::string str)
{
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;
	return strconverter.from_bytes(str);
}