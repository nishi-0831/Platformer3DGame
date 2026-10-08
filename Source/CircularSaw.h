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
		void ShowImGui() override;
		void Start() override;
		void StartOnEditMode() override;
		nlohmann::json SerializeProperties() const override;
		void DeserializeProperties(const nlohmann::json& _json) override;

	  private:
		void RotateInitialAngle();
		void CreateSaw();
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;

		// 自身からのこぎりまでの柱
		Transform* pPillarTransform_;
		MeshRenderer* pPillarMeshRenderer_;

		// のこぎり
		Saw* pSaw_;
		// 一秒あたりにのこぎりを回転させる角度
		DictionarySpinBox rotationSpeedSpinBox_;
		// 回転が負方向か否か
		bool reverse_;
		// プレイシーン開始時の初期回転角度
		SpinBox initialRotationAngleSpinBox_;
		// のこぎりとの距離
		SpinBox sawOffsetSpinBox_;
		static unsigned int generateCounter_;
	};

} // namespace mtgb