#include "RegisterComponents.h"
#include "IComponentPool.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "AudioPlayer.h"

void mtgb::RegisterComponents(ComponentFactory* _pComponentFactory)
{
	_pComponentFactory->RegisterComponent<Transform,TransformMemento>();
	_pComponentFactory->RegisterComponent<RigidBody,RigidBodyMemento>();
	_pComponentFactory->RegisterComponent<Collider,ColliderMemento>();
	_pComponentFactory->RegisterComponent<AudioPlayer,AudioPlayerMemento>();
	_pComponentFactory->RegisterComponent<MeshRenderer,MeshRendererMemento>();
}
