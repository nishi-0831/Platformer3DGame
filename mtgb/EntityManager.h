
#pragma once
#include "cmtgb.h"
#include "ISystem.h"
#include "Entity.h"
namespace mtgb
{
	class EntityManager : public ISystem
	{
	public:
		void Initialize() override;
		void Update() override;
		EntityId CreateEntity();
		EntityId GetCounter();
		void ChangeCounter(EntityId _counter);
		EntityId RegenerateEntity();
		void DecrementCounter();
	private:
		EntityId entityCounter_;
	};
}