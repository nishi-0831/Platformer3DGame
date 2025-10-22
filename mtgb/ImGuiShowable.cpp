#include "ImGuiShowable.h"
#include "Game.h"
#include "ImGuiRenderer.h"
#include "ImGuizmoManipulator.h"
#include "MTImGui.h"

namespace
{
    uint32_t defNameCount = 0;
}
mtgb::ImGuiShowable::ImGuiShowable(mtgb::EntityId _entityId)
    : entityId_{_entityId}
    , show_{ShowType::Inspector}
{
    MTImGui::Instance().Register(this);
    displayName_ = "Default (" + std::to_string(defNameCount++) + ")";
    /*if (defNameCount < UINT32_MAX) {
        displayName_ = "Default (" + std::to_string(defNameCount++) + ")";
    }
    else {
        displayName_ = "Default (Max)";
    }*/
}

void mtgb::ImGuiShowable::ShowImGui()
{

}

mtgb::ImGuiShowable::ImGuiShowable(ShowType _showType, mtgb::EntityId _entityId)
    : entityId_{ _entityId }
    , show_{_showType}
{
    MTImGui::Instance().Register(this);
}

mtgb::ImGuiShowable::ImGuiShowable(const std::string& _name, ShowType _showType, mtgb::EntityId _entityId)
    :ImGuiShowable(_showType,_entityId)
{
    displayName_ = _name;
}


mtgb::ImGuiShowable::~ImGuiShowable()
{
    MTImGui::Instance().Unregister(this);
}

