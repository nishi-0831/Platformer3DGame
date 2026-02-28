#include "WindowContextResourceManager.h"

mtgb::WindowContextResourceManager::WindowContextResourceManager()
	: currentContext_ { WindowContext::FIRST }
{
}

mtgb::WindowContextResourceManager::~WindowContextResourceManager()
{
	Release();
}

void mtgb::WindowContextResourceManager::Initialize() {}

void mtgb::WindowContextResourceManager::Update()
{
	auto& collection = collectionMap_[currentContext_];

	for (auto& resource : collection)
	{
		resource.second->Update();
	}
}

void mtgb::WindowContextResourceManager::Release()
{
	collectionMap_.clear();
}

void mtgb::WindowContextResourceManager::CreateResource(WindowContext _windowContext)
{
	auto [it, inserted]				  = collectionMap_.try_emplace(_windowContext);
	ResourceCollection& newCollection = it->second;

	// リソースの登録順に作成される
	for (const std::type_index& typeIdx : insertionOrder_)
	{
		auto itr = factoryMap_.find(typeIdx);
		assert(itr != factoryMap_.end() && "指定されたtype_indexのファクトリ関数が見つかりません");

		// ファクトリ関数でリソース作成
		WindowContextResource* pResource = itr->second(_windowContext);
		newCollection.Insert(typeIdx, pResource);
	}
}

void mtgb::WindowContextResourceManager::ChangeActiveResource(WindowContext _windowContext)
{
	auto itr = collectionMap_.find(_windowContext);
	assert(itr != collectionMap_.end() && "指定されたWindowContextのリソースが見つかりません");

	for (auto& collection : itr->second)
	{
		collection.second->SetResource();
	}

	currentContext_ = _windowContext;
}

void mtgb::WindowContextResourceManager::OnResizeAll(WindowContext _windowContext, UINT _width, UINT _height)
{
	auto itr = collectionMap_.find(_windowContext);
	if (itr == collectionMap_.end())
		return;

	// サイズ変更対象のウィンドウのリソース群を取得
	ResourceCollection& resourceCollection = collectionMap_[_windowContext];

	// リソース登録時とは逆順で、サイズ変更のために解放処理を行う
	for (auto itr = insertionOrder_.rbegin(); itr != insertionOrder_.rend(); itr++)
	{
		std::type_index typeIdx = *itr;
		resourceCollection[typeIdx]->Reset();
	}

	// リソース登録時と同じ順番で、サイズ変更後の処理を行う
	for (const std::type_index& typeIdx : insertionOrder_)
	{
		resourceCollection[typeIdx]->OnResize(_width, _height);
	}
}
