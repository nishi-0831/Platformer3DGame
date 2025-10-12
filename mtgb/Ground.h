#pragma once
#include "IShader.h"
#include <fbxsdk.h>

namespace mtgb
{
	class Ground : public IShader
	{
	public:
		struct Vertex
		{
			Vector3 position;  // ���W
			Vector3 normal;  // �@��
			Vector3 uv;  // uv���W
		};

		struct ConstantBuffer
		{
			Matrix4x4 g_matrixWorldViewProj;  // ���[���h�E�r���[�E�v���W�F�N�V�����s��
			Matrix4x4 g_matrixNormalTrans;    // ��]�s��Ɗg��s��̋t�s��
			Matrix4x4 g_matrixWorld;  // ���[���h�s��
			Vector4 g_lightDirection;  // ���C�g�̌���
			Vector4 g_diffuse;  // �������������Ƃ��ւ̊g�U���ˌ�(�}�e���A���F)
			Vector4 g_ambient;  // �S�̓I�Ȋ��� (����������Ȃ��ꏊ�ɂ����邭)
			Vector4 g_speculer;  // ���ʔ��� (Lambert�̏ꍇ��0)
			Vector4 g_cameraPosition; // �J�����̈ʒu�i�n�C���C�g�̌v�Z�ɕK�v�j
			FLOAT g_shininess;     // �X�y�L�����̋���
			BOOL g_isTexture;  // �e�N�X�`���̗L��
		};

		Ground(FbxNode* _pParent);
		~Ground();

		void Initialize() override;
		void Release() override;

		void Draw();

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

		/// <summary>
		/// �C���f�b�N�X�̐�
		/// </summary>
		/// <returns>�C���f�b�N�X�̐�</returns>
		inline uint32_t GetIndexCount() const
		{
			// �C���f�b�N�X�� = 1�|���S�� �ɂ� 3���_ (3�p������Ă��邽��)
			return polygonCount_ * 3;
		}
		/// <summary>
		/// �|���S�������璸�_�������߂�
		/// </summary>
		/// <param name="_polygonCount">�|���S����</param>
		/// <returns>���_��</returns>
		inline uint32_t GetVertexCount(const int _polygonCount = 1) const
		{
			// ���_�� = 1�|���S�� �ɂ� 3���_
			return _polygonCount * 3;
		}

	private:
		uint32_t vertexCount_;  // ���_��
		uint32_t polygonCount_;  // �|���S����
		uint32_t indexCount_;  // �C���f�b�N�X��
		uint32_t materialCount_;  // �}�e���A����
		uint32_t polygonVertexCount_;  // �|���S���̒��_��

		FbxNode* pNode_;  // Fbx�m�[�h���
		FbxMesh* pMesh_;  // ���b�V��

		Vertex* pVertexes_;  // ���_���
		DWORD* pIndexData_;  // �C���f�b�N�X���

		ID3D11Buffer** ppIndexBuffer_;
	};
}

