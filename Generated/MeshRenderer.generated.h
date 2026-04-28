// MeshRenderer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include <string>



// ============================================================================
// MeshRendererの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct MeshRendererState
{
		std::string meshFileName;
		mtgb::FBXModelHandle meshHandle;
		mtgb::GameObjectLayerFlag layer;
		mtgb::ShaderType shaderType;
};

// クラスの前方宣言
	namespace mtgb
	{
		class MeshRenderer;
	}



	

using MeshRendererMemento = mtgb::ComponentMemento<mtgb::MeshRenderer, MeshRendererState>;

// ============================================================================
// MeshRendererとMeshRendererMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_MeshRenderer() \
	public: \
	using Memento = MeshRendererMemento; \
	MeshRendererMemento* SaveToMemento(); \
	\
	void RestoreFromMemento(const Memento& _memento); \
	\
	friend struct MeshRenderer_Register; \
	friend void to_json(nlohmann::json& _j,const MeshRenderer& _target); \
	friend void from_json(const nlohmann::json& _j, MeshRenderer& _target); \
	\
	static std::string TypeName(){ return "MeshRenderer" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui(); \

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MeshRenderer()
#pragma warning(pop)