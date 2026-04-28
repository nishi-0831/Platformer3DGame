// TextRenderer.generated.h
#include "TextRenderer.h"
#include "MTImGui.h"





	

// ============================================================================
// TextRendererとTextRendererMementoの相互変換処理を実装
// ============================================================================


	TextRendererMemento* mtgb::TextRenderer::SaveToMemento()
	{ 
	OnPreSave(); 
		TextRendererState state;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::TextRenderer::RestoreFromMemento(const Memento& _memento) 
	{ 
		const TextRendererState& state = _memento.GetState();
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::TextRenderer& _target) 
	{
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::TextRenderer& _target) 
	{
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
			});
		MTImGui::RegisterComponentViewer<TextRenderer>();
	}