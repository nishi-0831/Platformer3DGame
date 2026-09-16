#pragma once
#include "Core/Entity.h"
#include "Core/Component/IComponentMemento.h"
#include "Core/GameObject/GameObject.h"

namespace mtgb
{
	/// <summary>
	/// ゲームオブジェクトのスナップショット。Undo/Redoに使う
	/// </summary>
	struct GameObjectSnapshot
	{
		EntityId entityId;
		std::string name;
		std::string typeName;
		std::vector<IComponentMemento*> mementos;
	};
} // namespace mtgb