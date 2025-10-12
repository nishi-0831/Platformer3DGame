#pragma once
#include "ISystem.h"
#include "Vector2Int.h"
#include "Vector2F.h"
namespace mtgb
{
	class Screen : public ISystem
	{
	public:
		Screen();
		~Screen();

		void Initialize() override;
		void Update() override;

		inline const Vector2Int GetSize() const { return size_; }
		inline const Vector2F GetSizeF() const { return Vector2F{ static_cast<float>(size_.x),static_cast<float>(size_.y) }; }
		//inline const Vector2Int GetInitialSize() const { return Vector2Int{ initialWidth_,initialHeight_ }; }
		/// <summary>
		/// �����l��1�Ƃ����Ƃ��̃X�N���[���T�C�Y�̔䗦��Ԃ�
		/// </summary>
		/// <returns></returns>
		const Vector2F GetSizeRatio() const;
		const mtgb::Vector2Int GetInitialSize() const;

		

		/// <summary>
		/// <para> �X�N���[���̃T�C�Y��ێ�����</para>
		/// </summary>
		/// <param name="_width"></param>
		/// <param name="_height"></param>
		void SetSize(int _width, int _height);

	private:
		Vector2F sizeRatio_;
		Vector2Int initialSize_;
		Vector2Int size_;
		//int initialWidth_; // �X�N���[���̉���(�����l)
		//int initialHeight_; // �X�N���[���̏c��(�����l)
		//int width_;     // �X�N���[���̉���
		//int height_;    // �X�N���[���̏c��
		int fpsLimit_;  // FPS���� (1�t���[��������̍X�V�t���[����)
	};
}
