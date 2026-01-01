#include "stdafx.h"
#include "IActor.h"
#include "ActorManager.h"
IActor::IActor(EntityId _entityId)
	: id_{_entityId}
{
	Game::System<ActorManager>().RegisterActor(this);
}

IActor::~IActor()
{
	Game::System<ActorManager>().UnregisterActor(id_);
}
