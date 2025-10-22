#include "RegisterComponents.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Collider.h"
#include "AudioPlayer.h"

void mtgb::RegisterComponents(ComponentFactory* _pComponentFactory)
{
	_pComponentFactory->RegisterComponent<Transform>();
	_pComponentFactory->RegisterComponent<RigidBody>();
	_pComponentFactory->RegisterComponent<Collider>();
	_pComponentFactory->RegisterComponent<AudioPlayer>();
}
