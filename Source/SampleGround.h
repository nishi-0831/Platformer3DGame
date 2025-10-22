#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SampleGround : public GameObject , public ImGuiShowable
	{
	public:
		SampleGround();
		~SampleGround();

		void Update() override;
		void Draw() const override;
		void ShowImGui() override;
	private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;
	};
}