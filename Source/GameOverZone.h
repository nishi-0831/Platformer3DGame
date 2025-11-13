#pragma once
#include <mtgb.h>

class GameOverZone : public mtgb::GameObject
{
public:
	GameOverZone();
	~GameOverZone();

	void Update() override;
	void Start() override;
	void Draw() const override;

	std::vector<IComponentMemento*> GetDefaultMementos(EntityId _entityId) const override;
private:
	Transform* pTransform_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	static unsigned int generateCounter_;
};