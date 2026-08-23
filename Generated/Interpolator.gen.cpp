// Interpolator.generated.h
#include "Interpolator.h"
#include "Editor/MTImGui.h"





	

// ============================================================================
// InterpolatorとInterpolatorMementoの相互変換処理を実装
// ============================================================================


	InterpolatorMemento* mtgb::Interpolator::SaveToMemento()
	{ 
	OnPreSave(); 
		InterpolatorState state;
		state.dir_ = this->dir_;
		state.elapsed_ = this->elapsed_;
		state.duration_ = this->duration_;
		state.startPos_ = this->startPos_;
		state.endPos_ = this->endPos_;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::Interpolator::RestoreFromMemento(const Memento& _memento) 
	{ 
		const InterpolatorState& state = _memento.GetState();
		this->dir_ = state.dir_;
		this->elapsed_ = state.elapsed_;
		this->duration_ = state.duration_;
		this->startPos_ = state.startPos_;
		this->endPos_ = state.endPos_;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::Interpolator& _target) 
	{
		_j["dir_"] = JsonConverter::Serialize<float>(_target.dir_);
		_j["elapsed_"] = JsonConverter::Serialize<float>(_target.elapsed_);
		_j["duration_"] = JsonConverter::Serialize<float>(_target.duration_);
		_j["startPos_"] = JsonConverter::Serialize<mtgb::Vector3>(_target.startPos_);
		_j["endPos_"] = JsonConverter::Serialize<mtgb::Vector3>(_target.endPos_);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::Interpolator& _target) 
	{
		JsonConverter::Deserialize<float>(_target.dir_, _j,"dir_");
		JsonConverter::Deserialize<float>(_target.elapsed_, _j,"elapsed_");
		JsonConverter::Deserialize<float>(_target.duration_, _j,"duration_");
		JsonConverter::Deserialize<mtgb::Vector3>(_target.startPos_, _j,"startPos_");
		JsonConverter::Deserialize<mtgb::Vector3>(_target.endPos_, _j,"endPos_");
		_target.OnPostRestore(); 
	} 
	/* ImGui表示処理の登録 */ 
	void mtgb::Interpolator::RegisterImGui() 
	{ 
		static bool registered = false; 
		if (registered) return; 
		registered = true; 
	
		RegisterShowFuncHolder::Set<Interpolator>([]( Interpolator* _target, const char* _name)
			{
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->dir_, "dir_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->elapsed_, "elapsed_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->duration_, "duration_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->startPos_, "startPos_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->endPos_, "endPos_");
			});
		MTImGui::RegisterComponentViewer<Interpolator>();
	}