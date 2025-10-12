#pragma once
#include <functional>
#include <d3d11.h>
#include "DirectX11Draw.h"
#include "Screen.h"
#include "Transform.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;

namespace mtgb
{
	class IShader
	{
	public:
		IShader();
		virtual ~IShader()
		{}

		/// <summary>
		/// �V�F�[�_�̏���������
		/// </summary>
		virtual void Initialize();
		//virtual void Draw() = 0;
		virtual void Release();

	protected:
		/// <summary>
		/// ���_�o�b�t�@�̏�����
		/// </summary>
		virtual void InitializeVertexBuffer(ID3D11Device* _pDevice) = 0;
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̏�����
		/// </summary>
		virtual void InitializeIndexBuffer(ID3D11Device* _pDevice) = 0;
		/// <summary>
		/// �R���X�^���g�o�b�t�@�̏�����
		/// </summary>
		virtual void InitializeConstantBuffer(ID3D11Device* _pDevice) = 0;

		/// <summary>
		/// �V�F�[�_�̕`�揈��(����)
		/// </summary>
		/// <typeparam name="ConstantBufferT">�R���X�^���g�o�b�t�@�̌^</typeparam>
		/// <typeparam name="VertexT">���_�̌^</typeparam>
		/// <param name="_makeConstantBufferCallback">
		/// <para>�R���X�^���g�o�b�t�@�ɓ����p�̃R�[���o�b�N�֐�</para>
		/// <para>�R�[���o�b�N�֐�:(ConstantBufferT* _pCB)->void</para>
		/// </param>
		/// <param name="_contextSetterCallback">
		/// <para>�R���X�^���g�o�b�t�@���M��̃R���e�L�X�g�����̃R�[���o�b�N�֐�</para>
		/// <para>�R�[���o�b�N�֐�:(ID3D11DeviceContext*)->void</para>
		/// </param>
		template<typename ConstantBufferT, typename VertexT>
		void Draw(
			const std::function<void(ConstantBufferT* _pConstantBuffer)>& _makeConstantBufferCallback,
			const std::function<void(ID3D11DeviceContext* _pContext)>& _contextSetterCallback,
			const int _drawIndexCount = 6);
		template<typename ConstantBufferT, typename VertexT>
		void Draw(
			const std::function<void(VertexT* _pVertex)>& _makeVertexBufferCallback,
			const std::function<void(ConstantBufferT* _pConstantBuffer)>& _makeConstantBufferCallback,
			const std::function<void(ID3D11DeviceContext* _pContext)>& _contextSetterCallback,
			const int _drawIndexCount = 6);

		/// <summary>
		/// �J�����̍��W�n���擾
		/// </summary>
		/// <returns>�J����Transfrom�̎Q�ƃ|�C���^</returns>
		//const mtgb::Transform& GetCameraTransfrom();

	protected:
		ComPtr<ID3D11Buffer> pVertexBuffer_;    // ���_�̈ʒu�A�F
		ComPtr<ID3D11Buffer> pIndexBuffer_;     // �|���S�������ԏ���
		ComPtr<ID3D11Buffer> pConstantBuffer_;  //�@�V�F�[�_�̃O���[�o���ϐ�
	};

	template<typename ConstantBufferT, typename VertexT>
	inline void IShader::Draw(
		const std::function<void(ConstantBufferT*)>& _makeConstantBufferCallback,
		const std::function<void(ID3D11DeviceContext*)>& _contextSetterCallback,
		const int _drawIndexCount)
	{
		UINT stride{ 0U };
		UINT offset{ 0U };

		stride = sizeof(VertexT);
		offset = 0;
		DirectX11Draw::pContext_->IASetVertexBuffers(0U, 1U, pVertexBuffer_.GetAddressOf(), &stride, &offset);  // ���_�o�b�t�@���Z�b�g
		DirectX11Draw::pContext_->IASetIndexBuffer(pIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);  // �C���f�b�N�X�o�b�t�@���Z�b�g
		DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());  // ���_�V�F�[�_�̃R���X�^���g�o�b�t�@���Z�b�g
		DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());  // �s�N�Z���V�F�[�_�̃R���X�^���g�o�b�t�@���Z�b�g

		ConstantBufferT constantBuffer{};

		_makeConstantBufferCallback(&constantBuffer);

		HRESULT hResult{};

		// �V�F�[�_�ɓn�����߂̃f�[�^
		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};

		// GPU����̃f�[�^�A�N�Z�X�������~�߂�
		hResult = DirectX11Draw::pContext_->Map(
			pConstantBuffer_.Get(),
			NULL,
			D3D11_MAP_WRITE_DISCARD,
			NULL,
			&mappedSubresource);

		massert(SUCCEEDED(hResult)  // GPU�f�[�^�A�N�Z�X�����~�߂ɐ���
			&& "GPU�f�[�^�A�N�Z�X�����~�߂Ɏ��s");

		// �f�[�^��������
		memcpy_s(
			mappedSubresource.pData,
			mappedSubresource.RowPitch,
			static_cast<void*>(&constantBuffer),
			sizeof(ConstantBufferT));

		// GPU�f�[�^�A�N�Z�X�����~�߉���
		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		_contextSetterCallback(DirectX11Draw::pContext_.Get());

		DirectX11Draw::pContext_->DrawIndexed(_drawIndexCount, 0, 0);
	}

	template<typename ConstantBufferT, typename VertexT>
	inline void IShader::Draw(
		const std::function<void(VertexT* _pVertex)>& _makeVertexBufferCallback,
		const std::function<void(ConstantBufferT* _pConstantBuffer)>& _makeConstantBufferCallback,
		const std::function<void(ID3D11DeviceContext* _pContext)>& _contextSetterCallback,
		const int _drawIndexCount)
	{
		UINT stride{ 0U };
		UINT offset{ 0U };

		stride = sizeof(VertexT);
		offset = 0;
		DirectX11Draw::pContext_->IASetVertexBuffers(0U, 1U, pVertexBuffer_.Get(), &stride, &offset);  // ���_�o�b�t�@���Z�b�g
		DirectX11Draw::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);  // �C���f�b�N�X�o�b�t�@���Z�b�g
		DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.Get());  // ���_�V�F�[�_�̃R���X�^���g�o�b�t�@���Z�b�g
		DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, pConstantBuffer_.Get());  // �s�N�Z���V�F�[�_�̃R���X�^���g�o�b�t�@���Z�b�g

		ConstantBufferT constantBuffer{};

		_makeConstantBufferCallback(&constantBuffer);

		HRESULT hResult{};

		// �V�F�[�_�ɓn�����߂̃f�[�^
		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};

		// GPU����̃f�[�^�A�N�Z�X�������~�߂�
		hResult = DirectX11Draw::pContext_->Map(
			pConstantBuffer_.Get(),
			NULL,
			D3D11_MAP_WRITE_DISCARD,
			NULL,
			&mappedSubresource);

		massert(SUCCEEDED(hResult)  // GPU�f�[�^�A�N�Z�X�����~�߂ɐ���
			&& "GPU�f�[�^�A�N�Z�X�����~�߂Ɏ��s");

		// �f�[�^��������
		memcpy_s(
			mappedSubresource.pData,
			mappedSubresource.RowPitch,
			static_cast<void*>(&constantBuffer),
			sizeof(ConstantBufferT));

		// GPU�f�[�^�A�N�Z�X�����~�߉���
		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		// �V�F�[�_�ɓn�����߂̃f�[�^
		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};

		// GPU����̃f�[�^�A�N�Z�X�������~�߂�
		hResult = DirectX11Draw::pContext_->Map(
			pConstantBuffer_.Get(),
			NULL,
			D3D11_MAP_WRITE_DISCARD,
			NULL,
			&mappedSubresource);

		massert(SUCCEEDED(hResult)  // GPU�f�[�^�A�N�Z�X�����~�߂ɐ���
			&& "GPU�f�[�^�A�N�Z�X�����~�߂Ɏ��s");

		// �f�[�^��������
		memcpy_s(
			mappedSubresource.pData,
			mappedSubresource.RowPitch,
			static_cast<void*>(&constantBuffer),
			sizeof(ConstantBufferT));

		// GPU�f�[�^�A�N�Z�X�����~�߉���
		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		_contextSetterCallback(DirectX11Draw::pContext_.Get());

		DirectX11Draw::pContext_->DrawIndexed(_drawIndexCount, 0, 0);
	}
}
