#pragma once

#include <vector>
#include <string>
#include "WindowContext.h"
#include "RectContainsInfo.h"
#include "GameObjectTag.h"
#include "RectF.h"
namespace mtgb
{
    /// <summary>
    /// ���o�V�X�e���̃C���^�[�t�F�[�X
    /// </summary>
    class IDetector
    {
    public:
        virtual ~IDetector() = default;

        /// <summary>
        /// ���o�����s
        /// </summary>
        virtual void UpdateDetection() = 0;

        /// <summary>
        /// ���o���ꂽ�Ώۂ����邩�ǂ���
        /// </summary>
        /// <returns>�Ώۂ���ł�����Ȃ� true</returns>
        virtual bool HasDetectedTargets() const = 0;

        /// <summary>
        /// ���o���ʂ��擾
        /// </summary>
        /// <returns>���o���ꂽ�Ώۂ̃��X�g</returns>
        virtual const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const = 0;

        

        /// <summary>
        /// ���o�͈͂�Ԃ�
        /// </summary>
        /// <returns></returns>
        virtual RectF GetDetectionArea() const { return RectF{}; };
        /// <summary>
        /// ���o���ʂ𑀍�
        /// </summary>
        virtual void ForEach(std::function<void(ScreenCoordContainsInfo&)> _func) = 0;
        virtual void ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const = 0;

    protected:
        std::vector<ScreenCoordContainsInfo> detectedTargets_;
    };
}