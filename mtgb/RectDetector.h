#pragma once

#include <vector>
#include <string>
#include <functional>
#include "WindowContext.h"
#include "RectF.h"
#include "RectContainsInfo.h"
#include "GameObjectTag.h"
#include "IDetector.h"
#include "DetectorConfigs.h"
#include "UIDrawCommand.h"
namespace mtgb
{
	/// <summary>
	/// ��`���̑Ώی��o�V�X�e��
	/// </summary>
	class RectDetector : public IDetector
	{
	public:
		RectDetector() = default;
		RectDetector(const RectDetectorConfig& _config);
		RectDetector(RectDetectorConfig&& _config);
		~RectDetector() = default;

		/// <summary>
		/// <para> ���o�����s�A���ʂ��擾</para>
		/// <para> ���O�Ɋ��蓖�Ă�ꂽ�ݒ���g�p</para>
		/// </summary>
		void UpdateDetection() override;

		/// <summary>
		/// <para> ���o�����s�A���ʂ��擾</para>
		/// <para> �����̐ݒ���g���Č��o</para>
		/// <param name="_config"> ���o�Ɏg�p����ݒ�</param>
		void UpdateDetection(RectDetectorConfig& _config);

		/// <summary>
		/// <para> �����̐ݒ�����蓖�āA���o�����s</para>
		/// </summary>
		/// <param name="_config"> ���蓖�āA���o�Ɏg�p����ݒ�</param>
		void UpdateAndSetDetection(RectDetectorConfig& _config);

		/// <summary>
		/// <para> �����̐ݒ�����蓖�āA���o�����s</para>
		/// </summary>
		/// <param name="_config"> ���蓖�āA���o�Ɏg�p����ݒ�</param>
		void UpdateAndSetDetection(RectDetectorConfig&& _config);

		/// <summary>
		/// ���o���ꂽ�Ώۂ����邩�ǂ���
		/// </summary>
		/// <returns>�Ώۂ���ł�����Ȃ� true</returns>
		bool HasDetectedTargets() const override;

		RectF GetDetectionArea() const override;
		/// <summary>
		/// ���o���ʂ��擾
		/// </summary>
		const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const override;

		void ForEach(std::function<void(ScreenCoordContainsInfo&)> _func) override;
		void ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const override;
		
		/// <summary>
		/// <para> �J��������^�[�Q�b�g�Ɏ������ʂ��Ă��邩</para>
		/// <para> !!!detectedTargets���ő�����������Ă���̂Ŕ�����I�B�Q�[�����d���Ȃ�����^����ɉ��P!!!</para>
		/// </summary>
		/// <param name="_cameraPos">�J����</param>
		/// <param name="_targetInfo">�^�[�Q�b�g</param>
		/// <returns></returns>
		bool IsLineOfSight(const Vector3& _cameraPos, const ScreenCoordContainsInfo& _targetInfo);
		
		RectDetectorConfig config;
	};
}