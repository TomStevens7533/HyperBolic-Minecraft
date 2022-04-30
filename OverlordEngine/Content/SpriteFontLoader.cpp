#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...
	
	if (pReader->Read<byte>() != 'B') {
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font", loadInfo.assetSubPath);
		return nullptr;
	}
	if (pReader->Read<byte>() != 'M') {
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font", loadInfo.assetSubPath);
		return nullptr;
	}
	if (pReader->Read<byte>() != 'F') {
		Logger::LogError( L"SpriteFontLoader::LoadContent > Not a valid .fnt font", loadInfo.assetSubPath);
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...

	if (pReader->Read<byte>() < 3) {
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported", loadInfo.assetSubPath);
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	pReader->Read<byte>();
	pReader->Read<int>();
	//Retrieve the FontSize [fontDesc.fontSize]
	short FontSize = pReader->Read<short>();
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [fontDesc.fontName]
	//...
	pReader->MoveBufferPosition(12);
	std::wstring name = pReader->ReadNullString();
	fontDesc.fontName = name;
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	fontDesc.fontSize = FontSize;	//Set font size

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	pReader->Read<byte>();
	pReader->Read<int>();
	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	pReader->MoveBufferPosition(4);
	UINT16 width = pReader->Read<UINT16>();
	UINT16 height = pReader->Read<UINT16>();
	fontDesc.textureWidth = width;
	fontDesc.textureHeight = height;
	
	//Retrieve PageCount
	//> if pagecount > 1
	//	> Log Error (Only one texture per font is allowed!)
	//Advance to Block2 (Move Reader)
	//...

	if (pReader->Read<UINT16>() > 1) {
		Logger::LogError(L"Only one texture per font allowed", loadInfo.assetSubPath);
		return nullptr;
	}
	pReader->MoveBufferPosition(5);


	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	pReader->Read<byte>();
	pReader->Read<int>();
	//Retrieve the PageName (BinaryReader::ReadNullString)
	std::wstring pageName = pReader->ReadNullString();
	if (pageName.empty()) {
		Logger::LogError(L"Invlid Font, No page name found", loadInfo.assetSubPath);
		return nullptr;
	}
	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]

	
	std::wstring fullTexturePath;
	fullTexturePath = loadInfo.assetFullPath;
	auto size = fullTexturePath.rfind(L"Consolas_32.fnt");
	if (size != std::string::npos) {
		fullTexturePath = fullTexturePath.substr(0, size);
	}
	fullTexturePath.append(pageName);
	TextureData* texture = ContentManager::Load<TextureData>(fullTexturePath);
	fontDesc.pTexture = texture;

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	pReader->Read<byte>();
	int blockSize = pReader->Read<int>();
	//Retrieve Character Count (see documentation)
	int numChar = blockSize / 20;



	//Create loop for Character Count, and:
	for (int i{}; i < numChar; ++i)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		UINT32 characterIdUINT = pReader->Read<UINT32>();
		wchar_t characterId = static_cast<wchar_t>(characterIdUINT);
		//> Create instance of FontMetric (struct)
		FontMetric fontMetric;
		//	> Set Character (CharacterId) [FontMetric::character]
		fontMetric.character = characterId;
		//	> Retrieve Xposition (store Local)
		UINT16 Xposition = pReader->Read<UINT16>();
		//	> Retrieve Yposition (store Local)
		UINT16 Yposition = pReader->Read<UINT16>();
		//	> Retrieve & Set Width [FontMetric::width]
		fontMetric.width = pReader->Read<unsigned short>();
		//	> Retrieve & Set Height [FontMetric::height]
		fontMetric.height = pReader->Read<unsigned short>();
		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		fontMetric.offsetX = pReader->Read<short>();
		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		fontMetric.offsetY = pReader->Read<short>();
		//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
		fontMetric.advanceX = pReader->Read<short>();
		//	> Retrieve & Set Page [FontMetric::page]
		fontMetric.page = pReader->Read<unsigned char>();

		//	> Retrieve Channel (BITFIELD!!!) 
		//		> See documentation for BitField meaning [FontMetrix::channel]
		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]

		BYTE bitField = pReader->Read<BYTE>();
		int rightMostPos = int(log2(bitField & -bitField) + 1);
		switch (rightMostPos)
		{
		case 1:
			fontMetric.channel = 2;
			break;
		case 2:
			fontMetric.channel = 1;
			break;
		case 3:
			fontMetric.channel = 0;
			break;
		case 4:
			fontMetric.channel = 3;
			break;
		}


		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		//...
		fontMetric.texCoord = DirectX::XMFLOAT2(Xposition / float(fontDesc.textureWidth), Yposition / float(fontDesc.textureHeight));


		//
		//> Insert new FontMetric to the metrics [font.metrics] map
		//	> key = (wchar_t) charId
		//	> value = new FontMetric
		//(loop restarts till all metrics are parsed)

		fontDesc.metrics.insert(std::make_pair(fontMetric.character, fontMetric));
		
	}
	

	

	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
