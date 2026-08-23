#include "ImGuiShowable.h"
#include "Game.h"
#include "ImGuiRenderer.h"
#include "Editor/ImGuizmoManipulator.h"
#include "Editor/MTImGui.h"

namespace
{
	uint32_t defNameCount = 0;
}
mtgb::ImGuiShowable::ImGuiShowable(mtgb::EntityId _entityId)
	: targetEntityId_ { _entityId }
	, show_ { ShowType::NONE }
	, scope_ { Scope::SCENE }
{
	MTImGui::Register(this);
	displayName_ = "Default (" + std::to_string(defNameCount++) + ")";
}

void mtgb::ImGuiShowable::ShowImGui() {}

mtgb::ImGuiShowable::ImGuiShowable(ShowType _showType, mtgb::EntityId _entityId)
	: targetEntityId_ { _entityId }
	, show_ { _showType }
{
	scope_ = Scope::SCENE;
	MTImGui::Register(this);
}

mtgb::ImGuiShowable::ImGuiShowable(const std::string& _name, ShowType _showType, mtgb::EntityId _entityId, Scope _scope)
	: ImGuiShowable(_showType, _entityId)
{
	displayName_ = _name;
	scope_		 = _scope;
}

mtgb::ImGuiShowable::~ImGuiShowable()
{
	MTImGui::Unregister(this);
}
