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
{
    // Œ^î•ñ‚É“o˜^‚³‚ê‚½–¼‘O‚ğæ“¾
    std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(PatrolChargerEnemy));
    name_ = std::format("{} ({})", typeName, generateCounter_++);
    displayName_ = name_;
    state_.OnUpdate(STATE::PATROL, [this]
        {
            this->Patrol();
        });

    state_.Change(STATE::PATROL);
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
    MTImGui::Instance().DrawCone(
        pTransform_->position,
        pTransform_->Forward(),
        foundFOV_,
        foundDistance_,
        1.0f, // ü‚Ì‘¾‚³
        16 // ‰~‚Ì•ªŠ„”
    );
}

void PatrolChargerEnemy::Patrol()
{
    pInterpolator_->UpdateProgress();
    pTransform_->position = pInterpolator_->Evaluate();
}

void PatrolChargerEnemy::Charge()
{
}

void PatrolChargerEnemy::Wait()
{
}

void PatrolChargerEnemy::ReturnToPatrol()
{
}

void PatrolChargerEnemy::Search()
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
        Quaternion targetRotation{ Quaternion::LookRotation(Vector3::Normalize(toTarget),pTransform_->Up()) };

        state_.Change(STATE::CHARGE);
    }
}
