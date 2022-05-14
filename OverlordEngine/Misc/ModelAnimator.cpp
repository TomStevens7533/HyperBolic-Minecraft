#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	TODO_W7_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);
		//2. 
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			m_TickCount += m_CurrentClip.duration;

		}
		else {
			//ELSE\
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if (m_TickCount > m_CurrentClip.duration) {
				m_TickCount -= m_CurrentClip.duration;
			}

		}



		//3.
		//Find the enclosing keys
		//AnimationKey keyA, keyB;
		AnimationKey keyA, keyB, keyFind;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		keyFind.tick = m_TickCount;
		auto it = std::lower_bound(m_CurrentClip.keys.begin(), m_CurrentClip.keys.end(), keyFind, [](AnimationKey& key1, AnimationKey key2)
			{
				return key1.tick < key2.tick;
			});
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		keyA = *it;
		std::advance(it, -1);
		keyB = *it;
		//4. Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//current tick count - start tick / diff
		float blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick); //get diff;
		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		for (unsigned int i{}; i < m_pMeshFilter->m_BoneCount; ++i)
		{
			//	Retrieve the transform from keyA (transformA)
			//	auto transformA = ...
			auto transformA = keyA.boneTransforms[i];
			// 	Retrieve the transform from keyB (transformB)
			auto transformB = keyB.boneTransforms[i];
			//	Decompose both transforms
			//a
			DirectX::XMVECTOR  posA;
			DirectX::XMVECTOR  scaleA;
			DirectX::XMVECTOR  rotationA;
			DirectX::XMMatrixDecompose(&scaleA, &rotationA, &posA, DirectX::XMLoadFloat4x4(&transformA));

			//b
			DirectX::XMVECTOR  posB;
			DirectX::XMVECTOR  scaleB;
			DirectX::XMVECTOR  rotationB;
			DirectX::XMMatrixDecompose(&scaleB, &rotationB, &posB, DirectX::XMLoadFloat4x4(&transformB));
			//	Lerp between all the transformations (Position, Scale, Rotation)
			DirectX::XMVECTOR pos = DirectX::XMVectorLerp(posA, posB, blendFactor);
			DirectX::XMVECTOR scale = DirectX::XMVectorLerp(scaleA, scaleB, blendFactor);
			DirectX::XMVECTOR rotation = DirectX::XMQuaternionSlerp(rotationA, rotationB, blendFactor);
			//	Compose a transformation matrix with the lerp-results
			DirectX::XMMATRIX posMatrix = DirectX::XMMatrixTranslationFromVector(pos);
			DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);
			DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
			//	Add the resulting matrix to the m_Transforms vector
			auto result = scaleMatrix * rotationMatrix * posMatrix;
			DirectX::XMFLOAT4X4 result4x4{};
			DirectX::XMStoreFloat4x4(&result4x4, result);
			m_Transforms.push_back(result4x4);

		}



	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	TODO_W7_()
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	auto animIT = std::find_if(m_pMeshFilter->m_AnimationClips.begin(), m_pMeshFilter->m_AnimationClips.end(), [&clipName](AnimationClip& animClip) {
		
		return animClip.name == clipName;
	});

	if (animIT != m_pMeshFilter->m_AnimationClips.end()) {
		//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
		SetAnimation(*animIT);
	}
	else {
		//Else
		//	Call Reset
		//	Log a warning with an appropriate message
		Reset();
		Logger::LogError(L"Animation clip not found | name was not found");
		return;
	}
	
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < m_pMeshFilter->m_AnimationClips.size()) {
		//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
		AnimationClip newAnimationClip = m_pMeshFilter->m_AnimationClips[clipNumber];
		SetAnimation(newAnimationClip);
	}

	else {
	//If not,
	//	Call Reset
		Reset();
	//	Log a warning with an appropriate message
		Logger::LogError(L"Animation clip not found idx out of boundds");
	//	return
		return;
	}


}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause == true)
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0.f;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;
	//If m_ClipSet is true
	if (m_ClipSet == true) {
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		auto boneTransform = m_CurrentClip.keys[0].boneTransforms;
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(boneTransform.begin(), boneTransform.end());

	}
	else {
		//	Create an IdentityMatrix 
		DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 identityMatrix4X4{};
		DirectX::XMStoreFloat4x4(&identityMatrix4X4, identityMatrix);
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_Transforms.size(), identityMatrix4X4);

	}
	
}
