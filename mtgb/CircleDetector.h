#pragma once

#include "IDetector.h"
#include "DetectorConfigs.h"
#include <functional>
#include "UIDrawCommand.h"
namespace mtgb
{
    /// <summary>
    /// �~�`�͈͓��̑Ώی��o�V�X�e��
    /// </summary>
    class CircleDetector : public IDetector
    {
    public:
        CircleDetector();
        CircleDetector(const CircleDetectorConfig& _config);
        CircleDetector(CircleDetectorConfig&& _config);
        ~CircleDetector() = default;

        /// <summary>
        /// ���o�����s
        /// </summary>
        void UpdateDetection() override;

        /// <summary>
        /// ���o�����s�i�ݒ�w��j
        /// </summary>
        void UpdateDetection(const CircleDetectorConfig& _config);

        /// <summary>
        /// �ݒ���X�V���Č��o�����s
        /// </summary>
        void UpdateAndSetDetection(CircleDetectorConfig&& _config);

        /// <summary>
        /// ���o���ꂽ�Ώۂ����邩�ǂ���
        /// </summary>
        bool HasDetectedTargets() const override;

        RectF GetDetectionArea() const override;
        /// <summary>
        /// ���o���ʂ��擾
        /// </summary>
        const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const override;

        
        void ForEach(std::function<void(ScreenCoordContainsInfo&)> _func) override;
        void ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const override;

        /// <summary>
        /// �_���~���ɂ��邩�`�F�b�N
        /// </summary>
        bool IsPointInCircle(const Vector2F& point, const Vector2F& center, float radius) const;

        CircleDetectorConfig config;
    };
}