// ColliderCP.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// ColliderCPの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct ColliderCPState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class ColliderCP;
	}



	

using ColliderCPMemento = mtgb::ComponentMemento<mtgb::ColliderCP, ColliderCPState>;

// ============================================================================
// ColliderCPとColliderCPMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_ColliderCP() \
	public: \
	using Memento = ColliderCPMemento; \
	ColliderCPMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		ColliderCPState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const ColliderCPState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct ColliderCP_Register; \
	friend void to_json(nlohmann::json& _j,const ColliderCP& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, ColliderCP& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "ColliderCP" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<ColliderCP>([]( ColliderCP* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<ColliderCP>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_ColliderCP()
#pragma warning(pop)