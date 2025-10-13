#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SampleGround : public GameObject
	{
	public:
		SampleGround();
		~SampleGround();

		void Update() override;
		void Draw() const override;
	private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
	};
}