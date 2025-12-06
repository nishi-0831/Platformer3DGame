#pragma once
#include <functional>
#include <map>
#include <cassert>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <initializer_list>
#include "ISystem.h"
#include "WindowContext.h"
#include "WindowContextResource.h"
#include "ReleaseUtility.h"
#include "ResourceCollection.h"
typedef struct HWND__* HWND;

namespace mtgb
{
	/// <summary>
	/// ウィンドウごとのリソースを管理するクラス
	/// </summary>
	class WindowContextResourceManager : public ISystem
	{
	private:
		// 現在のウィンドウ
		WindowContext currentContext_;
		// ウィンドウごとのリソース群
		std::map<WindowContext, ResourceCollection> collectionMap_;
		// リソースごとの生成関数
		std::map<std::type_index, std::function<WindowContextResource* (WindowContext)>> factoryMap_;
		
		std::vector<std::type_index> insertionOrder_;
		template<typename ResourceT>
		void RegisterFactory();
	public:
		WindowContextResourceManager();
		~WindowContextResourceManager();
		void Initialize() override;
		void Update() override;
		void Release();

		/// <summary>
		/// ウィンドウごとに必要なリソースを作成
		/// </summary>
		/// <param name="windowContext">作成するウィンドウの識別子</param>
		void CreateResource(WindowContext windowContext);

		/// <summary>
		/// リソースを切り替える
		/// </summary>
		/// <param name="windowContext">切り替えるウィンドウの識別子</param>
		void ChangeActiveResource(WindowContext windowContext);

		/// <summary>
		/// 全リソースのOnResizeを呼びだす
		/// </summary>
		/// <param name="windowContext"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void OnResizeAll(WindowContext windowContext, UINT width, UINT height);

		/// <summary>
		/// 指定した二つのウィンドウの指定したリソースを交換する
		/// 対応する入力機器を取り替える際などに使う
		/// </summary>
		/// <typeparam name="ResourceT">指定するリソースの型</typeparam>
		/// <param name="context1">デフォルトでFirst</param>
		/// <param name="context2">デフォルトでSecond</param>
		template<typename ResourceT>
		void SwapResource(WindowContext context1 = WindowContext::First, WindowContext context2 = WindowContext::Second);

		/// <summary>
		/// リソースを取得する
		/// </summary>
		/// <typeparam name="ResourceT">リソースの型</typeparam>
		/// <param name="windowContext">取得するウィンドウの識別子</param>
		/// <returns>リソースの参照</returns>
		template<typename ResourceT>
		ResourceT& Get(WindowContext windowContext)
		{
			auto itr = collectionMap_.find(windowContext);
			assert(itr != collectionMap_.end() && "指定されたWindowContextのリソースが見つかりません");

			static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
				"WindowContextResourceを継承していないクラスのインスタンスは取得できません。");

			return itr->second.Get<ResourceT>();
		}

		/// <summary>
		/// 現在有効なウィンドウの識別子を返す
		/// </summary>
		/// <returns>現在のWindowContext</returns>
		WindowContext CurrCtx()
		{
			return currentContext_;
		}

		/// <summary>
		/// 型を指定して登録する関数
		/// </summary>
		/// <typeparam name="...Args">指定した順番に初期化される</typeparam>
		template<class... Args>
		void RegisterResourceTypes()
		{
			static constexpr bool allAreBaseOfWindowContextResource =
				(std::is_base_of_v<WindowContextResource, Args> && ...);

			static_assert(allAreBaseOfWindowContextResource, "Args...はWindowContextResourceの派生型である必要があります");
		   
			(insertionOrder_.push_back(typeid(Args)), ...);
			(RegisterFactory<Args>(), ...);
		}
	};

	template<typename ResourceT>
	inline void WindowContextResourceManager::RegisterFactory()
	{
		factoryMap_[typeid(ResourceT)] = [](WindowContext _context) -> WindowContextResource*
			{
				return new ResourceT(_context);
			};
	}

	template<typename ResourceT>
	inline void WindowContextResourceManager::SwapResource(WindowContext context1, WindowContext context2)
	{
		// 同じウィンドウを指定していないか確認
		if (context1 == context2)
		{
			return;
		}

		// context1,2のResourceCollectionが登録されている確認
		auto itr1 = collectionMap_.find(context1);
		auto itr2 = collectionMap_.find(context2);

		assert(itr1 != collectionMap_.end() && "指定されたWindowContextが見つかりません");
		assert(itr2 != collectionMap_.end() && "指定されたWindowContextが見つかりません");

		std::type_index typeIdx = typeid(ResourceT);

		// 入れ替える
		std::swap(itr1->second[typeIdx], itr2->second[typeIdx]);
	}
}