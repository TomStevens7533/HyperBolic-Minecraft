#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "ChunkStructs.h"


class ParserException : public std::runtime_error
{
public:
	~ParserException() {}
	ParserException(const std::string msg)
		: std::runtime_error(msg)
	{}
};

class TextureData;
struct BlockInformation {
	std::string name;
	bool IsSolid;
	std::map< Faces, std::vector<XMFLOAT2>> uvCoords;
};
class BlockJsonParser 
{
public:
	BlockJsonParser() = default;
	BlockJsonParser(std::wstring path) { ParseFile(path); }
	const std::map< Faces, std::vector<XMFLOAT2>>* GetUVOfType(uint8_t id) const;
	bool IsSolid(uint8_t id) const;
	void ParseFile(std::wstring path);
private:
	TextureData* m_pTextureAtlas;
	std::map<uint8_t, BlockInformation> m_BlockMap;
private:
	std::wstring to_wstring(std::string str);
};