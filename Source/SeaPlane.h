#pragma once
#include <mtgb.h>
#include <PlaneUVScroll.h>

class SeaPlane : public GameObject
{
  public:
	SeaPlane();
	~SeaPlane();

	void Update() override;
	void Draw() const override;
	void Start() override;

  private:
	Transform* pTransform_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	static unsigned int generateCounter_;
};