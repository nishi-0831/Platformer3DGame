#pragma once
#include "cmtgb.h"
#include "Vector4.h"
#include <wrl/client.h>
#include <array>
#include "ShaderType.h"
#include <dxgi.h>
#include "MonitorInfo.h"
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIDevice1;
struct IDXGIAdapter1;
struct IDXGIFactory2;


struct IDXGISwapChain;
struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct D3D11_VIEWPORT;
struct ID3D11BlendState;

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11Texture2D;

namespace mtgb
{
	class Figure;
	class IShader;
	class ImGuiRenderer;

	

	/// <summary>
	/// �u�����h���[�h
	/// </summary>
	enum struct BlendMode : int8_t
	{
		Default,
		Sprite,
		Max,
	};

	/// <summary>
	/// GPU�ōs�������̃o���h��
	/// </summary>
	struct ShaderBundle
	{
		~ShaderBundle();

		/// <summary>
		/// <para>���_���C�A�E�g</para>
		/// <para></para>
		/// </summary>
		ComPtr<ID3D11InputLayout> pVertexLayout;

		/// <summary>
		/// <para>���_�V�F�[�_</para>
		/// <para>���_�̏��</para>
		/// </summary>
		ComPtr<ID3D11VertexShader> pVertexShader;

		/// <summary>
		/// <para>�s�N�Z���V�F�[�_</para>
		/// <para></para>
		/// </summary>
		ComPtr<ID3D11PixelShader> pPixelShader;

		/// <summary>
		/// <para>���X�^���C�U</para>
		/// <para>�ǂ̃s�N�Z�������点�邩�̏��</para>
		/// </summary>
		ComPtr<ID3D11RasterizerState> pRasterizerState;
	};

	/// <summary>
	/// DirectX11�ŕ`�悷����
	/// </summary>
	class DirectX11Draw final
	{
		// TODO: �ɗ�friend�͌��炷
		friend class DirectX11Manager;
		friend class Texture2D;
		friend class IShader;
		friend class DirectWrite;
		friend class Direct2D;
		friend class OBJ;
		friend class ImGuiRenderer;
		friend class FbxParts;
		friend class Trail;
		friend class DoubleWindow;
		friend class SceneSystem;
		friend class RenderSystem;
	public:
		/// <summary>
		/// �`�悷��V�F�[�_���Z�b�g����
		/// </summary>
		/// <param name="_type">�V�F�[�_�̎��</param>
		static void SetShader(const ShaderType _type);
		/// <summary>
		/// �`�悷��u�����h���[�h���Z�b�g����
		/// </summary>
		/// <param name="_mode">�u�����h���[�h</param>
		static void SetBlendMode(const BlendMode _mode);
		/// <summary>
		/// �[�x�o�b�t�@�ւ̏������݂����邩
		/// </summary>
		/// <param name="_enabled">�������݂����� true / false</param>
		static void SetIsWriteToDepthBuffer(const bool _enabled);

	private:
		DirectX11Draw() = delete;
		~DirectX11Draw() = delete;

		/// <summary>
		/// �`��J�n (�L�����o�X�����ꂢ�ɂ���)
		/// </summary>
		static void Begin();

		/// <summary>
		/// �`��I�� (�o�b�N�o�b�t�@�ƃX���b�v����)
		/// </summary>
		static void End();

		/// <summary>
		/// �������
		/// </summary>
		static void Release();

	private:
		static ComPtr<ID3D11Device> pDevice_;                                                             // �`����s�����߂̊��A���\�[�X�̍쐬�Ɏg��
		static ComPtr<ID3D11DeviceContext> pContext_;                                                     // GPU�ɖ��ߏo�����
		static ComPtr<IDXGIDevice1> pDXGIDevice_;
		static std::vector<ComPtr<IDXGIAdapter1>> pDXGIAdapters_;
		static ComPtr<IDXGIFactory2> pDXGIFactory_;
		static std::vector<MonitorInfo> monitorInfos_;

		static IDXGISwapChain* pSwapChain_;                                                        // �_�u���o�b�t�@�����O������
		static ComPtr<IDXGISwapChain1> pSwapChain1_;
		static ComPtr<ID3D11RenderTargetView> pRenderTargetView_;                                         // �`���
		static ComPtr<ID3D11DepthStencilView> pDepthStencilView_;                                         // �[�x�o�b�t�@
		static std::array<ComPtr<ID3D11DepthStencilState>, static_cast<int8_t>(BlendMode::Max)> pDepthStencilState_;  // �u�����h�ɂ��[�x�o�b�t�@�ւ̏������ݏ��
		static ComPtr<ID3D11Texture2D> pDepthStencil_;                                                    // �[�x�X�e���V��
		static std::array<ComPtr<ID3D11BlendState>, static_cast<int8_t>(BlendMode::Max)> pBlendState_;                // �u�����h�̏��
		static ShaderBundle shaderBundle_[static_cast<int8_t>(ShaderType::Max)];                   // �V�F�[�_�̃o���h��
		static Vector4 backgroundColor_;
		static ComPtr<ID3D11SamplerState> pDefaultSamplerState_;
	};
}
