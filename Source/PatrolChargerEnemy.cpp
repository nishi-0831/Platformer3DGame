#include "stdafx.h"
#include "PatrolChargerEnemy.h"
#include "Debug.h"
#include <format>

unsigned int PatrolChargerEnemy::generateCounter_{0};

PatrolChargerEnemy::PatrolChargerEnemy()
	: pTransform_{Component<Transform>()}
	, IActor(GetEntityId())
	, pRigidBody_{Component<RigidBody>()}
	, pMeshRenderer_{Component<MeshRenderer>()}
	, pCollider_{Component<Collider>()}
	, pTargetTransform_{nullptr}
	, pInterpolator_{Component<Interpolator>()}
	, foundFOV_{45.0f}
	, foundDistance_{6.0f}
	, waitTimeTransitionCharge_{1.0f}
	, waitTime_{3.0f}
	, chargeSpeed_{4.0f}
	, chargeTime_{5.0f}
	, takeDamageNum_{1}
	, walkAnimSpeed_{0.5f}
	, waitTimeAfterCharge_{2.0f}
	, targetEntityId_{INVALID_ENTITY}
	, returnToPatrolSpeed_{2.0f}
	, onStompedBounce_{5.0f}
{
	tag_						 = GameObjectTag::Enemy;
	pMeshRenderer_->meshFileName = "Model/GolemAnim.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer		 = AllLayer();
	pMeshRenderer_->shaderType	 = ShaderType::FbxPartsSkin;

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->SetExtents({1.0f, 1.0f, 1.0f});
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(PatrolChargerEnemy));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_		 = name_;

	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			OnCollisionEnter(_entityId);
		}
	);
	pRigidBody_->isKinematic_ = false;

	InitializeState();
}

PatrolChargerEnemy::~PatrolChargerEnemy()
{
}

void PatrolChargerEnemy::Update()
{
	if (animController_.has_value())
	{
		animController_->UpdateFrame();
		pMeshRenderer_->SetFrame(animController_->GetCurrentFrame());
	}
	state_.Update();
}

void PatrolChargerEnemy::Draw() const
{
}

void PatrolChargerEnemy::Start()
{
	// TagがPlayerのGameObjectを探す
	GameObject* targetGameObj = FindGameObject(GameObjectTag::Player);
	if (targetGameObj == nullptr)
	{
		LOGIMGUI("Not Found Target");
		return;
	}

	pTargetTransform_ = &Transform::Get(targetGameObj->GetEntityId());
	targetEntityId_	  = pTargetTransform_->GetEntityId();
}

void PatrolChargerEnemy::ShowImGui()
{
	if (state_.Current() == STATE::PATROL)
	{
		ImGui::Text("STATE::PATROL");
	}
	else if (state_.Current() == STATE::CHARGE)
	{
		ImGui::Text("STATE::CHARGE");
	}
	else if (state_.Current() == STATE::WAIT)
	{
		ImGui::Text("STATE::WAIT");
	}
	else if (state_.Current() == STATE::RETURN_TO_PATROL)
	{
		ImGui::Text("STATE::RETURN_TO_PATROL");
	}
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

void PatrolChargerEnemy::OnStomped(IActor* _pOther)
{
	if (state_.Current() != STATE::DYING)
	{
		// 踏まれたエフェクト再生
		EffectParameters params;
		params.isLoop	  = false;
		Vector3 effectPos = Vector3{
			pTransform_->position.x,
			pTransform_->position.y + pCollider_->GetExtents().y,
			pTransform_->position.z
		};
		Matrix4x4 mat	= DirectX::XMMatrixTranslation(effectPos.x, effectPos.y, effectPos.z);
		params.worldMat = mat;
		Game::System<EffectManager>().Play("Stomp", params);

		// 踏まれたSE再生
		Audio::PlayOneShotFile("Sound/Stomp.mp3");

		// 踏んだアクターを上に飛ばす
		EntityId id				  = _pOther->GetId();
		RigidBody& otherRigidBody = RigidBody::Get(id);
		otherRigidBody.velocity_.y += onStompedBounce_;
	}

	state_.Change(STATE::DYING);
}

void PatrolChargerEnemy::OnHitSide(IActor* _pOther)
{
	if (state_.Current() == STATE::CHARGE)
	{
		LOGIMGUI("collision enter player : ChargeEnemy");
		_pOther->TakeDamage(takeDamageNum_);
		state_.Change(STATE::WAIT);
		waitTime_  = waitTimeAfterCharge_;
		nextState_ = STATE::RETURN_TO_PATROL;
	}
}

void PatrolChargerEnemy::TakeDamage(int _damage)
{
	state_.Change(STATE::DYING);
}

void PatrolChargerEnemy::InitializeState()
{
	// アニメーションのコントローラを取得
	animController_ = Fbx::GetAnimationController(pMeshRenderer_->meshHandle);

	state_
		.OnStart(
			STATE::PATROL,
			[this]
			{
				animController_->PlayAnimation("Walk", true);
				animController_->SetAnimationSpeed(walkAnimSpeed_);
			}
		)
		.OnUpdate(
			STATE::PATROL,
			[this]
			{
				Patrol();
			}
		)
		.OnEnd(
			STATE::PATROL,
			[this]
			{
				animController_->SetAnimationSpeed(1.0f);
			}
		);

	state_
		.OnStart(
			STATE::CHARGE,
			[this]
			{
				animController_->PlayAnimation("Run", true);
			}
		)
		.OnUpdate(
			STATE::CHARGE,
			[this]
			{
				Charge();
			}
		);

	state_
		.OnStart(
			STATE::WAIT,
			[this]
			{
				animController_->PlayAnimation("Idle", true);
			}
		)
		.OnUpdate(
			STATE::WAIT,
			[this]
			{
				Wait();
			}
		);

	state_
		.OnStart(
			STATE::RETURN_TO_PATROL,
			[this]
			{
				animController_->PlayAnimation("Walk", true);
			}
		)
		.OnUpdate(
			STATE::RETURN_TO_PATROL,
			[this]
			{
				ReturnToPatrol();
			}
		);

	state_
		.OnStart(
			STATE::DYING,
			[this]
			{
				animController_->PlayAnimation("Dying", false);
			}
		)
		.OnUpdate(
			STATE::DYING,
			[this]
			{
				Dying();
			}
		);

	state_.Change(STATE::PATROL);
}

void PatrolChargerEnemy::Patrol()
{
	pInterpolator_->UpdateProgress();
	pTransform_->position = pInterpolator_->EvaluatePos();
	pTransform_->rotate	  = pInterpolator_->CalculateRot();

	Vector3 toTarget = pTargetTransform_->GetWorldPosition() - pTransform_->GetWorldPosition();
	if (toTarget.Size() <= foundDistance_)
	{
		// 一定時間待機してから突進状態に遷移
		state_.Change(STATE::WAIT);
		waitTime_  = waitTimeTransitionCharge_;
		nextState_ = STATE::CHARGE;

		// ターゲットの方向を向く
		pTransform_->rotate = Quaternion::LookRotation(Vector3::Normalize(toTarget), Vector3::Up());
	}
}

void PatrolChargerEnemy::Charge()
{
	// 目標地点
	// -------------------------------------------------------
	// 注意
	// x,z軸のみターゲットの座標を使っている。y軸は自身の座標のまま。
	// ターゲットとのy座標が異なると空中歩行してしまうから。
	// 斜面を移動させる場合は修正が必要
	// ------------------------------------------------------
	// Vector3 distPos = { pTargetTransform_->position };
	Vector3 distPos		= {pTargetTransform_->position.x, pTransform_->position.y, pTargetTransform_->position.z};
	Vector3 toTarget	= distPos - pTransform_->GetWorldPosition();
	Vector3 toTargetDir = Vector3::Normalize(toTarget);

	// ターゲットの方を向いて、突進!!!
	pTransform_->position += toTargetDir * chargeSpeed_ * Time::DeltaTimeF();
	pTransform_->rotate = Quaternion::LookRotation(toTarget, Vector3::Up());

	float detectionDistance = 0.1f;
	if (toTarget.Size() <= detectionDistance)
	{
		// 一定時間待機してから巡回地点に戻る
		state_.Change(STATE::WAIT);
		waitTime_  = waitTimeAfterCharge_;
		nextState_ = STATE::RETURN_TO_PATROL;
	}
}

void PatrolChargerEnemy::Wait()
{
	waitTime_ -= Time::DeltaTimeF();
	if (waitTime_ < 0.0f)
	{
		state_.Change(nextState_);
	}
}

void PatrolChargerEnemy::ReturnToPatrol()
{
	// 以前捜索していた際の座標
	Vector3 returnPos	 = pInterpolator_->EvaluatePos();
	Vector3 vToReturnPos = returnPos - pTransform_->GetWorldPosition();
	pTransform_->position += Vector3::Normalize(vToReturnPos) * returnToPatrolSpeed_ * Time::DeltaTimeF();
	pTransform_->rotate = Quaternion::LookRotation(Vector3::Normalize(vToReturnPos), Vector3::Up());

	float returnDistance = 0.1f;
	if (vToReturnPos.Size() <= returnDistance)
	{
		state_.Change(STATE::PATROL);
	}
}

void PatrolChargerEnemy::Dying()
{
	if (animController_->IsFinishedAnimation())
	{
		DestroyMe();
	}
}

bool PatrolChargerEnemy::Search()
{
	Vector3 toTarget = pTargetTransform_->GetWorldPosition() - pTransform_->GetWorldPosition();

	// 正面方向とターゲットの内積を計算
	float dotProduct =
		DirectX::XMVectorGetX(DirectX::XMVector3Dot(pTransform_->Forward(), Vector3::Normalize(toTarget)));
	// 角度を算出(ラジアン)
	float angleRadian = std::acosf(std::clamp(dotProduct, -1.0f, 1.0f));
	// degreeに直す
	float angleDegree = DirectX::XMConvertToDegrees(angleRadian);

	// 発見したと判定する角度、距離かどうか
	if (angleDegree <= foundFOV_ && toTarget.Size() <= foundDistance_)
	{
		// 発見
		// Quaternion targetRotation{ Quaternion::LookRotation(Vector3::Normalize(toTarget),pTransform_->Up()) };
		// state_.Change(STATE::CHARGE);
		return true;
	}
	return false;
}

void PatrolChargerEnemy::OnCollisionEnter(EntityId _entityId)
{
	/*if (_entityId != targetEntityId_)
		return;

	LOGIMGUI("collision enter player : ChargeEnemy");
	GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	Transform& otherTransform = Transform::Get(_entityId);

	if ( pTransform_->position.y <= otherTransform.position.y)
	{

		return;
	}*/
}
