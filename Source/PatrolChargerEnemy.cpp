#include "stdafx.h"
#include "PatrolChargerEnemy.h"
#include "Debug.h"

PatrolChargerEnemy::PatrolChargerEnemy()
    : pTransform_{Component<Transform>( GetEntityId())}
    , pRigidBody_{Component<RigidBody>(GetEntityId())}
    , pMeshRenderer_{ Component<MeshRenderer>(GetEntityId()) }
    , pCollider_{Component<Collider>(GetEntityId())}
    , pTargetTransform_{nullptr}
    , foundFOV_{45.0f}
{
    
}

PatrolChargerEnemy::~PatrolChargerEnemy()
{
}

void PatrolChargerEnemy::Update()
{
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
}

void PatrolChargerEnemy::ShowImGui()
{
}

std::vector<IComponentMemento*> PatrolChargerEnemy::GetDefaultMementos(EntityId _entityId) const
{
    return std::vector<IComponentMemento*>();
}

void PatrolChargerEnemy::Patrol()
{
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
    
    // 正面方向とターゲットの内積を計算
    float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(pTransform_->Forward(), Vector3::Normalize(toTarget)));
    // 角度を算出(ラジアン)
    float angleRadian = std::acosf(std::clamp(dotProduct, -1.0f, 1.0f));
    // degreeに直す
    float angleDegree = DirectX::XMConvertToDegrees(angleRadian);

    // 発見したと判定する角度内か
    if (angleDegree <= foundFOV_)
    {
        Quaternion targetRotation{ Quaternion::LookRotation(Vector3::Normalize(toTarget),pTransform_->Up()) };

        state_.Change(STATE::CHARGE);
    }
}
