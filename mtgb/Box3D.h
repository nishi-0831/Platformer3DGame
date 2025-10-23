#pragma once
#include <mtgb.h>

namespace mtgb
{
	class Box3D : public GameObject , public ImGuiShowable
	{
	public:
		Box3D();
		~Box3D();

		void Update() override;
		void Draw() const override;
		void ShowImGui() override;
	private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;
		static unsigned int generateCounter_;
	};
}