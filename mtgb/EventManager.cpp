#include "EventManager.h"
#include "Game.h"
#include "SceneSystem.h"
#include "ReleaseUtility.h"
mtgb::EventManager::~EventManager()
{
	for (auto& itr : events_)
	{
		SAFE_DELETE(itr.second);
	}
	events_.clear();
}
void mtgb::EventManager::Initialize()
{
	Game::System<SceneSystem>().OnMove([this]()
		{
			for (auto& event : events_)
			{
				event.second->UnsubscribeAll();
			}
		});
}