#include "ComponentFactory.h"

bool mtgb::ComponentFactory::CreateComponent(const std::type_index& _info, EntityId _id)
{
	std::type_index typeIdx(_info);
	auto itr = creators_.find(typeIdx);
	if (itr != creators_.end())
	{
		itr->second(_id);
		return true;
	}
	return false;
}
