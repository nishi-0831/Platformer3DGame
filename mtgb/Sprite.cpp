#include "Sprite.h"
#include "Transform.h"
#include "Debug.h"
#include "CompileShaderUtility.h"
#include "HLSLInclude.h"
mtgb::Sprite::Sprite()
	: texture2D_ {}
{
}

mtgb::Sprite::~Sprite() {}

void mtgb::Sprite::Initialize()
{
	InitializeVertexBuffer();
	InitializeIndexBuffer();
	InitializeConstantBuffer();
	InitializeShader();
}

void mtgb::Sprite::Load(const std::wstring& _fileName)
{
	fileName_ = _fileName;
	texture2D_.Load(_fileName);
}

void mtgb::Sprite::Draw(const RectF& _draw, const float _rotationZ, const RectF& _cut, const Color& _color)
{
	using DirectX::XMMatrixIdentity;	// 単位行列
	using DirectX::XMMatrixRotationZ;	// Z軸の回転行列
	using DirectX::XMMatrixScaling;		// 拡縮
	using DirectX::XMMatrixTranslation; // 平行移動
	using DirectX::XMMatrixTranspose;	// 行と列を入れ替える

	DirectX11Draw::pContext_->RSSetState(pRasterizerState.Get());
	DirectX11Draw::pContext_->VSSetShader(pVertexShader.Get(), nullptr, 0);
	DirectX11Draw::pContext_->PSSetShader(pPixelShader.Get(), nullptr, 0);
	DirectX11Draw::pContext_->IASetInputLayout(pVertexLayout.Get());
	DirectX11Draw::SetIsWriteToDepthBuffer(false); // 深度バッファへの書き込みなし
	DirectX11Draw::SetBlendMode(BlendMode::SPRITE);

	ConstantBuffer cb;
	cb.g_color				   = _color.ToVector4Norm();
	cb.g_angle				   = {};
	cb.g_matrixCameraRotation  = XMMatrixIdentity(); // カメラは無し = UI座標
	cb.g_matrixCameraTranslate = XMMatrixIdentity();
	cb.g_matrixWorldRotation   = XMMatrixRotationZ(_rotationZ);

	// スクリーンサイズを取得
	const Vector2Int SCREEN_SIZE { Game::System<Screen>().GetSize() };

	// 数学座標と描画座標のy軸差異解消
	RectF cartesianBox { _draw };
	cartesianBox.y = SCREEN_SIZE.y - cartesianBox.y;
	cartesianBox.height *= -1;

	const Vector2F VIEW_BEGIN { cartesianBox.GetBegin() };
	const Vector2F VIEW_END { cartesianBox.GetEnd() };

	// 表示するサイズに合わせる
	Matrix4x4 scalingBox =
		XMMatrixScaling(std::abs(VIEW_END.x - VIEW_BEGIN.x) * 2.0f, std::abs(VIEW_END.y - VIEW_BEGIN.y) * 2.0f, 1.0f);

	// 表示するボックスの位置を移動する
	Matrix4x4 moveBox = XMMatrixTranslation(
		((VIEW_END.x - VIEW_BEGIN.x) / 2.0f + VIEW_BEGIN.x) / (SCREEN_SIZE.x / 2.0f),
		((VIEW_BEGIN.y - VIEW_END.y) / 2.0f + VIEW_END.y) / (SCREEN_SIZE.y / 2.0f),
		0.0f
	);

	// 画面に合わせる
	Matrix4x4 scalingView = XMMatrixScaling(1.0f / (SCREEN_SIZE.x * 2), 1.0f / (SCREEN_SIZE.y * 2), 1.0f);

	// オフセット - 画面中心は(0, 0) 左下は(-1, -1)
	Matrix4x4 offsetView { XMMatrixTranslation(-1.0f, -1.0f, 0.0f) };

	// 最終的な行列
	Matrix4x4 world { scalingBox * scalingView * moveBox * offsetView };

	cb.g_matrixWorldTranslate = XMMatrixTranspose(world);

	// トリミング計算

	const Vector2F CUT_BEGIN { _cut.GetBegin() };
	const Vector2F CUT_END { _cut.GetEnd() };

	// トリミング矩形の左上点を並行移動
	Matrix4x4 uvMove = XMMatrixTranslation(
		CUT_BEGIN.x * 1.0f / texture2D_.GetSize().x,
		CUT_BEGIN.y * 1.0f / texture2D_.GetSize().y,
		0.0f
	);

	// トリミング矩形の拡縮
	Matrix4x4 uvScaling = XMMatrixScaling(
		static_cast<float>(CUT_END.x) / texture2D_.GetSize().x,
		static_cast<float>(CUT_END.y) / texture2D_.GetSize().y,
		1.0f
	);

	Matrix4x4 uv { uvScaling * uvMove };

	cb.g_matrixTexture = XMMatrixTranspose(uv);

	UINT stride { 0U };
	UINT offset { 0U };

	stride = sizeof(Vertex);
	offset = 0;
	DirectX11Draw::pContext_->IASetVertexBuffers(
		0U,
		1U,
		pVertexBuffer_.GetAddressOf(),
		&stride,
		&offset
	); // 頂点バッファをセット
	DirectX11Draw::pContext_->IASetIndexBuffer(
		pIndexBuffer_.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	); // インデックスバッファをセット
	DirectX11Draw::pContext_->VSSetConstantBuffers(
		0,
		1,
		pConstantBuffer_.GetAddressOf()
	); // 頂点シェーダのコンスタントバッファをセット
	DirectX11Draw::pContext_->PSSetConstantBuffers(
		0,
		1,
		pConstantBuffer_.GetAddressOf()
	); // ピクセルシェーダのコンスタントバッファをセット

	HRESULT hResult {};

	// シェーダに渡すためのデータ
	D3D11_MAPPED_SUBRESOURCE mappedSubresource {};

	// GPUからのデータアクセスをせき止める
	hResult =
		DirectX11Draw::pContext_->Map(pConstantBuffer_.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);

	massert(
		SUCCEEDED(hResult) // GPUデータアクセスせき止めに成功
		&& "GPUデータアクセスせき止めに失敗"
	);

	// データ書き込み
	memcpy_s(mappedSubresource.pData, mappedSubresource.RowPitch, reinterpret_cast<void*>(&cb), sizeof(ConstantBuffer));
	// GPUデータアクセスせき止め解除
	DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);
	ID3D11SamplerState* pSamplerState { texture2D_.GetSamplerState() };
	DirectX11Draw::pContext_.Get()->PSSetSamplers(0, 1, &pSamplerState);

	ID3D11ShaderResourceView* pSRV { texture2D_.GetShaderResourceView() };
	DirectX11Draw::pContext_.Get()->PSSetShaderResources(0, 1, &pSRV);

	DirectX11Draw::pContext_->DrawIndexed(6, 0, 0);
}

void mtgb::Sprite::InitializeVertexBuffer()
{
	Vertex vertexes[] {
		{ Vector4 { -1, 1, 0, 1 }, Vector4 { 0, 0, 0, 0 } },  // 左上
		{ Vector4 { 1, 1, 0, 1 }, Vector4 { 1, 0, 0, 0 } },	  // 右上
		{ Vector4 { -1, -1, 0, 1 }, Vector4 { 0, 1, 0, 0 } }, // 左下
		{ Vector4 { 1, -1, 0, 1 }, Vector4 { 1, 1, 0, 0 } },  // 右下
	};

	const D3D11_BUFFER_DESC BUFFER_DESC {
		.ByteWidth			 = sizeof(vertexes),
		.Usage				 = D3D11_USAGE_DEFAULT,
		.BindFlags			 = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags		 = 0,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA {
		.pSysMem		  = vertexes,
		.SysMemPitch	  = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult {};
	hResult =
		DirectX11Draw::pDevice_->CreateBuffer(&BUFFER_DESC, &INITIALIZE_DATA, pVertexBuffer_.ReleaseAndGetAddressOf());

	massert(
		SUCCEEDED(hResult) // 頂点バッファの作成に成功
		&& "頂点バッファの作成に失敗 @Sprite::InitializeVertexBuffer"
	);
}

void mtgb::Sprite::InitializeIndexBuffer()
{
	static const int INDEXES[] { 2, 1, 0, 2, 3, 1 };

	const D3D11_BUFFER_DESC BUFFER_DESC {
		.ByteWidth			 = sizeof(INDEXES),
		.Usage				 = D3D11_USAGE_DEFAULT,
		.BindFlags			 = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags		 = 0,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA {
		.pSysMem		  = INDEXES,
		.SysMemPitch	  = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult {};
	hResult =
		DirectX11Draw::pDevice_->CreateBuffer(&BUFFER_DESC, &INITIALIZE_DATA, pIndexBuffer_.ReleaseAndGetAddressOf());

	massert(
		SUCCEEDED(hResult) // インデックスバッファの作成に成功
		&& "インデックスバッファの作成に失敗 @Sprite::InitializeIndexBuffer"
	);
}

void mtgb::Sprite::InitializeConstantBuffer()
{
	const D3D11_BUFFER_DESC BUFFER_DESC {
		.ByteWidth			 = sizeof(ConstantBuffer),
		.Usage				 = D3D11_USAGE_DYNAMIC, // MEMO: 途中で書き換えるためdynamic
		.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult {};
	hResult = DirectX11Draw::pDevice_->CreateBuffer(
		&BUFFER_DESC,
		nullptr, // 初期データなし
		pConstantBuffer_.ReleaseAndGetAddressOf()
	);

	massert(SUCCEEDED(hResult) && "コンスタントバッファの作成に失敗 @Sprite::InitializeConstantBuffer");
}

void mtgb::Sprite::InitializeShader()
{
	HLSLInclude hlslInclude {};
	HRESULT hResult {};
	const wchar_t* fileName = L"Shader/Sprite.hlsl";
	// 項点シェーダのインタフェース
	ID3DBlob* pCompileVS { nullptr };
	// 頂点シェーダのコンパイル
	hResult = D3DCompileFromFile(
		fileName,	  // ファイルパス
		nullptr,	  // シェーダマクロの配列
		&hlslInclude, // インクルードするやつ
		"VS",		  // エントリポイントの関数名
		"vs_5_0",	  // シェーダのバージョン (オプションで付けるやつ)
		0,			  // オプションフラグ1
		0,			  // オプションフラグ2
		&pCompileVS,  // コンパイル済みコードへのアクセスインタフェース
		nullptr
	); // エラーメッセージ受信用 無し
	massert(
		SUCCEEDED(hResult) // 頂点シェーダのコンパイルに成功
		&& "頂点シェーダのコンパイルに失敗 @IShader::CompileShader"
	);

	pVertexLayout.Attach(CreateInputLayout(DirectX11Draw::pDevice_.Get(), pCompileVS));
	// ピクセルシェーダを作成し、指定タイプのバンドルに格納する
	hResult = DirectX11Draw::pDevice_->CreateVertexShader(
		pCompileVS->GetBufferPointer(), // コンパイルされたバッファのポインタ
		pCompileVS->GetBufferSize(),	// バッファのサイズ
		nullptr,						// リンケージクラス: 無し
		pVertexShader.ReleaseAndGetAddressOf()
	);
	massert(
		SUCCEEDED(hResult) // 頂点シェーダの作成に成功
		&& "頂点シェーダの作成に失敗 @IShader::CompileShader"
	);

	// ピクセルシェーダのインタフェース
	ID3DBlob* pCompilePS { nullptr };

	// ピクセルシェーダのコンパイル
	hResult = D3DCompileFromFile(
		fileName,	  // ファイルパス
		nullptr,	  // シェーダマクロの配列
		&hlslInclude, // インクルードするやつ
		"PS",		  // エントリポイントの関数名
		"ps_5_0",	  // シェーダのバージョン (オプションで付けるやつ)
		0,			  // オプションフラグ1
		0,			  // オプションフラグ2
		&pCompilePS,  // コンパイル済みコードへのアクセスインタフェース
		nullptr
	); // エラーメッセージ受信用 無し

	massert(
		SUCCEEDED(hResult) // ピクセルシェーダのコンパイルに成功
		&& "ピクセルシェーダのコンパイルに失敗 @IShader::CompileShader"
	);

	// ピクセルシェーダを作成し、指定タイプのバンドルに格納する
	hResult = DirectX11Draw::pDevice_->CreatePixelShader(
		pCompilePS->GetBufferPointer(), // コンパイルされたバッファのポインタ
		pCompilePS->GetBufferSize(),	// バッファのサイズ
		nullptr,						// リンケージクラス: 無し
		pPixelShader.ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // ピクセルシェーダの作成に成功
		&& "ピクセルシェーダの作成に失敗 @IShader::CompileShader"
	);

	CD3D11_RASTERIZER_DESC cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC {
		.FillMode			   = D3D11_FILL_SOLID, // 塗りつぶし: solid
		.CullMode			   = D3D11_CULL_NONE,  // カリング:
		.FrontCounterClockwise = FALSE,			   // 三角形の正面向き = 時計回り
		.DepthBias			   = {},
		.DepthBiasClamp		   = {},
		.SlopeScaledDepthBias  = {},
		.DepthClipEnable	   = true, // クリッピングを有効にする
		.ScissorEnable		   = {},
		.MultisampleEnable	   = {},
		.AntialiasedLineEnable = {},
	});
	hResult =
		DirectX11Draw::pDevice_->CreateRasterizerState(&cRasterizerDesc, pRasterizerState.ReleaseAndGetAddressOf());
	massert(
		SUCCEEDED(hResult) // ラスタライザーステート作成に成功
		&& "ラスタライザーステート作成に失敗 @IShader::CompileShader"
	);
	SAFE_RELEASE(pCompileVS);
	SAFE_RELEASE(pCompilePS);
}
