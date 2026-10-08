#include "ShadowSettings.h"
#include "Components/Transform/Transform.h"
#include "Components/Collider/ColliderCP.h"
#include "Graphics/DirectX11Draw.h"
#include "ShaderManager.h"
#include <iterator>
#include <type_traits>

namespace
{
	constexpr int MAX_CASTER_COUNT { 128 };
}
mtgb::ShadowSettings::ShadowSettings()
	: shadowParams_ {}
{
	shadowParams_.casters.reserve(MAX_CASTER_COUNT);
}

void mtgb::ShadowSettings::Initialize() {}

void mtgb::ShadowSettings::Update()
{
	shadowParams_.casters.clear();

	shadowParams_.casterCount = 0;
}

void mtgb::ShadowSettings::AddCaster(EntityId _id, float _radius)
{
	Transform& casterTransform = Game::System<TransformCP>().Get(_id);
	const Vector4 casterPosition { casterTransform.position.x,
								   casterTransform.position.y,
								   casterTransform.position.z,
								   1.0f };
	if (shadowParams_.casters.size() < MAX_CASTER_COUNT)
	{
		shadowParams_.casters.push_back({ casterPosition, _radius });
		shadowParams_.casterCount = shadowParams_.casters.size();
	}
}

void mtgb::ShadowSettings::SetCB()
{
	ReflectiveConstantBuffer* cBuf =
		Game::System<ShaderManager>().GetShader(ShaderType::BOX3_D).GetConstantBuffer("ShadowParam");
	if (cBuf != nullptr)
	{
		// ステージ編集モードの場合、丸影は描画しない
		if (Game::IsEditMode())
		{
			std::span<const Caster> emptySpan {};
			cBuf->SetVariableArray("casters", emptySpan);
			cBuf->SetVariable("casterCount", 0);
		}
		else
		{
			std::span<const Caster> casterSpan { shadowParams_.casters.data(), shadowParams_.casters.size() };
			cBuf->SetVariableArray("casters", casterSpan);
			cBuf->SetVariable("casterCount", shadowParams_.casterCount);
		}
		cBuf->ApplyChanges(DirectX11Draw::pContext_.Get());
		cBuf->BindPS(DirectX11Draw::pContext_.Get());
	}
}

mtgb::ShadowSettings::Caster::Caster(const Vector4& _pos, float _radius)
	: pos { _pos }
	, radius { _radius }
	, padding {}
{
}

ShadowSettings::ShadowParam mtgb::ShadowSettings::ShadowParam::Disabled()
{
	ShadowParam param;
	param.casterCount = 0;
	return param;
}