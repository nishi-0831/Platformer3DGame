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

	IComponentPool::RegisterComponentPoolType(typeid(Transform), typeid(TransformCP));
	IComponentPool::RegisterComponentPoolType(typeid(RigidBody), typeid(RigidBodyCP));
	IComponentPool::RegisterComponentPoolType(typeid(Collider), typeid(ColliderCP));
	IComponentPool::RegisterComponentPoolType(typeid(AudioPlayer), typeid(AudioPlayerCP));
	IComponentPool::RegisterComponentPoolType(typeid(MeshRenderer), typeid(MeshRendererCP));
}
