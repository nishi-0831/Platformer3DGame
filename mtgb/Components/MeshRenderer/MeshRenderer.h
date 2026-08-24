#pragma once
#include "Editor/ReflectionMacro.h"
#include "Core/Component/IComponent.h"
#include "ISerializableObject.h"
#include "Core/Component/IComponentMemento.h"
#include "Components/MeshRenderer/MeshRendererCP.h"
#include "Handlers.h"
#include "Core/GameObject/GameObjectLayer.h"
#include "Graphics/ShaderType.h"
#include "Graphics/IRenderable.h"
#include "cmtgb.h"
#include "MeshRenderer.generated.h"

namespace fbxsdk
{
	class FbxAnimStack;
}
namespace mtgb
{
	class MeshRendererCP;

	/// <summary>
	/// メッシュとマテリアルを管理する描画コンポーネント
	/// </summary>
	class [[MT_COMPONENT()]] MeshRenderer : public IRenderable,
											public IComponent<MeshRendererCP, MeshRenderer>,
											public ISerializableObject
	{
	  public:
		MT_GENERATED_BODY()
		friend MeshRendererCP;

		MeshRenderer();
		MeshRenderer(EntityId _entityId);
		/// <summary>
		/// メッシュハンドルを設定
		/// </summary>
		void SetMesh(FBXModelHandle _meshHandle)
		{
			meshHandle = _meshHandle;
		}

		/// <summary>
		/// メッシュハンドルを取得
		/// </summary>
		FBXModelHandle GetMesh() const
		{
			return meshHandle;
		}

		/// <summary>
		/// 使用するシェーダーの種類を返す
		/// </summary>
		/// <returns></returns>
		ShaderType GetShaderType() const
		{
			return shaderType;
		}
		/// <summary>
		/// 描画可能かチェック
		/// </summary>
		// bool CanRender() const { return meshHandle != INVALID_HANDLE; }

		GameObjectLayerFlag GetLayer() const override
		{
			return layer;
		}
		void OnChangeMeshFileName();
		void Render() const override;
		bool CanRender() const override
		{
			return meshHandle != INVALID_HANDLE;
		};
		void SetFrame(int _frame);
		void SetAnimStack(fbxsdk::FbxAnimStack* _pAnimStack);
		[[MT_PROPERTY()]]
		std::string meshFileName;
		[[MT_PROPERTY(HideInEditor)]]
		FBXModelHandle meshHandle;
		[[MT_PROPERTY()]]
		GameObjectLayerFlag layer;
		[[MT_PROPERTY()]]
		ShaderType shaderType;

	  protected:
		void OnPostRestore() override;

	  private:
		int frame_;
		fbxsdk::FbxAnimStack* pAnimStack_;
	};

	using MeshRendererMemento = ComponentMemento<MeshRenderer, MeshRendererState>;
} // namespace mtgb