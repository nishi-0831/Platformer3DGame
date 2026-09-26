// Interpolator.generated.h
#include "../mtgb/Components/Interpolator/Interpolator.h"
#include "Editor/MTImGui.h"





	

// ============================================================================
// InterpolatorとInterpolatorMementoの相互変換処理を実装
// ============================================================================


	InterpolatorMemento* mtgb::Interpolator::SaveToMemento()
	{ 
	OnPreSave(); 
		InterpolatorState state;
		state.dir_ = this->dir_;
		state.startPos_ = this->startPos_;
		state.endPos_ = this->endPos_;
		state.speed_ = this->speed_;
		state.speedName_ = this->speedName_;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::Interpolator::RestoreFromMemento(const Memento& _memento) 
	{ 
		const InterpolatorState& state = _memento.GetState();
		this->dir_ = state.dir_;
		this->startPos_ = state.startPos_;
		this->endPos_ = state.endPos_;
		this->speed_ = state.speed_;
		this->speedName_ = state.speedName_;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::Interpolator& _target) 
	{
		_j["dir_"] = JsonConverter::Serialize<float>(_target.dir_);
		_j["startPos_"] = JsonConverter::Serialize<mtgb::Vector3>(_target.startPos_);
		_j["endPos_"] = JsonConverter::Serialize<mtgb::Vector3>(_target.endPos_);
		_j["speed_"] = JsonConverter::Serialize<float>(_target.speed_);
		_j["speedName_"] = JsonConverter::Serialize<std::string>(_target.speedName_);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::Interpolator& _target) 
	{
		JsonConverter::Deserialize<float>(_target.dir_, _j,"dir_");
		JsonConverter::Deserialize<mtgb::Vector3>(_target.startPos_, _j,"startPos_");
		JsonConverter::Deserialize<mtgb::Vector3>(_target.endPos_, _j,"endPos_");
		JsonConverter::Deserialize<float>(_target.speed_, _j,"speed_");
		JsonConverter::Deserialize<std::string>(_target.speedName_, _j,"speedName_");
		_target.OnPostRestore(); 
	}