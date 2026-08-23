#pragma once
#include <vector>
#include <queue>
#include <type_traits>
#include <typeindex>
#include <functional>
#include "ShowType.h"
#include "ImGuiShowable.h"
#include "PropertyDisplayRegistry.h"
#include <unordered_map>
#include "cmtgb.h"
#include "Core/GameObject/GameObject.h"

namespace mtgb
{

	struct Vector3;

	using ShowItem	= std::pair<std::string, std::function<void()>>;
	using ShowQueue = std::queue<ShowItem>;

	struct ImGuiWindowState
	{
		std::string selectedName;
		EntityId entityId { INVALID_ENTITY };
		bool isOpen { true };
	};

	/// <summary>
	/// ImGuiに表示をする際に使う
	/// </summary>
	class MTImGui
	{

	  public:
		static void Initialize();
		static void Update();

		/// <summary>
		/// ImGuiウィンドウの表示、キュー内の表示関数の一括実行、ImGuiウィンドウのEnd()までを行う
		/// </summary>
		/// <param name="_showType"></param>
		static void ShowWindow(ShowType _showType);
		/// <summary>
		/// 表示キューを一括実行し、クリア
		/// </summary>
		/// <param name="show"></param>
		static void ExecuteShowQueue(ShowType _show);

		/// <summary>
		/// 型を指定して表示キューに積む
		/// 事前にSetupShowFuncで表示方法を登録する必要あり
		/// </summary>
		/// <typeparam name="T">表示対象の型</typeparam>
		/// <param name="target">表示対象のポインタ</param>
		/// <param name="name">表示対象の名前</param>
		/// <param name="show">表示するImGuiWindow</param>
		template <typename T> static void TypedShow(T* _target, const std::string& _name, ShowType _show);
		/// <summary>
		/// ImGuiShowable*インスタンスを登録、毎回ShowImGuiを呼ぶ
		/// ImGuiShowableは自動で登録される
		/// </summary>
		/// <param name="obj"></param>
		static void Register(ImGuiShowable* _obj);
		/// <summary>
		/// 登録解除
		/// デストラクタで呼ばれる
		/// </summary>
		/// <param name="_obj"></param>
		static void Unregister(ImGuiShowable* _obj);

		/// <summary>
		/// コールバックを表示キューに直接積む
		/// </summary>
		/// <param name="_func">コールバック</param>
		/// <param name="_show">表示場所</param>
		template <typename Func>
			requires std::is_invocable_v<Func>
		static void DirectShow(Func&& _func, const std::string& _name, ShowType _show);

		/// <summary>
		/// <para> ImGuiWindowに線分を描画 </para>
		/// </summary>
		/// <param name="_from">始点</param>
		/// <param name="_to">終点</param>
		/// <param name="_thickness">線の太さ</param>
		static void DrawLine(const Vector3& _from, const Vector3& _to, float _thickness);

		/// <summary>
		/// <para> ImGuiWindowにベクトルを描画 </para>
		/// <para> 始点 + ベクトルで描画される </para>
		/// </summary>
		/// <param name="_start">始点</param>
		/// <param name="_vec">ベクトル</param>
		/// <param name="_thickness"></param>
		static void DrawVec(const Vector3& _start, const Vector3& _vec, float _thickness);

		/// <summary>
		/// ImGuiWindowに円錐を描画
		/// </summary>
		/// <param name="_position"> 円錐の頂点 </param>
		/// <param name="_direction"> 円錐の向き </param>
		/// <param name="_fovAngleDegree"> 視野角(度) </param>
		/// <param name="_distance"> 円錐の高さ </param>
		/// <param name="_thickness"> 線の太さ </param>
		/// <param name="_segments"> 円の分割数(多いほど滑らか) </param>
		static void DrawCone(
			const Vector3& _position,
			const Vector3& _direction,
			float _fovAngleDegree,
			float _distance,
			float _thickness = 1.0f,
			int _segments	 = 16
		);
		static const char* GetName(ShowType _showType)
		{
			if (_showType == ShowType::SCENE_VIEW)
			{
				return "Game View";
			}
			if (_showType == ShowType::SETTINGS)
			{
				return "Settings";
			}
			if (_showType == ShowType::EDITOR)
			{
				return "Editor";
			}
			if (_showType == ShowType::DOCKSPACE)
			{
				return "DOCKSPACE";
			}
			return "None";
		}

		static void ClearShowQueue();
		static void SetWindowOpen(ShowType _showType, bool _flag);
		static void SetAllWindowOpen(bool _flag);
		static void ChangeWindowOpen(ShowType _showType);
		static void ChangeAllWindowOpen();
		static void ShowLog();
		static void ShowComponents(EntityId _entityId);
		template <typename T> static void RegisterComponentViewer();

	  private:
		static MTImGui& Instance();
		MTImGui();
		MTImGui(const MTImGui& _other) = delete;
		MTImGui(MTImGui&& _other)	   = delete;
		~MTImGui();

		/// <summary>
		/// TypedShowの_targetの型に対応する関数を登録する
		/// </summary>
		void SetupShowFunc();
		void ShowListView(ShowType _show);
		std::function<void()> GetSelectedFunc(ShowType _show);

		std::vector<ImGuiShowable*> showableObjs_;

		std::unordered_map<ShowType, ShowQueue> showQueues_;
		std::unordered_map<ShowType, ImGuiWindowState> imguiWindowStates_; // ShowTypeごとのウィンドウの状態

		std::unordered_map<std::type_index, std::function<void(EntityId)>> componentShowFuncs_;
		std::queue<std::function<void()>> sceneViewShowList_;

		void DrawRayImpl(const Vector3& _start, const Vector3& _dir, float _thickness);
		void DrawLineImpl(const Vector3& _from, const Vector3& _to, float _thickness);
		bool updatingImGuiShowable_;
	};

	template <typename T> inline void MTImGui::TypedShow(T* _target, const std::string& _name, ShowType _show)
	{
		using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
		// PushShowFunc( [=] {proxy->ShowImGui(std::any(target), name); }, show);
		DirectShow(
			[=]()
			{
				PropertyDisplayRegistry::Instance().ShowProperty<Type>(_target, _name.c_str());
			},
			_name,
			_show
		);
	}
	template <typename Func>
		requires std::is_invocable_v<Func>
	inline void MTImGui::DirectShow(Func&& _func, const std::string& _name, ShowType _show)
	{
		if (_show == ShowType::SCENE_VIEW)
		{
			// SceneViewは名前不要
			Instance().sceneViewShowList_.push(std::forward<Func>(_func));
		}
		else
		{
			Instance().showQueues_[_show].emplace(_name, std::forward<Func>(_func));
		}
	}
	template <typename T> void mtgb::MTImGui::RegisterComponentViewer()
	{
		std::type_index typeIdx(typeid(T));

		Instance().componentShowFuncs_.emplace(
			typeIdx,
			[](EntityId _entityId)
			{
				GameObject* obj	 = mtgb::GameObject::FindGameObject(_entityId);
				std::string name = obj->GetName() + ":Components";
				PropertyDisplayRegistry::Instance().ShowProperty<T>(&(T::Get(_entityId)), name.c_str());
			}
		);
	}

} // namespace mtgb