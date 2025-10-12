#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include <map>
#include "Vector2Int.h"
#include "Vector2F.h"

namespace mtgb
{
	class Sprite;

	class Image : public ISystem
	{
	public:
		Image();
		~Image();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// �摜�n���h������X�v���C�g�̃|�C���^���擾
		/// </summary>
		/// <param name="_imageHandle"></param>
		/// <returns></returns>
		inline Sprite* GetSprite(const ImageHandle _imageHandle)
		{
			massert((0 < _imageHandle) && (_imageHandle <= Image::handleCounter_)
				&& "�����ȃn���h�� @Image::GetSprite");

			massert(Image::sprites_.count(_imageHandle) != 0
				&& "�X�v���C�g�����݂��Ȃ� @Image::GetSprite");

			return Image::sprites_[_imageHandle];
		}
	//private:
		
		/// <summary>
		/// �摜��ǂݍ��݁A�n���h�����擾
		/// </summary>
		/// <param name="_fileName">�t�@�C���p�X</param>
		/// <returns>�摜�̃n���h��</returns>
		static ImageHandle Load(const std::string_view& _fileName);
		/// <summary>
		/// �ǂݍ��񂾉摜�̃T�C�Y���擾
		/// </summary>
		/// <param name="_imageHandle">�摜�̃n���h��</param>
		/// <returns>�摜�̏c���T�C�Y</returns>
		static const Vector2Int GetSize(const ImageHandle _imageHandle);
		static const Vector2F GetSizeF(const ImageHandle _imageHandle);
	private:
		ImageHandle handleCounter_;               // �n���h���̉��Z�p
		std::map<ImageHandle, Sprite*> sprites_;  // �ǂݍ��܂ꂽ�X�v���C�g
	};
}
