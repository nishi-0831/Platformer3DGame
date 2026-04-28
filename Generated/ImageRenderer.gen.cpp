// ImageRenderer.generated.h
#include "ImageRenderer.h"
#include "MTImGui.h"





	

// ============================================================================
// ImageRendererとImageRendererMementoの相互変換処理を実装
// ============================================================================


	ImageRendererMemento* mtgb::ImageRenderer::SaveToMemento()
	{ 
	OnPreSave(); 
		ImageRendererState state;
		return new Memento(GetEntityId(), state);
	} 
	
	void mtgb::ImageRenderer::RestoreFromMemento(const Memento& _memento) 
	{ 
		const ImageRendererState& state = _memento.GetState();
		OnPostRestore(); 
	} 
	
	void mtgb::to_json(nlohmann::json& _j,const mtgb::ImageRenderer& _target) 
	{
	} 
	void mtgb::from_json(const nlohmann::json& _j, mtgb::ImageRenderer& _target) 
	{
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
			});
		MTImGui::RegisterComponentViewer<ImageRenderer>();
	}