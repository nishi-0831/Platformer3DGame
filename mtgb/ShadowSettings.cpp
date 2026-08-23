#include "ShadowSettings.h"
#include "Components/Transform/Transform.h"
#include "Components/Collider/ColliderCP.h"
#include "DirectX11Draw.h"
#include "ShaderManager.h"
mtgb::ShadowSettings::ShadowSettings()
	: params {}
{
}

void mtgb::ShadowSettings::Initialize() {}

void mtgb::ShadowSettings::Update() {}

void mtgb::ShadowSettings::SetCaster(EntityId _id)
{
	Transform& casterTransform = Game::System<TransformCP>().Get(_id);

	params.casterPos =
		Vector4 { casterTransform.position.x, casterTransform.position.y, casterTransform.position.z, 1.0f };
}

void mtgb::ShadowSettings::SetCB()
{
	ReflectiveConstantBuffer* cBuf =
		Game::System<ShaderManager>().GetShader(ShaderType::BOX3_D).GetConstantBuffer("ShadowParam");
	if (cBuf != nullptr)
	{
		cBuf->SetConstantBuffer(params);
		cBuf->ApplyChanges(DirectX11Draw::pContext_.Get());
		cBuf->BindPS(DirectX11Draw::pContext_.Get());
	}
}

mtgb::ShadowParams::ShadowParams()
	: casterPos {}
	, softness { 1.0f }
{
}
