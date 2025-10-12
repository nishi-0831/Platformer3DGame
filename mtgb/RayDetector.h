#pragma once

#include "IDetector.h"
#include "DetectorConfigs.h"
#include <functional>

namespace mtgb
{
    /// <summary>
    /// ���C�����E�p�x�ɂ��Ώی��o�V�X�e��
    /// </summary>
    class RayDetector : public IDetector
    {
    public:
        RayDetector() = default;
        RayDetector(const RayDetectorConfig& _config);
        RayDetector(RayDetectorConfig&& _config);
        ~RayDetector() = default;

        /// <summary>
        /// ���o�����s
        /// </summary>
        void UpdateDetection() override;

        /// <summary>
        /// ���o�����s�i�ݒ�w��j
        /// </summary>
        void UpdateDetection(const RayDetectorConfig& _config);

        /// <summary>
        /// �ݒ���X�V���Č��o�����s
        /// </summary>
        void UpdateAndSetDetection(RayDetectorConfig&& _config);

        /// <summary>
        /// ���o���ꂽ�Ώۂ����邩�ǂ���
        /// </summary>
        bool HasDetectedTargets() const override;

        /// <summary>
        /// ���o���ʂ��擾
        /// </summary>
        const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const override;

        void ForEach(std::function<void(ScreenCoordContainsInfo&)> _func) override;
        void ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const override;

        /// <summary>
        /// �^�[�Q�b�g�����C�̊p�x�͈͓��ɂ��邩�`�F�b�N
        /// </summary>
        bool IsTargetInRayAngle(const Vector3& targetPos, const Vector3& rayOrigin, 
                               const Vector3& rayDirection, float maxAngleDegrees) const;

        RayDetectorConfig config;
    };
}