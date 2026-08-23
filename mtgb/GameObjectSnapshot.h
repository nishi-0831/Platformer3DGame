#pragma once
#include "Entity.h"
#include "IComponentMemento.h"
#include "GameObject.h"

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