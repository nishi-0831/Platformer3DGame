#pragma once
#include "IShader.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "RectInt.h"
#include "RectF.h"
#include "Color.h"

namespace mtgb
{
	class Figure : public IShader
	{
		/// <summary>
		/// ���_���
		/// </summary>
		struct Vertex
		{
			Vector3 position;  // ���W
		};

		/// <summary>
		/// GPU�ɑ�����
		/// </summary>
		struct ConstantBuffer
		{
			Vector4 g_color;  // �F
			Matrix4x4 g_worldMatrix;  // ���_���W�ϊ��s��
		};

	public:
		Figure();
		~Figure();

		void Draw(
			const RectInt& _rect,
			const Color& _color);
		void Draw(
			const RectF& _rect,
			const Color& _color);
	private:
		/// <summary>
		/// ���_�o�b�t�@�̏�����
		/// </summary>
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̏�����
		/// </summary>
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// �R���X�^���g�o�b�t�@
		/// </summary>
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;
	};
}
