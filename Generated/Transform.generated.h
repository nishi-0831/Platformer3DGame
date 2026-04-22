// Transform.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include <string>



// ============================================================================
// Transformの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct TransformState
{
		mtgb::EntityId parent;
		mtgb::Vector3 position;
		mtgb::Vector3 scale;
		mtgb::Quaternion rotate;
};

// クラスの前方宣言
	namespace mtgb
	{
		class Transform;
	}



	

using TransformMemento = mtgb::ComponentMemento<mtgb::Transform, TransformState>;

// ============================================================================
// TransformとTransformMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_Transform() \
	public: \
	using Memento = TransformMemento; \
	TransformMemento* SaveToMemento(); \
	\
	void RestoreFromMemento(const Memento& _memento); \
	\
	friend struct Transform_Register; \
	friend void to_json(nlohmann::json& _j,const Transform& _target); \
	friend void from_json(const nlohmann::json& _j, Transform& _target); \
	\
	static std::string TypeName(){ return "Transform" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui(); \

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Transform()
#pragma warning(pop)