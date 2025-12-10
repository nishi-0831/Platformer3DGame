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
		return new ComponentMemento<MovingFloor, MovingFloorState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<MovingFloor, MovingFloorState>& _memento) \
	{ \
		const MovingFloorState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct MovingFloor_Register; \
	friend void to_json(nlohmann::json& _j,MovingFloor& _target) \
	{ \
	_target.OnPreSave(); \
	} \
	friend void from_json(const nlohmann::json& _j, MovingFloor& _target) \
	{ \
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
			}); \
		MTImGui::Instance().RegisterComponentViewer<MovingFloor>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_MovingFloor()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MovingFloor()
#pragma warning(pop)