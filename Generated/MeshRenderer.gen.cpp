// MeshRenderer.generated.h
#include "MeshRenderer.h"
#include "Editor/MTImGui.h"





	

// ============================================================================
// MeshRendererとMeshRendererMementoの相互変換処理を実装
// ============================================================================


	MeshRendererMemento* mtgb::MeshRenderer::SaveToMemento()
	{ 
	OnPreSave(); 
		MeshRendererState state;
		state.meshFileName = this->meshFileName;
		state.meshHandle = this->meshHandle;
		state.layer = this->layer;
		state.shaderType = this->shaderType;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::MeshRenderer::RestoreFromMemento(const Memento& _memento) 
	{ 
		const MeshRendererState& state = _memento.GetState();
		this->meshFileName = state.meshFileName;
		this->meshHandle = state.meshHandle;
		this->layer = state.layer;
		this->shaderType = state.shaderType;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::MeshRenderer& _target) 
	{
		_j["meshFileName"] = JsonConverter::Serialize<std::string>(_target.meshFileName);
		_j["meshHandle"] = JsonConverter::Serialize<mtgb::FBXModelHandle>(_target.meshHandle);
		_j["layer"] = JsonConverter::Serialize<mtgb::GameObjectLayerFlag>(_target.layer);
		_j["shaderType"] = JsonConverter::Serialize<mtgb::ShaderType>(_target.shaderType);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::MeshRenderer& _target) 
	{
		JsonConverter::Deserialize<std::string>(_target.meshFileName, _j,"meshFileName");
		JsonConverter::Deserialize<mtgb::FBXModelHandle>(_target.meshHandle, _j,"meshHandle");
		JsonConverter::Deserialize<mtgb::GameObjectLayerFlag>(_target.layer, _j,"layer");
		JsonConverter::Deserialize<mtgb::ShaderType>(_target.shaderType, _j,"shaderType");
		_target.OnPostRestore(); 
	} 
	/* ImGui表示処理の登録 */ 
	void mtgb::MeshRenderer::RegisterImGui() 
	{ 
		static bool registered = false; 
		if (registered) return; 
		registered = true; 
	
		RegisterShowFuncHolder::Set<MeshRenderer>([]( MeshRenderer* _target, const char* _name)
			{
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->meshFileName, "meshFileName");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->layer, "layer");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->shaderType, "shaderType");
			});
		MTImGui::RegisterComponentViewer<MeshRenderer>();
	}