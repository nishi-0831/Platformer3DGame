// TextRenderer.generated.h
#include "TextRenderer.h"
#include "Editor/MTImGui.h"





	

// ============================================================================
// TextRendererとTextRendererMementoの相互変換処理を実装
// ============================================================================


	TextRendererMemento* mtgb::TextRenderer::SaveToMemento()
	{ 
	OnPreSave(); 
		TextRendererState state;
		state.alignment = this->alignment;
		state.text_ = this->text_;
		state.rect_ = this->rect_;
		state.fontSize_ = this->fontSize_;
		state.params_ = this->params_;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::TextRenderer::RestoreFromMemento(const Memento& _memento) 
	{ 
		const TextRendererState& state = _memento.GetState();
		this->alignment = state.alignment;
		this->text_ = state.text_;
		this->rect_ = state.rect_;
		this->fontSize_ = state.fontSize_;
		this->params_ = state.params_;
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::TextRenderer& _target) 
	{
		_j["alignment"] = JsonConverter::Serialize<mtgb::TextAlignment>(_target.alignment);
		_j["text_"] = JsonConverter::Serialize<std::string>(_target.text_);
		_j["rect_"] = JsonConverter::Serialize<mtgb::RectF>(_target.rect_);
		_j["fontSize_"] = JsonConverter::Serialize<int>(_target.fontSize_);
		_j["params_"] = JsonConverter::Serialize<mtgb::UIParams>(_target.params_);
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::TextRenderer& _target) 
	{
		JsonConverter::Deserialize<mtgb::TextAlignment>(_target.alignment, _j,"alignment");
		JsonConverter::Deserialize<std::string>(_target.text_, _j,"text_");
		JsonConverter::Deserialize<mtgb::RectF>(_target.rect_, _j,"rect_");
		JsonConverter::Deserialize<int>(_target.fontSize_, _j,"fontSize_");
		JsonConverter::Deserialize<mtgb::UIParams>(_target.params_, _j,"params_");
		_target.OnPostRestore(); 
	} 
	/* ImGui表示処理の登録 */ 
	void mtgb::TextRenderer::RegisterImGui() 
	{ 
		static bool registered = false; 
		if (registered) return; 
		registered = true; 
	
		RegisterShowFuncHolder::Set<TextRenderer>([]( TextRenderer* _target, const char* _name)
			{
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->alignment, "alignment");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->text_, "text_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->rect_, "rect_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->fontSize_, "fontSize_");
				PropertyDisplayRegistry::Instance().ShowProperty(&_target->params_, "params_");
			});
		MTImGui::RegisterComponentViewer<TextRenderer>();
	}