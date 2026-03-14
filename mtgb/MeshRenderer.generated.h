// MeshRenderer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
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
#define MT_GENERATED_BODY_MeshRenderer()                                                                  \
  public:                                                                                                 \
	using Memento = MeshRendererMemento;                                                                  \
	MeshRendererMemento* SaveToMemento()                                                                  \
	{                                                                                                     \
		OnPreSave();                                                                                      \
		MeshRendererState state;                                                                          \
		state.meshFileName = this->meshFileName;                                                          \
		state.meshHandle   = this->meshHandle;                                                            \
		state.layer		   = this->layer;                                                                 \
		state.shaderType   = this->shaderType;                                                            \
		return new Memento(GetEntityId(), state);                                                         \
	}                                                                                                     \
                                                                                                          \
	void RestoreFromMemento(const Memento& _memento)                                                      \
	{                                                                                                     \
		const MeshRendererState& state = _memento.GetState();                                             \
		this->meshFileName			   = state.meshFileName;                                              \
		this->meshHandle			   = state.meshHandle;                                                \
		this->layer					   = state.layer;                                                     \
		this->shaderType			   = state.shaderType;                                                \
		OnPostRestore();                                                                                  \
	}                                                                                                     \
                                                                                                          \
	friend struct MeshRenderer_Register;                                                                  \
	friend void to_json(nlohmann::json& _j, const MeshRenderer& _target)                                  \
	{                                                                                                     \
		_j["meshFileName"] = JsonConverter::Serialize<std::string>(_target.meshFileName);                 \
		_j["meshHandle"]   = JsonConverter::Serialize<mtgb::FBXModelHandle>(_target.meshHandle);          \
		_j["layer"]		   = JsonConverter::Serialize<mtgb::GameObjectLayerFlag>(_target.layer);          \
		_j["shaderType"]   = JsonConverter::Serialize<mtgb::ShaderType>(_target.shaderType);              \
	}                                                                                                     \
	friend void from_json(const nlohmann::json& _j, MeshRenderer& _target)                                \
	{                                                                                                     \
		JsonConverter::Deserialize<std::string>(_target.meshFileName, _j, "meshFileName");                \
		JsonConverter::Deserialize<mtgb::FBXModelHandle>(_target.meshHandle, _j, "meshHandle");           \
		JsonConverter::Deserialize<mtgb::GameObjectLayerFlag>(_target.layer, _j, "layer");                \
		JsonConverter::Deserialize<mtgb::ShaderType>(_target.shaderType, _j, "shaderType");               \
		_target.OnPostRestore();                                                                          \
	}                                                                                                     \
	static std::string TypeName()                                                                         \
	{                                                                                                     \
		return "MeshRenderer";                                                                            \
	}                                                                                                     \
	/* ImGui表示処理の登録 */                                                                             \
	static void RegisterImGui()                                                                           \
	{                                                                                                     \
		static bool registered = false;                                                                   \
		if (registered)                                                                                   \
			return;                                                                                       \
		registered = true;                                                                                \
                                                                                                          \
		RegisterShowFuncHolder::Set<MeshRenderer>(                                                        \
			[](MeshRenderer* _target, const char* _name)                                                  \
			{                                                                                             \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->meshFileName, "meshFileName"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->meshHandle, "meshHandle");     \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->layer, "layer");               \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->shaderType, "shaderType");     \
			}                                                                                             \
		);                                                                                                \
		MTImGui::RegisterComponentViewer<MeshRenderer>();                                                 \
	}

#pragma warning(push)
#pragma warning(disable : 4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MeshRenderer()
#pragma warning(pop)