#pragma once
#include <mtgb.h>
#include "IActor.h"
#include "Saw.h"
#include "Editor/SpinBox.h"

namespace mtgb
{
	/// <summary>
	/// 自身を中心にのこぎりを回転させるゲームオブジェクト
	/// </summary>
	class CircularSaw : public GameObject
	{
	  public:
		CircularSaw();
		~CircularSaw();

		void Update() override;
		void Draw() const override;
		void ShowImGui() override;
		void Start() override;
		void StartOnEditMode() override;

	  private:
		void CreateSaw();
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;

		// 自身からのこぎりまでの柱

		Transform* pPillarTransform_;
		MeshRenderer* pPillarMeshRenderer_;

		// のこぎり
		Saw* pSaw_;
		// のこぎりとの距離
		float sawOffset_;
		// 一秒あたりにのこぎりを回転させる角度
		float rotateAngleSec_;
		static unsigned int generateCounter_;
	};

} // namespace mtgb