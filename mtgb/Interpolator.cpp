#include "stdafx.h"
#include "Interpolator.h"
#include "GameTime.h"
#include "Mathf.h"
#include "GameObject.h"
#include "SceneSystem.h"

namespace
{
	// 始点、終点の初期設定

	// 座標のオフセット
	mtgb::Vector3 INIT_OFFSET{ 1.0f,0.0f,0.0f };
	// コライダー(球)の半径
	float INIT_RADIUS{ 1.0f };
}

mtgb::Interpolator::Interpolator(EntityId _entityId)
	: pTransform_{ &Transform::Get(_entityId) }
	, dir_{ 1.0f }
	, elapsed_{ 0.0f }
	, duration_{ 1.0f }
	, startPos_{ Vector3::Zero()}
	, endPos_{ Vector3::Zero() }
	, pStartTransform_{ nullptr }
	, pEndTransform_{ nullptr }
	, pStartCollider_{ nullptr }
	, pEndCollider_{ nullptr }
{
	// 始点、終点のゲームオブジェクト作成
	GameObject* start = new GameObject();
	GameObject* end = new GameObject();

	// ゲームオブジェクトをシーンに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(start);
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(end);

	// Transform作成
	pStartTransform_ = start->Component<Transform>();
	pEndTransform_ = end->Component<Transform>();
	// オフセット分動かす
	pStartTransform_->position = pTransform_->position - INIT_OFFSET;
	pEndTransform_->position = pTransform_->position + INIT_OFFSET;

	// コライダー作成
	pStartCollider_ = start->Component<Collider>();
	pEndCollider_ = end->Component<Collider>();

	// コライダーを球に設定
	pStartCollider_->colliderType_ = ColliderType::TYPE_SPHERE;
	pEndCollider_->colliderType_ = ColliderType::TYPE_SPHERE;
	pStartCollider_->SetRadius(INIT_RADIUS);
	pEndCollider_->SetRadius(INIT_RADIUS);
}

void mtgb::Interpolator::UpdateTransform()
{
	// 進行度を更新
	UpdateProgress();

	// 補間値に座標を更新
	pTransform_->position = Evaluate();
}

void mtgb::Interpolator::UpdateProgress()
{
	elapsed_ += Time::DeltaTimeF() * dir_;
	float progress = elapsed_ / duration_;
	// 進行度が0から1を超えた場合、進行方向を反転させる
	if (progress > 1.0f || progress < 0.0f)
	{
		dir_ *= -1.0f;
		elapsed_ = std::clamp(elapsed_, 0.0f, 1.0f);
	}
}

mtgb::Vector3 mtgb::Interpolator::Evaluate()
{
	// 始点から終点を進行度で補間
	float progres = elapsed_ / duration_;
	return Mathf::Lerp(pStartTransform_->position, pEndTransform_->position, progres);
}

void mtgb::Interpolator::SetEndpoints(const Vector3& _start, const Vector3& _end)
{
	pStartTransform_->position = _start;
	pEndTransform_->position = _end;
}

void mtgb::Interpolator::OnPostRestore()
{
	// 読み込んだ値を始点、終点の座標に代入
	pStartTransform_->position = startPos_;	
	pEndTransform_->position = endPos_;
}

void mtgb::Interpolator::OnPreSave()
{
	// シリアライズ用の変数に始点、終点の座標を代入
	startPos_ = pStartTransform_->position;
	endPos_ = pEndTransform_->position;
}
