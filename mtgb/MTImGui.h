#pragma once
#include <vector>
#include <queue>
#include <type_traits>
#include <typeindex>
#include <functional>
#include "ShowType.h"
#include "ImGuiShowable.h"
#include "TypeRegistry.h"
#include <optional>
#include <tuple>
#include "ComponentConcept.h"
#include <map>
namespace mtgb
{

	struct Vector3;

	using ShowItem = std::pair<std::string, std::function<void()>>;
	using ShowQueue = std::queue<ShowItem>;

	struct ImGuiWindowState
	{
		std::string selectedName;
		EntityId entityId{ INVALD_ENTITY };
		bool isOpen{ false };
	};

	

	/// <summary>
		/// ImGui�ɕ\��������ۂɎg��
		/// </summary>
	class MTImGui final
	{

	public:
		static MTImGui& Instance()
		{
			static MTImGui instance;
			return instance;
		}

		void Initialize();
		void Update();

		/// <summary>
		/// ImGui�E�B���h�E�̕\���A�L���[���̕\���֐��̈ꊇ���s�AImGui�E�B���h�E��End()�܂ł��s��
		/// </summary>
		/// <param name="_showType"></param>
		void ShowWindow(ShowType _showType);
		/// <summary>
		/// �\���L���[���ꊇ���s���A�N���A
		/// </summary>
		/// <param name="show"></param>
		void ExecuteShowQueue(ShowType show);

		/// <summary>
		/// �^���w�肵�ĕ\���L���[�ɐς�
		/// ���O��SetupShowFunc�ŕ\�����@��o�^����K�v����
		/// </summary>
		/// <typeparam name="T">�\���Ώۂ̌^</typeparam>
		/// <param name="target">�\���Ώۂ̃|�C���^</param>
		/// <param name="name">�\���Ώۂ̖��O</param>
		/// <param name="show">�\������ImGuiWindow</param>
		template<typename T>
		void TypedShow(T* target, const std::string& name, ShowType show = ShowType::Inspector);
		/// <summary>
		/// ImGuiShowable*�C���X�^���X��o�^�A����ShowImGui���Ă�
		/// ImGuiShowable�͎����œo�^�����
		/// </summary>
		/// <param name="obj"></param>
		void Register(ImGuiShowable* obj);
		/// <summary>
		/// �o�^����
		/// �f�X�g���N�^�ŌĂ΂��
		/// </summary>
		/// <param name="obj"></param>
		void Unregister(ImGuiShowable* obj);

		/// <summary>
		/// �R�[���o�b�N��\���L���[�ɒ��ڐς�
		/// </summary>
		/// <param name="func">�R�[���o�b�N</param>
		/// <param name="show">�\���ꏊ</param>
		void DirectShow(std::function<void()> func, const std::string& name, ShowType show);

		/// <summary>
		/// <para> ImGuiWindow�ɐ�����`�� </para>
		/// </summary>
		/// <param name="_from">�n�_</param>
		/// <param name="_to">�I�_</param>
		/// <param name="_thickness">���̑���</param>
		void DrawLine(const Vector3& _from, const Vector3& _to, float _thickness);

		
		/// <summary>
		/// <para> ImGuiWindow�Ƀx�N�g����`�� </para>
		/// <para> �n�_ + �x�N�g���ŕ`�悳��� </para>
		/// </summary>
		/// <param name="_start">�n�_</param>
		/// <param name="_vec">�x�N�g��</param>
		/// <param name="_thickness"></param>
		void DrawVec(const Vector3& _start, const Vector3 & _vec, float _thickness);

		static const char* GetName(ShowType _showType)
		{
			if (_showType == ShowType::Inspector)
			{
				return "Inspector";
			}
			if (_showType == ShowType::SceneView)
			{
				return "Game View";
			}
			if (_showType == ShowType::Settings)
			{
				return "Settings";
			}

			return "None";
		}

		void SetWindowOpen(ShowType _showType, bool _flag);
		void SetAllWindowOpen(bool _flag);
		void ChangeWindowOpen(ShowType _showType);
		void ChangeAllWindowOpen();
		void ShowLog();
		void ShowComponents(EntityId _entityId);
	private:
		MTImGui();
		MTImGui(const MTImGui& other) = delete;
		~MTImGui();

		/// <summary>
		/// TypedShow��_target�̌^�ɑΉ�����֐���o�^����
		/// </summary>
		void SetupShowFunc();

		void ShowListView(ShowType _show);

		std::vector<ImGuiShowable*> showableObjs_;
		
		std::map<ShowType, ShowQueue> showQueues_;
		std::map<ShowType, ImGuiWindowState> imguiWindowStates_; // ShowType���Ƃ̃E�B���h�E�̏��

		void RegisterAllComponentViewers();
		template<ComponentT T>
		void RegisterComponentViewer();

		std::unordered_map<std::type_index, std::function<void(EntityId)>> componentShowFuncs_;
		std::queue<std::function<void()>> sceneViewShowList_;

		void DrawRayImpl(const Vector3& _start, const Vector3& _dir, float _thickness);
		void DrawLineImpl(const Vector3& _from, const Vector3& _to, float _thickness);

		bool updatingImGuiShowable_;
	};

	template<typename T>
	inline void MTImGui::TypedShow(T* target, const std::string& name, ShowType show)
	{
		using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
		//PushShowFunc( [=] {proxy->ShowImGui(std::any(target), name); }, show);
		DirectShow([=]() {TypeRegistry::Instance().CallFunc<Type>(target, name.c_str()); }, name, show);
	}
	
	
}