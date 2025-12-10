#include "ComponentFactory.h"


mtgb::IComponentMemento* mtgb::ComponentFactory::AddComponent(const std::type_index& _info, EntityId _id) const
{
	std::type_index typeIdx{_info};
	auto itr = creators_.find(typeIdx);
	if (itr != creators_.end())
	{
		IComponentMemento* memento = itr->second(_id);
		if (memento)
			return memento;
	}
	return nullptr;
}

bool mtgb::ComponentFactory::AddComponentFromMemento(const IComponentMemento& _memento) const
{
	std::type_index typeIdx{_memento.GetComponentType()};
	auto itr = creatorsFromMemento_.find(typeIdx);
	if (itr != creatorsFromMemento_.end())
	{
		itr->second(_memento);
		return true;
	}
	return false;
}

