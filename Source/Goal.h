#pragma once
#include <mtgb.h>

class Goal : public mtgb::GameObject
{
  public:
	Goal();
	~Goal();

	void Update() override;
	void Start() override;
	void Draw() const override;

  private:
	void OnClear();
	Transform* pTransform_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	static unsigned int generateCounter_;
	// ゴールの3Dモデルと一緒に描画するエフェクト
	std::weak_ptr<EffectParameters> pEffect_;
	// ゴールに触れてからリザルトシーンに遷移するまでの猶予
	float transitionSceneDelay_;
};