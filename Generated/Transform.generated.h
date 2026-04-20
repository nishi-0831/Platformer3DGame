// Transform.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
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
	TransformMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		TransformState state; \
		state.parent = this->parent; \
		state.position = this->position; \
		state.scale = this->scale; \
		state.rotate = this->rotate; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const TransformState& state = _memento.GetState(); \
		this->parent = state.parent; \
		this->position = state.position; \
		this->scale = state.scale; \
		this->rotate = state.rotate; \
		OnPostRestore(); \
	} \
	\
	friend struct Transform_Register; \
	friend void to_json(nlohmann::json& _j,const Transform& _target) \
	{ \
		_j["parent"] = JsonConverter::Serialize<mtgb::EntityId>(_target.parent); \
		_j["position"] = JsonConverter::Serialize<mtgb::Vector3>(_target.position); \
		_j["scale"] = JsonConverter::Serialize<mtgb::Vector3>(_target.scale); \
		_j["rotate"] = JsonConverter::Serialize<mtgb::Quaternion>(_target.rotate); \
	} \
	friend void from_json(const nlohmann::json& _j, Transform& _target) \
	{ \
		JsonConverter::Deserialize<mtgb::EntityId>(_target.parent, _j,"parent"); \
		JsonConverter::Deserialize<mtgb::Vector3>(_target.position, _j,"position"); \
		JsonConverter::Deserialize<mtgb::Vector3>(_target.scale, _j,"scale"); \
		JsonConverter::Deserialize<mtgb::Quaternion>(_target.rotate, _j,"rotate"); \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "Transform" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<Transform>([]( Transform* _target, const char* _name) \
			{ \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->parent, "parent"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->position, "position"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->scale, "scale"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->rotate, "rotate"); \
			}); \
		MTImGui::RegisterComponentViewer<Transform>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Transform()
#pragma warning(pop)