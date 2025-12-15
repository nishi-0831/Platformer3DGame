#include "stdafx.h"
#include "PatrolChargerEnemy.h"
#include "Debug.h"
#include <format>

unsigned int PatrolChargerEnemy::generateCounter_{0};

PatrolChargerEnemy::PatrolChargerEnemy()
	: pTransform_{Component<Transform>()}
	, pRigidBody_{Component<RigidBody>()}
	, pMeshRenderer_{ Component<MeshRenderer>() }
	, pCollider_{Component<Collider>()}
	, pTargetTransform_{nullptr}
	, pInterpolator_{Component<Interpolator>()}
	, foundFOV_{45.0f}
	, foundDistance_{5.0f}
	, waitTime_{ 3.0f }
	, chargeSpeed_{ 2.0f }
	, targetEntityId_{ INVALID_ENTITY }
	, returnToPatrolSpeed_{1.0f}
	, distPos_{Vector3::Zero()}
{
	pMeshRenderer_->meshFileName = "Model/GolemAnim2.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::FbxPartsSkin;
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->SetExtents({ 1.0f ,1.0f ,1.0f });
	// Œ^î•ñ‚É“o˜^‚³‚ê‚½–¼‘O‚ğæ“¾
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(PatrolChargerEnemy));
	name_ = std::format("{} ({})", typeName, generateCounter_++);
	displayName_ = name_;
	state_.OnUpdate(STATE::PATROL, [this]
		{
			Patrol();
		});
	state_.OnUpdate(STATE::CHARGE, [this]
		{
			Charge();
		});
	state_.OnUpdate(STATE::WAIT, [this]
		{
			Wait();
		});
	state_.OnUpdate(STATE::RETURN_TO_PATROL, [this]
		{
			ReturnToPatrol();
		});
	state_.Change(STATE::PATROL);

	pRigidBody_->OnCollisionEnter([this](EntityId _entityId)
		{
			if (_entityId == targetEntityId_)
			{
				this->OnChargePlayer();
			}
		});
}

PatrolChargerEnemy::~PatrolChargerEnemy()
{
}

void PatrolChargerEnemy::Update()
{
	state_.Update();
}

void PatrolChargerEnemy::Draw() const
{
}

void PatrolChargerEnemy::Start()
{
	// Tag‚ªPlayer‚ÌGameObject‚ğ’T‚·
	GameObject* targetGameObj = FindGameObject(GameObjectTag::Player);
	if (targetGameObj == nullptr)
	{
		LOGIMGUI("Not Found Target");
		return;
	}

	pTargetTransform_ = &Transform::Get(targetGameObj->GetEntityId());
	targetEntityId_ = pTargetTransform_->GetEntityId();
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

	// ‹–ì‚ğ‰~‚Æ‚µ‚Ä•`‰æ
	//MTImGui::Instance().DrawCone(
	//    pTransform_->position,
	//    pTransform_->Forward(),
	//    foundFOV_,
	//    foundDistance_,
	//    1.0f, // ü‚Ì‘¾‚³
	//    16 // ‰~‚Ì•ªŠ„”
	//);
	MTImGui::Instance().ShowComponents(Entity::entityId_);

}

void PatrolChargerEnemy::Patrol()
{
	pInterpolator_->UpdateProgress();
	pTransform_->position = pInterpolator_->Evaluate();

	Vector3 toTarget = pTargetTransform_->GetWorldPosition() - pTransform_->GetWorldPosition();
	if (toTarget.Size() <= foundDistance_)
	{
		state_.Change(STATE::WAIT);
		waitTime_ = 2.0f;
		nextState_ = STATE::CHARGE;
	}
}

void PatrolChargerEnemy::Charge()
{



	distPos_ = { pTargetTransform_->position.x ,pTransform_->position.y,pTargetTransform_->position.z };
	Vector3 toTarget = distPos_ - pTransform_->GetWorldPosition();
	pTransform_->position += Vector3::Normalize(toTarget) * chargeSpeed_ * Time::DeltaTimeF();

	float detectionDistance = 0.1f;
	if (toTarget.Size() <= detectionDistance)
	{
		state_.Change(STATE::WAIT);
		waitTime_ = 1.0f;
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
	// ˆÈ‘O‘{õ‚µ‚Ä‚¢‚½Û‚ÌÀ•W
	Vector3 returnPos = pInterpolator_->Evaluate();
	Vector3 vToReturnPos = returnPos - pTransform_->GetWorldPosition();
	pTransform_->position += Vector3::Normalize(vToReturnPos) * returnToPatrolSpeed_ * Time::DeltaTimeF();

	float returnDistance = 0.1f;
	if (vToReturnPos.Size() <= returnDistance)
	{
		state_.Change(STATE::PATROL);
	}
}

bool PatrolChargerEnemy::Search()
{
	Vector3 toTarget =  pTargetTransform_->GetWorldPosition() - pTransform_->GetWorldPosition();
	
	// ³–Ê•ûŒü‚Æƒ^[ƒQƒbƒg‚Ì“àÏ‚ğŒvZ
	float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(pTransform_->Forward(), Vector3::Normalize(toTarget)));
	// Šp“x‚ğZo(ƒ‰ƒWƒAƒ“)
	float angleRadian = std::acosf(std::clamp(dotProduct, -1.0f, 1.0f));
	// degree‚É’¼‚·
	float angleDegree = DirectX::XMConvertToDegrees(angleRadian);

	// ”­Œ©‚µ‚½‚Æ”»’è‚·‚éŠp“xA‹——£‚©‚Ç‚¤‚©
	if (angleDegree <= foundFOV_ && toTarget.Size() <= foundDistance_ )
	{
		// ”­Œ©
		//Quaternion targetRotation{ Quaternion::LookRotation(Vector3::Normalize(toTarget),pTransform_->Up()) };
		//state_.Change(STATE::CHARGE);

		return true;
	}
	return false;
}

void PatrolChargerEnemy::OnChargePlayer()
{
	LOGIMGUI("collision enter player : ChargeEnemy");
	state_.Change(STATE::WAIT);
	waitTime_ = 2.0f;
	nextState_ = STATE::RETURN_TO_PATROL;
}
