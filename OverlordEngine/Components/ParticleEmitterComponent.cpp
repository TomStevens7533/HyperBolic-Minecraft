#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"
#include "Utils/MathHelper.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//Create static material
	if(m_pParticleMaterial == nullptr)
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();


	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);

}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement CreateVertexBuffer")

		if (m_pVertexBuffer)
			m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&desc, nullptr, &m_pVertexBuffer));
	
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	float particleInterval = ((m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2.f) / m_ParticleCount;


	//Increate particle time 



	m_LastParticleSpawn += sceneContext.pGameTime->GetElapsed();
	

	//3
	m_ActiveParticles = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	float deltaTime = sceneContext.pGameTime->GetElapsed();
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//D3D11_MAP_WRITE_DISCARD
	VertexParticle* pBuffer = static_cast<VertexParticle*>(mappedResource.pData); //cast to right type
	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		Particle& currentLookUp = (m_ParticlesArray[i]);
		if (currentLookUp.isActive) {
			//if active
			UpdateParticle(currentLookUp, deltaTime);
			
		}
		if (currentLookUp.isActive == false && m_LastParticleSpawn >= particleInterval) {
			//not active
			SpawnParticle(currentLookUp);
		}
		if (currentLookUp.isActive) {
			pBuffer[m_ActiveParticles] = currentLookUp.vertexInfo;
			m_ActiveParticles++;
		}
	}
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	if (p.isActive == false)
		return;

	//2
	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy < 0.f) {
		p.isActive = false;
		return;
	}

	//3
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, (XMLoadFloat3(&p.vertexInfo.Position) + (XMLoadFloat3(&m_EmitterSettings.velocity) * elapsedTime)));
	p.vertexInfo.Position = newPos;

	//4
	float lifePercent = p.currentEnergy / p.totalEnergy;
	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w *= lifePercent;

	//5

	p.vertexInfo.Size = std::lerp(p.initialSize, p.initialSize * p.sizeChange, 1.f - lifePercent);
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	p.isActive = true;

	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	//pos init
	XMFLOAT3 randomDir = { 1,0,0 };
	XMMATRIX randomMat = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));
	XMStoreFloat3(&randomDir, XMVector3TransformNormal(XMLoadFloat3(&randomDir), randomMat));
	//random distance
	float randomDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);
	XMVECTOR  gameobjectPos = XMLoadFloat3(&GetTransform()->GetPosition());

	XMStoreFloat3(&p.vertexInfo.Position, (XMLoadFloat3(&randomDir) * randomDistance) + gameobjectPos);
	//size init
	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = p.vertexInfo.Size;

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);
	//rot init
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);
	//color
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	auto world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	auto viewProj = XMLoadFloat4x4(&sceneContext.pCamera->GetViewProjection());//GetViewProjection
	XMFLOAT4X4 wvp;
	XMStoreFloat4x4(&wvp,world * viewProj);

	auto viewInv= sceneContext.pCamera->GetViewInverse();//GetViewProjection
	
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", viewInv);
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", wvp);



	auto techContext = m_pParticleMaterial->GetTechniqueContext();
	unsigned int stride = sizeof(VertexParticle);
	unsigned int offset = 0;
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	//gameContext.pDeviceContext->IASetIndexBuffer(, DXGI_FORMAT_R32_UINT, 0);
	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(techContext.pInputLayout);

	//Set Primitive Topology
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);


	//DRAW
	auto tech = techContext.pTechnique;
	D3DX11_TECHNIQUE_DESC techDesc{};

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}



}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}