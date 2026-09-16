#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SkySphere : public GameObject
	{
	  public:
		SkySphere();
		~SkySphere();

		void Update() override;
		void Draw() const override;
		void Start() override;

	  private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		/// <summary>
		/// 一秒あたりの回転角度
		/// </summary>
		float rotateAngleSec_;
		/// <summary>
		/// スカイスフィアの大きさ
		/// </summary>
		float sphereScale_;
	};
} // namespace mtgb