// ImageRenderer.generated.h
#include "ImageRenderer.h"
#include "Editor/MTImGui.h"





	

// ============================================================================
// ImageRendererとImageRendererMementoの相互変換処理を実装
// ============================================================================


	ImageRendererMemento* mtgb::ImageRenderer::SaveToMemento()
	{ 
	OnPreSave(); 
		ImageRendererState state;
		state.imageFileName_ = this->imageFileName_;
		state.drawRect_ = this->drawRect_;
		state.uiParams_ = this->uiParams_;
		state.layer_ = this->layer_;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::ImageRenderer::RestoreFromMemento(const Memento& _memento) 
	{ 
		const ImageRendererState& state = _memento.GetState();
		this->imageFileName_ = state.imageFileName_;
		this->drawRect_ = state.drawRect_;
		this->uiParams_ = state.uiParams_;
		this->layer_ = state.layer_;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::ImageRenderer& _target) 
	{
		_j["imageFileName_"] = JsonConverter::Serialize<std::string>(_target.imageFileName_);
		_j["drawRect_"] = JsonConverter::Serialize<mtgb::RectF>(_target.drawRect_);
		_j["uiParams_"] = JsonConverter::Serialize<mtgb::UIParams>(_target.uiParams_);
		_j["layer_"] = JsonConverter::Serialize<mtgb::GameObjectLayerFlag>(_target.layer_);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::ImageRenderer& _target) 
	{
		JsonConverter::Deserialize<std::string>(_target.imageFileName_, _j,"imageFileName_");
		JsonConverter::Deserialize<mtgb::RectF>(_target.drawRect_, _j,"drawRect_");
		JsonConverter::Deserialize<mtgb::UIParams>(_target.uiParams_, _j,"uiParams_");
		JsonConverter::Deserialize<mtgb::GameObjectLayerFlag>(_target.layer_, _j,"layer_");
		_target.OnPostRestore(); 
	} 
	/* ImGui表示処理の登録 */ 
	void mtgb::ImageRenderer::RegisterImGui() 
	{ 
		static bool registered = false; 
		if (registered) return; 
		registered = true; 
	
		RegisterShowFuncHolder::Set<ImageRenderer>([]( ImageRenderer* _target, const char* _name)
			{
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->imageFileName_, "imageFileName_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->drawRect_, "drawRect_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->uiParams_, "uiParams_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->layer_, "layer_");
			});
		MTImGui::RegisterComponentViewer<ImageRenderer>();
	}