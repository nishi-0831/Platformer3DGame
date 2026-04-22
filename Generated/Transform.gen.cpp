// Transform.generated.h
#include "Transform.h"
#include "MTImGui.h"





	

// ============================================================================
// TransformとTransformMementoの相互変換処理を実装
// ============================================================================


	TransformMemento* mtgb::Transform::SaveToMemento()
	{ 
	OnPreSave(); 
		TransformState state;
		state.parent = this->parent;
		state.position = this->position;
		state.scale = this->scale;
		state.rotate = this->rotate;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::Transform::RestoreFromMemento(const Memento& _memento) 
	{ 
		const TransformState& state = _memento.GetState();
		this->parent = state.parent;
		this->position = state.position;
		this->scale = state.scale;
		this->rotate = state.rotate;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::Transform& _target) 
	{
		_j["parent"] = JsonConverter::Serialize<mtgb::EntityId>(_target.parent);
		_j["position"] = JsonConverter::Serialize<mtgb::Vector3>(_target.position);
		_j["scale"] = JsonConverter::Serialize<mtgb::Vector3>(_target.scale);
		_j["rotate"] = JsonConverter::Serialize<mtgb::Quaternion>(_target.rotate);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::Transform& _target) 
	{
		JsonConverter::Deserialize<mtgb::EntityId>(_target.parent, _j,"parent");
		JsonConverter::Deserialize<mtgb::Vector3>(_target.position, _j,"position");
		JsonConverter::Deserialize<mtgb::Vector3>(_target.scale, _j,"scale");
		JsonConverter::Deserialize<mtgb::Quaternion>(_target.rotate, _j,"rotate");
		_target.OnPostRestore(); 
	} 
	/* ImGui表示処理の登録 */ 
	void mtgb::Transform::RegisterImGui() 
	{ 
		static bool registered = false; 
		if (registered) return; 
		registered = true; 
	
		RegisterShowFuncHolder::Set<Transform>([]( Transform* _target, const char* _name)
			{
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->parent, "parent");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->position, "position");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->scale, "scale");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->rotate, "rotate");
			});
		MTImGui::RegisterComponentViewer<Transform>();
	}