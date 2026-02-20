// RigidBodyCP.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// RigidBodyCPの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct RigidBodyCPState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class RigidBodyCP;
	}



	

using RigidBodyCPMemento = mtgb::ComponentMemento<mtgb::RigidBodyCP, RigidBodyCPState>;

// ============================================================================
// RigidBodyCPとRigidBodyCPMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_RigidBodyCP() \
	public: \
	using Memento = RigidBodyCPMemento; \
	RigidBodyCPMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		RigidBodyCPState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const RigidBodyCPState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct RigidBodyCP_Register; \
	friend void to_json(nlohmann::json& _j,const RigidBodyCP& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, RigidBodyCP& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "RigidBodyCP" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<RigidBodyCP>([]( RigidBodyCP* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<RigidBodyCP>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_RigidBodyCP()
#pragma warning(pop)