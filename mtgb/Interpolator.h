#pragma once
#include "ReflectionMacro.h"
#include "ISerializableObject.h"
#include "IComponent.h"
#include "Transform.h"
#include "Collider.h"
#include "Vector3.h"
#include "InterpolatorCP.h"
#include "Interpolator.generated.h"

namespace mtgb
{
	class InterpolatorCP;
	MT_COMPONENT()
	class Interpolator : public IComponent<InterpolatorCP,Interpolator>, public ISerializableObject
	{
	public:
		MT_GENERATED_BODY()

		friend InterpolatorCP;
		using IComponent::IComponent;

		Interpolator(EntityId _entityId);
		void UpdateTransform();
		/// <summary>
		/// 補間の進行状況を更新する
		/// 経過時間を進め、端まで到達した場合は移動方向を反転する
		/// </summary>
		void UpdateProgress();
		/// <summary>
		/// 現在の進行状況で補間された値を返す
		/// </summary>
		/// <returns></returns>
		Vector3 Evaluate();
		/// <summary>
		/// 端点を設定する
		/// </summary>
		/// <param name="_start">始点</param>
		/// <param name="_end">終点</param>
		void SetEndpoints(const Vector3& _start, const Vector3& _end);

		void OnPostRestore() override;
		void OnPreSave() override;
	private:
		Transform* pTransform_;

		MT_PROPERTY()
		// 進行方向
		float dir_;

		MT_PROPERTY()
		// 経過時間
		float elapsed_;

		MT_PROPERTY()
		// 補間にかかる時間(秒)
		float duration_;


		MT_PROPERTY()
		// 始点の座標(シリアライズ用)
		Vector3 startPos_;

		MT_PROPERTY()
		// 終点の座標(シリアライズ用)
		Vector3 endPos_;
		
		// TODO:ポインタをシリアライズ
		// 直接、Transformなどのコンポーネントの状態を保存、復元できるようにすべき
		// 現状はstartPos_,endPos_のように値型の変数を別に用意している
 
		// 始点、終点のTransform
		Transform* pStartTransform_;
		Transform* pEndTransform_;
		// 始点、終点のコライダー
		Collider* pStartCollider_;
		Collider* pEndCollider_;
	};
}
