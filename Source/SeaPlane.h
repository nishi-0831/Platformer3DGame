#pragma once
#include <mtgb.h>

class SeaPlane : public GameObject
{
  public:
	SeaPlane();
	~SeaPlane();

  private:
	Transform* pTransform_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	static unsigned int generateCounter_;
};