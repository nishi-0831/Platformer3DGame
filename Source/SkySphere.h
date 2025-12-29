#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SkySphere : public GameObject, public ImGuiShowable
	{
	public:
		SkySphere();
		~SkySphere();

		void Update() override;
		void Draw() const override;
		void ShowImGui() override;
		void Start() override;
	private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		float rotateAngleSec_;
	};
}