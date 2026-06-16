#include "ShadowSettings.h"
#include "Transform.h"
#include "ColliderCP.h"
#include "DirectX11Draw.h"
mtgb::ShadowSettings::ShadowSettings()
	: params {}
	, shadowCastDistance_ { 100.0f }
{
}

void mtgb::ShadowSettings::Initialize() 
{
	
	const D3D11_BUFFER_DESC BUFFER_DESC{
		.ByteWidth = sizeof(ShadowParams),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult {};
	hResult = DirectX11Draw::pDevice_->CreateBuffer(
		&BUFFER_DESC,
		nullptr, // 初期データなし
		pShadowCB_.ReleaseAndGetAddressOf()
	);
	massert(SUCCEEDED(hResult) && "コンスタントバッファの作成に失敗");
}

void mtgb::ShadowSettings::Update() {}

void mtgb::ShadowSettings::SetCaster(EntityId _id) 
{
	Transform& casterTransform = Game::System<TransformCP>().Get(_id);

	float distance = shadowCastDistance_;
	EntityId nearestStageObjectId = Game::System<ColliderCP>()
		.RayCastHitAll(casterTransform.position, Vector3::Down(), &distance, ColliderTag::STAGE);

	params.casterPos   = Vector4 { casterTransform.position.x, casterTransform.position.y, casterTransform.position.z,1.0f };
	params.playerHeight = casterTransform.position.y - distance;
}

void mtgb::ShadowSettings::SetCB() 
{
	DirectX11Draw::SetShader(ShaderType::BOX3_D);

	DirectX11Draw::pContext_->PSSetConstantBuffers(1, 1, pShadowCB_.GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE pData_;
	DirectX11Draw::pContext_->Map(pShadowCB_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData_);
	memcpy_s(pData_.pData, pData_.RowPitch, (void*)(&params), sizeof(params));
	DirectX11Draw::pContext_->Unmap(pShadowCB_.Get(), 0);
}

mtgb::ShadowParams::ShadowParams()
	: casterPos {}
	, softness {0.5f}
	, playerHeight {0.0f}
{
}
