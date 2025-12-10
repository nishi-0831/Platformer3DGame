// MovingFloor.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// MovingFloorの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
#define MT_COMPONENT_MovingFloor() \
	struct MovingFloorState \
	{ \
			float duration_; \
			Vector3 to_; \
			Vector3 from_; \
	}; \
	class MovingFloor;\
	using MovingFloorMemento = ComponentMemento<MovingFloor, MovingFloorState>;

// ============================================================================
// MovingFloorとMovingFloorMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_MovingFloor() \
	public: \
	using Memento = MovingFloorMemento; \
	MovingFloorMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		MovingFloorState state; \
		state.duration_ = this->duration_; \
		state.to_ = this->to_; \
		state.from_ = this->from_; \
		return new ComponentMemento<MovingFloor, MovingFloorState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<MovingFloor, MovingFloorState>& _memento) \
	{ \
		const MovingFloorState& state = _memento.GetState(); \
		this->duration_ = state.duration_; \
		this->to_ = state.to_; \
		this->from_ = state.from_; \
		OnPostRestore(); \
	} \
	\
	friend struct MovingFloor_Register; \
	friend void to_json(nlohmann::json& _j,const MovingFloor& _target) \
	{ \
		_j["duration_"] = JsonConverter::Serialize<float>(_target.duration_); \
		_j["to_"] = JsonConverter::Serialize<Vector3>(_target.to_); \
		_j["from_"] = JsonConverter::Serialize<Vector3>(_target.from_); \
	} \
	friend void from_json(const nlohmann::json& _j, MovingFloor& _target) \
	{ \
		JsonConverter::Deserialize<float>(_target.duration_, _j.at("duration_")); \
		JsonConverter::Deserialize<Vector3>(_target.to_, _j.at("to_")); \
		JsonConverter::Deserialize<Vector3>(_target.from_, _j.at("from_")); \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "MovingFloor" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<MovingFloor>([]( MovingFloor* _target, const char* _name) \
			{ \
				TypeRegistry::Instance().CallFunc(&_target->duration_, "duration_"); \
				TypeRegistry::Instance().CallFunc(&_target->to_, "to_"); \
				TypeRegistry::Instance().CallFunc(&_target->from_, "from_"); \
			}); \
		MTImGui::Instance().RegisterComponentViewer<MovingFloor>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_MovingFloor()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MovingFloor()
#pragma warning(pop)