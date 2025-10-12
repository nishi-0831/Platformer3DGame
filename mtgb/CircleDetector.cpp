#include "CircleDetector.h"
#include "CameraSystem.h"
#include "Game.h"
#include "GameObject.h"
#include <cmath>
#include <string>
#include "Screen.h"
#include "Draw.h"
#include "Image.h"
namespace mtgb
{
    // �^�[�Q�b�g�Ƃ��Ė�������G�̖��O
    const std::string ignoreName{ "EnemyBroken" };

    CircleDetector::CircleDetector()
    {
    }
    CircleDetector::CircleDetector(const CircleDetectorConfig& _config)
        : CircleDetector{}
    {
        config = _config;
    }

    CircleDetector::CircleDetector(CircleDetectorConfig&& _config)
        : CircleDetector{}
    {
        config = std::move(_config);
    }

    void CircleDetector::UpdateDetection()
    {
        UpdateDetection(config);
    }

    void CircleDetector::UpdateDetection(const CircleDetectorConfig& _config)
    {
        detectedTargets_.clear();

        // �^�O�Ŏ擾
        std::vector<GameObject*> findObjs;
        GameObject::FindGameObjects(_config.base.targetTag, &findObjs);

        for (const auto& obj : findObjs)
        {
            if (obj->GetName() == ignoreName)
            {
                continue;  // ���O�������ΏۂȂ��A
            }
            
            // Transform�擾
            Transform* pTransform = &Transform::Get(obj->GetEntityId());
            Vector3 worldPos = pTransform->GetWorldPosition();

            // �����`�F�b�N
            // �J�����ʒu���擾
            Vector3 cameraPos = Game::System<CameraSystem>().GetTransform(_config.base.windowContext).GetWorldPosition();
            float distance = (worldPos - cameraPos).Size();
            
            if (distance < _config.base.minDistance || distance > _config.base.maxDistance)
            {
                continue;
            }

            // ���[���h���W���X�N���[�����W�ɕϊ�
            Vector3 screenPos = Game::System<CameraSystem>().GetWorldToScreenPos(worldPos, _config.base.windowContext);
            
            // �X�N���[�����W���L���͈͓����`�F�b�N
            if (screenPos.z < 0.0f || screenPos.z > 1.0f)
            {
                continue;
            }

            // �~�`�͈͓����`�F�b�N
            Vector2F screenPos2D = { screenPos.x, screenPos.y };
            if (IsPointInCircle(screenPos2D, _config.center, _config.radius))
            {
                detectedTargets_.emplace_back(worldPos, screenPos, obj->GetEntityId());
            }
        }
    }

    void CircleDetector::UpdateAndSetDetection(CircleDetectorConfig&& _config)
    {
        config = std::move(_config);
        UpdateDetection();
    }

    bool CircleDetector::HasDetectedTargets() const
    {
        return !detectedTargets_.empty();
    }

    RectF CircleDetector::GetDetectionArea() const
    {
        Vector2F ratio = Game::System<Screen>().GetSizeRatio();
        float scale = (std::min)(ratio.x, ratio.y);

        float scaledSize = config.radius * 2.0f * scale;
        Vector2F center = Game::System<Screen>().GetSizeF() * 0.5f;
        Vector2F newPoint = center - Vector2F{ scaledSize, scaledSize } *0.5f;
        return { newPoint,{scaledSize,scaledSize} };
    }

    const std::vector<ScreenCoordContainsInfo>& CircleDetector::GetDetectedTargets() const
    {
        return detectedTargets_;
    }

  

    void CircleDetector::ForEach(std::function<void(ScreenCoordContainsInfo&)> _func)
    {
        for (auto& target : detectedTargets_)
        {
            _func(target);
        }
    }

    void CircleDetector::ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const
    {
        for (const auto& target : detectedTargets_)
        {
            _func(target);
        }
    }

    bool CircleDetector::IsPointInCircle(const Vector2F& point, const Vector2F& center, float radius) const
    {
        float dx = point.x - center.x;
        float dy = point.y - center.y;
        float distanceSquared = dx * dx + dy * dy;
        return distanceSquared <= (radius * radius);
    }
}