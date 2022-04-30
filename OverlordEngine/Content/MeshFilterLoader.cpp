#include "stdafx.h"
#include "MeshFilterLoader.h"

//#define MULTI_TEXCOORD
#define OVM_vMAJOR 1
#ifdef MULTI_TEXCOORD
	#define OVM_vMINOR 61
#else
	#define OVM_vMINOR 1
#endif


MeshFilter* MeshFilterLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if(!pReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = pReader->Read<char>();
	const int versionMinor = pReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogWarning(L"Wrong OVM version\n\tFile: \"{}\" \n\tExpected version {}.{}, not {}.{}.", loadInfo.assetFullPath.filename().wstring(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);

		delete pReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	const auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataType>(pReader->Read<char>());
		if(meshDataType == MeshDataType::END)
			break;

		const auto dataOffset = pReader->Read<unsigned int>();

		switch(meshDataType)
		{
		case MeshDataType::HEADER:
			{
				pMesh->m_MeshName = pReader->ReadString();
				vertexCount = pReader->Read<unsigned int>();
				indexCount = pReader->Read<unsigned int>();

				pMesh->m_VertexCount = vertexCount;
				pMesh->m_IndexCount = indexCount;
			}
			break;
		case MeshDataType::POSITIONS:
			{
				pMesh->SetElement(ILSemantic::POSITION);

				for(unsigned int i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 pos{};
					pos.x = pReader->Read<float>();
					pos.y = pReader->Read<float>();
					pos.z = pReader->Read<float>();
					pMesh->m_Positions.emplace_back(pos);
				}
			}
			break;
		case MeshDataType::INDICES:
			{
				for(unsigned int i = 0; i<indexCount; ++i)
				{
					pMesh->m_Indices.emplace_back(pReader->Read<DWORD>());
				}
			}
			break;
		case MeshDataType::NORMALS:
			{
				pMesh->SetElement(ILSemantic::NORMAL);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 normal{};
					normal.x = pReader->Read<float>();
					normal.y = pReader->Read<float>();
					normal.z = pReader->Read<float>();
					pMesh->m_Normals.emplace_back(normal);
				}
			}
			break;
		case MeshDataType::TANGENTS:
			{
				pMesh->SetElement(ILSemantic::TANGENT);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 tangent{};
					tangent.x = pReader->Read<float>();
					tangent.y = pReader->Read<float>();
					tangent.z = pReader->Read<float>();
					pMesh->m_Tangents.emplace_back(tangent);
				}
			}
			break;
		case MeshDataType::BINORMALS:
			{
				pMesh->SetElement(ILSemantic::BINORMAL);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
					XMFLOAT3 binormal{};
					binormal.x = pReader->Read<float>();
					binormal.y = pReader->Read<float>();
					binormal.z = pReader->Read<float>();
					pMesh->m_Binormals.emplace_back(binormal);
				}
			}
			break;
		case MeshDataType::TEXCOORDS:
			{
				pMesh->SetElement(ILSemantic::TEXCOORD);

				constexpr auto amountTexCoords = 1;
#ifdef MULTI_TEXCOORD
				amountTexCoords = pReader->Read<USHORT>();
#endif
				pMesh->m_TexCoordCount = amountTexCoords;
				for (unsigned int i = 0; i<vertexCount*amountTexCoords; ++i)
				{
					XMFLOAT2 tc{};
					tc.x = pReader->Read<float>();
					tc.y = pReader->Read<float>();
					pMesh->m_TexCoords.emplace_back(tc);
				}
			}
			break;
		case MeshDataType::COLORS:
			{
				pMesh->SetElement(ILSemantic::COLOR);

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
					XMFLOAT4 color{};
					color.x = pReader->Read<float>();
					color.y = pReader->Read<float>();
					color.z = pReader->Read<float>();
					color.w = pReader->Read<float>();
					pMesh->m_Colors.emplace_back(color);
				}
			}
			break;
		case MeshDataType::BLENDINDICES:
		{
			pMesh->SetElement(ILSemantic::BLENDINDICES);
			//// BLENDINDICES BLOCK (ID 9)
			// Contains the BlendIndex values for every vertex
			//	 Data
			//	 For every vertex
			/*	 4 Floats(x, y, z and w components)*/
		
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 blendIndice;
				blendIndice.x = pReader->Read<float>();
				blendIndice.y = pReader->Read<float>();
				blendIndice.z = pReader->Read<float>();
				blendIndice.w = pReader->Read<float>();
				pMesh->m_BlendIndices.push_back(blendIndice);
			}
		}
		break;
		case MeshDataType::BLENDWEIGHTS:
		{
			pMesh->SetElement(ILSemantic::BLENDWEIGHTS);

			// BLENDWEIGHTS BLOCK(ID 10)
			//	 Contains the BlendWeight values for every vertex
			//	 Data
			//	 For every vertex
			//	 4 Floats(x, y, z and w components

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 blendWeight;
				blendWeight.x = pReader->Read<float>();
				blendWeight.y = pReader->Read<float>();
				blendWeight.z = pReader->Read<float>();
				blendWeight.w = pReader->Read<float>();
				pMesh->m_BlendWeights.push_back(blendWeight);
			}
		}
		break;
		case MeshDataType::ANIMATIONCLIPS:
		{
			pMesh->m_HasAnimations = true;

			/*	ANIMATIONCLIPS BLOCK(ID 11)
				 Contains information about every AnimationClip(Name, Duration, Keys, ...), needed to
				calculate the BoneTransformations for our model.*/

			//ClipCount(UNSIGNED INTEGER, 2 bytes)[Number of clips]
			UINT16 clipCount = pReader->Read<UINT16>();
			//2. For every clip
			for (UINT16 i{}; i < clipCount; ++i)
			{
				AnimationClip clip{};
				//read name
				clip.name = pReader->ReadString();
				//read duration
				clip.duration = pReader->Read<float>();
				//read ticks per second
				clip.ticksPerSecond = pReader->Read<float>();
				//read key count
				UINT16 keyCount = pReader->Read<UINT16>();
				//voor alle keys
				for (UINT16 j{}; j < keyCount; ++j)
				{
					AnimationKey key{};
					//read tick
					key.tick = pReader->Read<float>();
					//read boneTransform
					UINT16 boneCount = pReader->Read<UINT16>();
					//for every bonetransformm
					for (UINT16 k{}; k < boneCount; ++k)
					{
						//Get transform for that bone
 						DirectX::XMFLOAT4X4 transform;
						transform = pReader->Read<DirectX::XMFLOAT4X4>();
		
						key.boneTransforms.push_back(transform);
					}
					clip.keys.push_back(key);
				}
				pMesh->m_AnimationClips.push_back(clip);
			}
		}
		break;
		case MeshDataType::SKELETON:
		{
			pMesh->m_BoneCount = pReader->Read<UINT16>();
			//Move back 
			pReader->MoveBufferPosition(dataOffset - sizeof(UINT16));
		}
		break;
		default:
			pReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete pReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}
