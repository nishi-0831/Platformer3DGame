#pragma once
#include "ISystem.h"
#include <string>
#include <wrl/client.h>
#include "Vector2Int.h"
#include <dxgi1_2.h> 
#include <vector>
#include <optional>
#include "MonitorInfo.h"

#pragma comment(lib, "d3d11.lib")  // DirectX11�̃��C�u����
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")  // �V�F�[�_�R���p�C���p���C�u����

using Microsoft::WRL::ComPtr;
struct D3D11_INPUT_ELEMENT_DESC;
struct CD3D11_RASTERIZER_DESC;
struct D3D11_VIEWPORT;
struct IDXGISwapChain1;
struct IDXGISurface;
struct IDXGIOutput;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
typedef struct HWND__* HWND;

namespace mtgb
{
	enum struct ShaderType : int8_t;

	class DirectX11Manager : public ISystem
	{
	public:
		DirectX11Manager();
		~DirectX11Manager();

		void Initialize() override;
		void Update() override;

		void InitializeCommonResources();
		void CreateDXGISurface(IDXGISwapChain1* pSwapChain1, IDXGISurface** ppDXGISurface);
		
		//void CreateOutput(int index, IDXGIOutput** ppOutput);
		void CreateSwapChain(HWND hWnd, IDXGIOutput* pOutput, IDXGISwapChain1** ppSwapChain1);
		void CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView);
		void CreateViewport(const Vector2Int& size, D3D11_VIEWPORT& viewport);
		void CreateDepthStencilAndDepthStencilView(const Vector2Int bufSize, ID3D11Texture2D** ppDepthStencil, ID3D11DepthStencilView** ppDepthStencilView);


		void ChangeViewport(const D3D11_VIEWPORT& viewport);
		void ChangeRenderTargets(ComPtr<ID3D11RenderTargetView> pRenderTargetView, ComPtr<ID3D11DepthStencilView> pDepthStencilView);
		void ChangeSwapChain(ComPtr<IDXGISwapChain1> pSwapChain1);
				
		void ClearState();
		/// <summary>
		/// ClearState��ɕK�v�ȃf�t�H���g��Ԃ��Đݒ�
		/// </summary>
		void SetDefaultStates();
		
		/// <summary>
		/// ���p�\�ȃ��j�^�[�������Ŋ��蓖�Ă�
		/// </summary>
		/// <param name="ppOutput">����Ƀ��j�^�[�����蓖�Ă�</param>
		/// <returns>���蓖�Ă�ꂽ���j�^�[�̃C���f�b�N�X�A���s���� -1</returns>
		std::optional<MonitorInfo> AssignAvailableMonitor(IDXGIOutput** ppOutput);

		int GetAvailableMonitorCount() const;

		void Release() override;
	private:
		std::vector<DXGI_ADAPTER_DESC1> adaptersDesc_;
		/// <summary>
		/// ���p�\�ȃ��j�^�[��񋓂���
		/// </summary>
		void EnumAvailableMonitors();
		/// <summary>
		/// �V�F�[�_�o���h��������������
		/// </summary>
		void InitializeShaderBundle();

		/// <summary>
		/// �V�F�[�_�t�@�C�����R���p�C������
		/// </summary>
		/// <param name="_fileName">�t�@�C���p�X</param>
		/// <param name="_type">�V�F�[�_�^�C�v</param>
		/// <param name="_pHLSLLayout">���͏��z��</param>
		/// <param name="_layoutLength">���͏��z��̗v�f��</param>
		/// <param name="_pRasterizerDesc">���X�^���C�U�̐ݒ�</param>
		void CompileShader(
			const std::wstring& _fileName,
			const ShaderType& _type,
			const D3D11_INPUT_ELEMENT_DESC* _pHLSLLayout,
			const unsigned int _layoutLength,
			const CD3D11_RASTERIZER_DESC* _pRasterizerDesc);
	};
}
