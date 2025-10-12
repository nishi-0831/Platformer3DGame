#pragma once
#include "IShader.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector2.h"
#include "RectInt.h"
#include "Color.h"
#include <string>
#include "Texture2D.h"
#include "RectF.h"
#include "Vector2F.h"

namespace mtgb
{
	class Transform;

	class Sprite : public IShader
	{
		/// <summary>
		/// ���_���
		/// </summary>
		struct Vertex
		{
			Vector3 position;
			Vector3 uv;
		};

		/// <summary>
		/// GPU�ɑ�����
		/// </summary>
		struct ConstantBuffer
		{
			Matrix4x4 g_matrixCameraTranslate;          // �J�������W�ϊ��s��
			Matrix4x4 g_matrixCameraRotation;          // �J�������W�ϊ��s��
			Matrix4x4 g_matrixWorldTranslate;  // ���_���W�ϊ��s��
			Matrix4x4 g_matrixWorldRotation;   // ���_���W�ϊ��s��
			Matrix4x4 g_matrixTexture;         // �e�N�X�`�����W�ϊ��s��
			Vector4 g_color;                   // �F
			Vector2 g_angle;                   // �p�x
		};

	public:
		Sprite();
		~Sprite();

		/// <summary>
		/// �摜��ǂݍ���
		/// </summary>
		/// <param name="_fileName">�摜�̃t�@�C����</param>
		void Load(const std::wstring& _fileName);
		/// <summary>
		/// Sprite��`�悷��
		/// </summary>
		/// <param name="_draw">�`�悷���`</param>
		/// <param name="_cut">�摜���̃g���~���O��`</param>
		/// <param name="_rotationZ">��]�p�x(Degree)</param>
		/// <param name="_color">�`��F</param>
		void Draw(
			const RectF& _draw,
			const float _rotationZ,
			const RectF& _cut,
			const Color& _color);

		/// <summary>
		/// Transform�����Ƃɕ`�悷��
		/// </summary>
		/// <param name="_pTransform">�摜��Transform</param>
		/// <param name="_pCameraTransform">
		/// <para>�J������Transform</para>
		/// <para>nullptr�w��\</para>
		/// </param>
		/// <param name="_imageSize">�摜�T�C�Y</param>
		/// <param name="_color">�F</param>
		void Draw(
			const Transform* _pTransform,
			const Transform* _pCameraTransform,
			const Vector2Int& _imageSize,
			const Color& _color);

		/// <summary>
		/// �ǂݍ��܂ꂽ�摜�̃T�C�Y���擾
		/// </summary>
		/// <returns></returns>
		inline const Vector2Int GetSize() const { return texture2D_.GetSize(); }

		inline std::wstring GetFileName() { return fileName_; }

	private:
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

	private:
		Texture2D texture2D_;  // 2D�̃e�N�X�`��
		std::wstring fileName_;  // �ǂݍ��񂾃t�@�C��
	};
}
