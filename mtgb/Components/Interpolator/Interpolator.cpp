#include "stdafx.h"
#include "Interpolator.h"
#include "Core/Time/GameTime.h"
#include "Math/Mathf.h"
#include "Core/GameObject/GameObject.h"
#include "Core/SceneSystem.h"

namespace
{
	// 始点、終点の初期設定

	// 座標のオフセット
	mtgb::Vector3 INIT_OFFSET { 1.0f, 0.0f, 0.0f };
	// コライダー(球)の半径
	float INIT_RADIUS { 1.0f };
} // namespace

mtgb::Interpolator::Interpolator(EntityId _entityId)
	: IComponent(_entityId)
	, pTransform_ { &Transform::Get(_entityId) }
	, dir_ { 1.0f }
	, elapsed_ { 0.0f }
	, duration_ { 1.0f }
	, startPos_ { Vector3::Zero() }
	, endPos_ { Vector3::Zero() }
	, pStartTransform_ { nullptr }
	, pEndTransform_ { nullptr }
	, pStartCollider_ { nullptr }
	, pEndCollider_ { nullptr }
{
	// 始点、終点のゲームオブジェクト作成
	GameObject* start = new GameObject();
	GameObject* end	  = new GameObject();

	// ゲームオブジェクトをシーンに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(start);
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(end);

	// Transform作成
	pStartTransform_ = start->Component<Transform>();
	pEndTransform_	 = end->Component<Transform>();
	// オフセット分動かす

	pStartTransform_->position = pTransform_->position - Vector3 { pTransform_->scale * INIT_OFFSET };
	pEndTransform_->position   = pTransform_->position + Vector3 { pTransform_->scale * INIT_OFFSET };

	// コライダー作成
	pStartCollider_ = start->Component<Collider>();
	pEndCollider_	= end->Component<Collider>();

	// コライダーを球に設定
	pStartCollider_->colliderType_ = ColliderType::TYPE_SPHERE;
	pStartCollider_->isTrigger_	   = true;
	pEndCollider_->colliderType_   = ColliderType::TYPE_SPHERE;
	pStartCollider_->SetRadius(INIT_RADIUS);
	pEndCollider_->isTrigger_ = true;
	pEndCollider_->SetRadius(INIT_RADIUS);
}

Interpolator& mtgb::Interpolator::operator=(const Interpolator& _other)
{
	if (this == &_other)
	{
		return *this;
	}
	IComponent::operator=(_other);
	this->dir_		  = _other.dir_;
	this->elapsed_	  = _other.elapsed_;
	this->duration_	  = _other.duration_;
	this->startPos_	  = _other.startPos_;
	this->endPos_	  = _other.endPos_;
	this->pTransform_ = _other.pTransform_;
	return *this;
}

mtgb::Interpolator::~Interpolator() {}

void mtgb::Interpolator::UpdateTransform()
{
	// 進行度を更新
	UpdateProgress();

	// 補間値に座標を更新
	pTransform_->position = EvaluatePos();
}

void mtgb::Interpolator::UpdateProgress()
{
	elapsed_ += Time::DeltaTimeF() * dir_;
	float progress = elapsed_ / duration_;
	// 進行度が0から1を超えた場合、進行方向を反転させる
	if (progress > 1.0f || progress < 0.0f)
	{
		dir_ *= -1.0f;
		elapsed_ = std::clamp(elapsed_, 0.0f, duration_);
	}
}

mtgb::Vector3 mtgb::Interpolator::EvaluatePos()
{
	// 始点から終点を進行度で補間
	float progres = elapsed_ / duration_;
	return Mathf::Lerp(pStartTransform_->position, pEndTransform_->position, progres);
}

Quaternion mtgb::Interpolator::CalculateRot()
{
	Vector3 to, from;

	if (dir_ < 0.0f)
	{
		// 終点から始点へ
		to	 = pStartTransform_->position;
		from = pEndTransform_->position;
	}
	else
	{
		// 始点から終点へ
		to	 = pEndTransform_->position;
		from = pStartTransform_->position;
	}

	// ベクトルから四元数作成
	Vector3 rot = Vector3::Normalize(to - from);
	return Quaternion::LookRotation(rot, Vector3::Up());
}

void mtgb::Interpolator::SetEndpoints(const Vector3& _start, const Vector3& _end)
{
	pStartTransform_->position = _start;
	pEndTransform_->position   = _end;
}

void mtgb::Interpolator::OnPostRestore()
{
	// 読み込んだ値を始点、終点の座標に代入
	pStartTransform_->position = startPos_;
	pEndTransform_->position   = endPos_;
}

void mtgb::Interpolator::OnPreSave()
{
	// シリアライズ用の変数に始点、終点の座標を代入
	startPos_ = pStartTransform_->position;
	endPos_	  = pEndTransform_->position;
}

void mtgb::Interpolator::OnChangeEntityId()
{
	pTransform_ = &Transform::Get(GetEntityId());
}

void mtgb::Interpolator::Reset()
{
	Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(pStartCollider_->GetEntityId());
	Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(pEndCollider_->GetEntityId());
}
