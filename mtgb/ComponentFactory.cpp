#include "ComponentFactory.h"

bool mtgb::ComponentFactory::AddComponent(const std::type_index& _info, EntityId _id)
{
	std::type_index typeIdx{_info};
	auto itr = creators_.find(typeIdx);
	if (itr != creators_.end())
	{
		itr->second(_id);
		return true;
	}
	return false;
}

bool mtgb::ComponentFactory::AddComponentFromMemento(const IComponentMemento& _memento)
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
