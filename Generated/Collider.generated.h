// Collider.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include <string>



// ============================================================================
// Colliderの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct ColliderState
{
		ColliderType colliderType_;
		bool isStatic_;
		ColliderTag colliderTag_;
		bool isTrigger_;
		mtgb::Vector3 center_;
		float radius_;
		mtgb::Vector3 extents_;
};

// クラスの前方宣言
	namespace mtgb
	{
		class Collider;
	}



	

using ColliderMemento = mtgb::ComponentMemento<mtgb::Collider, ColliderState>;

// ============================================================================
// ColliderとColliderMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_Collider() \
	public: \
	using Memento = ColliderMemento; \
	ColliderMemento* SaveToMemento(); \
	\
	void RestoreFromMemento(const Memento& _memento); \
	\
	friend struct Collider_Register; \
	friend void to_json(nlohmann::json& _j,const Collider& _target); \
	friend void from_json(const nlohmann::json& _j, Collider& _target); \
	\
	static std::string TypeName(){ return "Collider" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui(); \

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Collider()
#pragma warning(pop)