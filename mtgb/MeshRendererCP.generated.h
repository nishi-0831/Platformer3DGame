// MeshRendererCP.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// MeshRendererCPの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct MeshRendererCPState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class MeshRendererCP;
	}



	

using MeshRendererCPMemento = mtgb::ComponentMemento<mtgb::MeshRendererCP, MeshRendererCPState>;

// ============================================================================
// MeshRendererCPとMeshRendererCPMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_MeshRendererCP() \
	public: \
	using Memento = MeshRendererCPMemento; \
	MeshRendererCPMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		MeshRendererCPState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const MeshRendererCPState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct MeshRendererCP_Register; \
	friend void to_json(nlohmann::json& _j,const MeshRendererCP& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, MeshRendererCP& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "MeshRendererCP" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<MeshRendererCP>([]( MeshRendererCP* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<MeshRendererCP>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MeshRendererCP()
#pragma warning(pop)