// RigidBody.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// RigidBodyの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
#define MT_COMPONENT_RigidBody() \
	struct RigidBodyState \
	{ \
			bool useGravity_; \
			bool isKinematic_; \
	}; \
	class RigidBody;\
	using RigidBodyMemento = ComponentMemento<RigidBody, RigidBodyState>;

// ============================================================================
// RigidBodyとRigidBodyMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_RigidBody() \
	public: \
	using Memento = RigidBodyMemento; \
	RigidBodyMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		RigidBodyState state; \
		state.useGravity_ = this->useGravity_; \
		state.isKinematic_ = this->isKinematic_; \
		return new ComponentMemento<RigidBody, RigidBodyState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<RigidBody, RigidBodyState>& _memento) \
	{ \
		const RigidBodyState& state = _memento.GetState(); \
		this->useGravity_ = state.useGravity_; \
		this->isKinematic_ = state.isKinematic_; \
		OnPostRestore(); \
	} \
	\
	friend struct RigidBody_Register; \
	friend void to_json(nlohmann::json& _j,RigidBody& _target) \
	{ \
	_target.OnPreSave(); \
		_j["useGravity_"] = JsonConverter::Serialize<bool>(_target.useGravity_); \
		_j["isKinematic_"] = JsonConverter::Serialize<bool>(_target.isKinematic_); \
	} \
	friend void from_json(const nlohmann::json& _j, RigidBody& _target) \
	{ \
		JsonConverter::Deserialize<bool>(_target.useGravity_, _j.at("useGravity_")); \
		JsonConverter::Deserialize<bool>(_target.isKinematic_, _j.at("isKinematic_")); \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "RigidBody" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<RigidBody>([]( RigidBody* _target, const char* _name) \
			{ \
				TypeRegistry::Instance().CallFunc(&_target->useGravity_, "useGravity_"); \
				TypeRegistry::Instance().CallFunc(&_target->isKinematic_, "isKinematic_"); \
			}); \
		MTImGui::Instance().RegisterComponentViewer<RigidBody>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_RigidBody()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_RigidBody()
#pragma warning(pop)