#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "../ImGui/imgui.h"

#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiEditorCamera.h"
using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

namespace mtgb
{
	class GameObject;
	class Transform;
	struct Vector3;
	
	class ImGuiRenderer final : public ISystem
	{
	public:
		enum class WindowFlag
		{
			None,
			NoMoveWhenHovered // �}�E�X�J�[�\�����E�B���h�E���ɂ���Ƃ��ړ��֎~
		};
	public:
	
		ImGuiRenderer();
		~ImGuiRenderer();
		void Initialize() override;
		void Update() override;
		void BeginFrame();
		void BeginImGuizmoFrame();
		void SetWindowName(const char* _name);
		
		void Begin(const char* _str, bool* _isOpen = NULL ,WindowFlag _flag = WindowFlag::None);
		
		/// <summary>
		/// ImGuizmo�E�B���h�E��`�悷�邽�߂�RTV���Z�b�g
		/// </summary>
		void SetImGuizmoRenderTargetView();

		void SetGameViewCamera();
		void Draw();
		
		void EndFrame();
		void SetDrawList();
		
		void RenderSceneView();
		bool IsHoveringWindow();
		/*bool IsMouseInGameView();*/
		void UpdateGameViewRect();
		void End();
		void Release();

		/// <summary>
		/// 
		/// </summary>
		void ResetComPtrs();
		/// <summary>
		/// �E�B���h�E���T�C�Y���̏���
		/// </summary>
		/// <param name="width">�V������</param>
		/// <param name="height">�V��������</param>
		void OnResize(UINT width, UINT height);
		
		D3D11_VIEWPORT GetViewport(){ return viewport_; }
		ImGuiEditorCamera& GetEditorCamera() { return *pEditorCamera_; };
		//ImGuizmoManipulator&  Manipulator() { return *pManipulator_; };
	private:
		void CreateD3DResources();
		UINT winWidth_, winHeight_;
		bool isManipulatingGuizmo_;
		ImGuizmoManipulator* pManipulator_;
		ImGuiEditorCamera* pEditorCamera_;

		// Game View�E�B���h�E�̑O�t���[�����
		ImVec2 gameViewPos_;
		ImVec2 gameViewSize_;
		bool gameViewRectValid_;

		//ImGuiIO io;
		ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
		ComPtr<ID3D11ShaderResourceView> pSRV_;
		ComPtr<ID3D11Texture2D> pSRVTexture_;
		ComPtr<ID3D11Texture2D> pTexture_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;
		D3D11_VIEWPORT viewport_;
	};
}