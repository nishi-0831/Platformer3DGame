// Transform.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// Transformの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
#define MT_COMPONENT_Transform() \
	struct TransformState \
	{ \
			EntityId parent; \
			Vector3 position; \
			Vector3 scale; \
			Quaternion rotate; \
	}; \
	class Transform;\
	using TransformMemento = ComponentMemento<Transform, TransformState>;

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
		return new ComponentMemento<Transform, TransformState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<Transform, TransformState>& _memento) \
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
		_j["parent"] = JsonConverter::Serialize<EntityId>(_target.parent); \
		_j["position"] = JsonConverter::Serialize<Vector3>(_target.position); \
		_j["scale"] = JsonConverter::Serialize<Vector3>(_target.scale); \
		_j["rotate"] = JsonConverter::Serialize<Quaternion>(_target.rotate); \
	} \
	friend void from_json(const nlohmann::json& _j, Transform& _target) \
	{ \
		JsonConverter::Deserialize<EntityId>(_target.parent, _j.at("parent")); \
		JsonConverter::Deserialize<Vector3>(_target.position, _j.at("position")); \
		JsonConverter::Deserialize<Vector3>(_target.scale, _j.at("scale")); \
		JsonConverter::Deserialize<Quaternion>(_target.rotate, _j.at("rotate")); \
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
				TypeRegistry::Instance().CallFunc(&_target->parent, "parent"); \
				TypeRegistry::Instance().CallFunc(&_target->position, "position"); \
				TypeRegistry::Instance().CallFunc(&_target->scale, "scale"); \
				TypeRegistry::Instance().CallFunc(&_target->rotate, "rotate"); \
			}); \
		MTImGui::Instance().RegisterComponentViewer<Transform>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_Transform()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Transform()
#pragma warning(pop)