// Collider.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
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
	ColliderMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		ColliderState state; \
		state.colliderType_ = this->colliderType_; \
		state.isStatic_ = this->isStatic_; \
		state.colliderTag_ = this->colliderTag_; \
		state.isTrigger_ = this->isTrigger_; \
		state.center_ = this->center_; \
		state.radius_ = this->radius_; \
		state.extents_ = this->extents_; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const ColliderState& state = _memento.GetState(); \
		this->colliderType_ = state.colliderType_; \
		this->isStatic_ = state.isStatic_; \
		this->colliderTag_ = state.colliderTag_; \
		this->isTrigger_ = state.isTrigger_; \
		this->center_ = state.center_; \
		this->radius_ = state.radius_; \
		this->extents_ = state.extents_; \
		OnPostRestore(); \
	} \
	\
	friend struct Collider_Register; \
	friend void to_json(nlohmann::json& _j,const Collider& _target) \
	{ \
		_j["colliderType_"] = JsonConverter::Serialize<ColliderType>(_target.colliderType_); \
		_j["isStatic_"] = JsonConverter::Serialize<bool>(_target.isStatic_); \
		_j["colliderTag_"] = JsonConverter::Serialize<ColliderTag>(_target.colliderTag_); \
		_j["isTrigger_"] = JsonConverter::Serialize<bool>(_target.isTrigger_); \
		_j["center_"] = JsonConverter::Serialize<mtgb::Vector3>(_target.center_); \
		_j["radius_"] = JsonConverter::Serialize<float>(_target.radius_); \
		_j["extents_"] = JsonConverter::Serialize<mtgb::Vector3>(_target.extents_); \
	} \
	friend void from_json(const nlohmann::json& _j, Collider& _target) \
	{ \
		JsonConverter::Deserialize<ColliderType>(_target.colliderType_, _j,"colliderType_"); \
		JsonConverter::Deserialize<bool>(_target.isStatic_, _j,"isStatic_"); \
		JsonConverter::Deserialize<ColliderTag>(_target.colliderTag_, _j,"colliderTag_"); \
		JsonConverter::Deserialize<bool>(_target.isTrigger_, _j,"isTrigger_"); \
		JsonConverter::Deserialize<mtgb::Vector3>(_target.center_, _j,"center_"); \
		JsonConverter::Deserialize<float>(_target.radius_, _j,"radius_"); \
		JsonConverter::Deserialize<mtgb::Vector3>(_target.extents_, _j,"extents_"); \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "Collider" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<Collider>([]( Collider* _target, const char* _name) \
			{ \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->colliderType_, "colliderType_"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->isStatic_, "isStatic_"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->colliderTag_, "colliderTag_"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->isTrigger_, "isTrigger_"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->center_, "center_"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->radius_, "radius_"); \
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->extents_, "extents_"); \
			}); \
		MTImGui::RegisterComponentViewer<Collider>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Collider()
#pragma warning(pop)