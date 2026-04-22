// RigidBody.generated.h
#include "RigidBody.h"
#include "MTImGui.h"





	

// ============================================================================
// RigidBodyとRigidBodyMementoの相互変換処理を実装
// ============================================================================


	RigidBodyMemento* mtgb::RigidBody::SaveToMemento()
	{ 
	OnPreSave(); 
		RigidBodyState state;
		state.useGravity_ = this->useGravity_;
		state.isKinematic_ = this->isKinematic_;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::RigidBody::RestoreFromMemento(const Memento& _memento) 
	{ 
		const RigidBodyState& state = _memento.GetState();
		this->useGravity_ = state.useGravity_;
		this->isKinematic_ = state.isKinematic_;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::RigidBody& _target) 
	{
		_j["useGravity_"] = JsonConverter::Serialize<bool>(_target.useGravity_);
		_j["isKinematic_"] = JsonConverter::Serialize<bool>(_target.isKinematic_);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::RigidBody& _target) 
	{
		JsonConverter::Deserialize<bool>(_target.useGravity_, _j,"useGravity_");
		JsonConverter::Deserialize<bool>(_target.isKinematic_, _j,"isKinematic_");
		_target.OnPostRestore(); 
	} 
	/* ImGui表示処理の登録 */ 
	void mtgb::RigidBody::RegisterImGui() 
	{ 
		static bool registered = false; 
		if (registered) return; 
		registered = true; 
	
		RegisterShowFuncHolder::Set<RigidBody>([]( RigidBody* _target, const char* _name)
			{
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->useGravity_, "useGravity_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->isKinematic_, "isKinematic_");
			});
		MTImGui::RegisterComponentViewer<RigidBody>();
	}