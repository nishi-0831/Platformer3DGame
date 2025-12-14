// Interpolator.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// Interpolatorの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
#define MT_COMPONENT_Interpolator() \
	struct InterpolatorState \
	{ \
			float dir_; \
			float elapsed_; \
			float duration_; \
			Vector3 startPos_; \
			Vector3 endPos_; \
	}; \
	class Interpolator;\
	using InterpolatorMemento = ComponentMemento<Interpolator, InterpolatorState>;

// ============================================================================
// InterpolatorとInterpolatorMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_Interpolator() \
	public: \
	using Memento = InterpolatorMemento; \
	InterpolatorMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		InterpolatorState state; \
		state.dir_ = this->dir_; \
		state.elapsed_ = this->elapsed_; \
		state.duration_ = this->duration_; \
		state.startPos_ = this->startPos_; \
		state.endPos_ = this->endPos_; \
		return new ComponentMemento<Interpolator, InterpolatorState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<Interpolator, InterpolatorState>& _memento) \
	{ \
		const InterpolatorState& state = _memento.GetState(); \
		this->dir_ = state.dir_; \
		this->elapsed_ = state.elapsed_; \
		this->duration_ = state.duration_; \
		this->startPos_ = state.startPos_; \
		this->endPos_ = state.endPos_; \
		OnPostRestore(); \
	} \
	\
	friend struct Interpolator_Register; \
	friend void to_json(nlohmann::json& _j,const Interpolator& _target) \
	{ \
		_j["dir_"] = JsonConverter::Serialize<float>(_target.dir_); \
		_j["elapsed_"] = JsonConverter::Serialize<float>(_target.elapsed_); \
		_j["duration_"] = JsonConverter::Serialize<float>(_target.duration_); \
		_j["startPos_"] = JsonConverter::Serialize<Vector3>(_target.startPos_); \
		_j["endPos_"] = JsonConverter::Serialize<Vector3>(_target.endPos_); \
	} \
	friend void from_json(const nlohmann::json& _j, Interpolator& _target) \
	{ \
		JsonConverter::Deserialize<float>(_target.dir_, _j.at("dir_")); \
		JsonConverter::Deserialize<float>(_target.elapsed_, _j.at("elapsed_")); \
		JsonConverter::Deserialize<float>(_target.duration_, _j.at("duration_")); \
		JsonConverter::Deserialize<Vector3>(_target.startPos_, _j.at("startPos_")); \
		JsonConverter::Deserialize<Vector3>(_target.endPos_, _j.at("endPos_")); \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "Interpolator" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<Interpolator>([]( Interpolator* _target, const char* _name) \
			{ \
				TypeRegistry::Instance().CallFunc(&_target->dir_, "dir_"); \
				TypeRegistry::Instance().CallFunc(&_target->elapsed_, "elapsed_"); \
				TypeRegistry::Instance().CallFunc(&_target->duration_, "duration_"); \
				TypeRegistry::Instance().CallFunc(&_target->startPos_, "startPos_"); \
				TypeRegistry::Instance().CallFunc(&_target->endPos_, "endPos_"); \
			}); \
		MTImGui::Instance().RegisterComponentViewer<Interpolator>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_Interpolator()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Interpolator()
#pragma warning(pop)