#include "RegisterComponents.h"
#include "Transform.h"
#include "TransformMemento.h"
#include "RigidBody.h"
#include "RigidBodyMemento.h"
#include "Collider.h"
#include "ColliderMemento.h"
#include "AudioPlayer.h"
#include "AudioPlayerMemento.h"

void mtgb::RegisterComponents(ComponentFactory* _pComponentFactory)
{
	_pComponentFactory->RegisterComponent<Transform,TransformMemento>();
	_pComponentFactory->RegisterComponent<RigidBody,RigidBodyMemento>();
	_pComponentFactory->RegisterComponent<Collider,ColliderMemento>();
	_pComponentFactory->RegisterComponent<AudioPlayer,AudioPlayerMemento>();
}
