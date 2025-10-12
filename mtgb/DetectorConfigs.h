#pragma once

#include "WindowContext.h"
#include "RectF.h"
#include "Vector2F.h"
#include "Vector3.h"
#include "GameObjectTag.h"
#include "Transform.h"
#include "UIDrawCommand.h"
namespace mtgb
{
    /// <summary>
    /// ���o��̋��ʐݒ�
    /// </summary>
    struct DetectorConfigBase
    {
        //std::string targetName;         // ���o�Ώۂ̖��O
        GameObjectTag targetTag;        // ���o�Ώۂ̃^�O
        WindowContext windowContext;    // �Ώۂ̃E�B���h�E
        float maxDistance;              // ���肷��ő勗��
        float minDistance = 0.0f;       // ���肷��ŏ�����
        UIParams uiParams;

    };

    /// <summary>
    /// ��`���o��̐ݒ�
    /// </summary>
    struct RectDetectorConfig
    {
        DetectorConfigBase base;
        RectF detectionRect;            // ���o�͈͂̋�`
    };

    /// <summary>
    /// �~���o��̐ݒ�
    /// </summary>
    struct CircleDetectorConfig
    {
        DetectorConfigBase base;
        Vector2F center;                // �~�̒��S���W�i�X�N���[�����W�j
        float radius;                   // �~�̔��a
    };

    /// <summary>
    /// ���C���o��̐ݒ�
    /// </summary>
    struct RayDetectorConfig
    {
        DetectorConfigBase base;
        Transform* rayTransform;           // ���C�̋N�_�A����
        float maxAngleDegrees;            // ���o�\�ȍő�p�x�i�x�j
    };
}