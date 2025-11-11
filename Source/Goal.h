#pragma once
#include <mtgb.h>

class Goal : public mtgb::GameObject , public ImGuiShowable
{
public:
	Goal();
	~Goal();

	void Update() override;
	void Start() override;
	void Draw() const override;
	void ShowImGui() override;

	std::vector<IComponentMemento*> GetDefaultMementos(EntityId _entityId) const override;
private:
	Transform* pTransform_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	static unsigned int generateCounter_;

};